#include <GellyD3D.h>
#include <GellyFluid.h>

#define SDL_MAIN_HANDLED
#include <DirectXMath.h>
#include <SDL.h>
#include <SDL_syswm.h>
#include <d3d11.h>
#include <windows.h>
#include <wrl.h>

#include <array>
#include <vector>

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_sdl2.h"

using namespace DirectX;
using namespace Microsoft::WRL;

bool g_sdlInitialized = false;
SDL_Window *g_Window = nullptr;
int g_Width = 1366;
int g_Height = 768;
ID3D11Device *g_Device = nullptr;
ID3D11DeviceContext *g_Context = nullptr;
IDXGISwapChain *g_SwapChain = nullptr;
ID3D11RenderTargetView *g_BackBufferRTV = nullptr;
const int g_FrameTimeSampleCount = 512;
std::vector<float> g_FrameTimeSamples;

XMFLOAT3 g_CameraPosition = {0.0f, 0.0f, 0.0f};
XMFLOAT3 g_CameraDirection = {1.f, 0.f, 0.f};
XMFLOAT3 g_WorldUp = {0.f, 0.f, 1.f};

XMFLOAT2 g_LastMousePos = {0.0f, 0.0f};
XMFLOAT2 g_MousePos = {0.0f, 0.0f};
bool g_MouseDragging = false;

XMFLOAT4X4 g_ViewMatrix;
XMFLOAT4X4 g_ProjectionMatrix;

float g_CameraFOVDeg = 60.0f;
float g_CameraNearPlane = 0.1f;
float g_CameraFarPlane = 1000.0f;
float g_CameraPivotRadius = 1.0f;

SolverContext *g_SolverContext = nullptr;
PBFSolver *g_Solver = nullptr;
const int g_MaxParticles = 10000;
float g_ParticleRadius = 0.03f;

ID3D11Buffer *g_ParticlePositions = nullptr;
ID3D11InputLayout *g_ParticlePosLayout = nullptr;
ID3D11Texture2D *g_DSTexture = nullptr;
ID3D11DepthStencilView *g_DSV = nullptr;
ID3D11DepthStencilState *g_DSS = nullptr;
ID3D11RasterizerState *g_RasterizerState = nullptr;

ComPtr<ID3D11VertexShader> g_DebugRenderVS;
ComPtr<ID3DBlob> g_DebugRenderVSBytecode;
ComPtr<ID3D11GeometryShader> g_DebugRenderGS;
ComPtr<ID3D11PixelShader> g_DebugRenderPS;
struct DebugCB {
	XMFLOAT4X4 projection;
	XMFLOAT4X4 view;
	float radius;
	float pad1;
	float pad2;
	float pad3;
};
DebugCB g_DebugCBData{};
d3d11::ConstantBuffer<DebugCB> g_DebugCB;

ID3D11InfoQueue *g_DebugMsgQueue = nullptr;

// clang-format off
static const char *g_DebugRenderVSCode =
	#include "shaders/d3d11/DebugRender.vs.embed.hlsl"
;

static const char *g_DebugRenderGSCode =
	#include "shaders/d3d11/DebugRender.gs.embed.hlsl"
;

static const char *g_DebugRenderPSCode =
	#include "shaders/d3d11/DebugRender.ps.embed.hlsl"
;
// clang-format on

#define LOAD_SHADER_INFO(shaderName)                  \
	options.shader.buffer = (void *)shaderName##Code; \
	options.shader.size = strlen(shaderName##Code);   \
	options.shader.name = #shaderName;                \
	options.shader.entryPoint = "main";

void EnsureDebugCBInitialized() {
	if (!g_Device) {
		return;
	}

	g_DebugCB.Init(g_Device);
}

void EnsureRasterizerInitialized() {
	if (!g_Device) {
		return;
	}

	D3D11_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = false;

	DX("Failed to create the rasterizer state",
	   g_Device->CreateRasterizerState(&rasterizerDesc, &g_RasterizerState));
}

void UpdateDebugCB() {
	XMMATRIX viewMatrix = XMLoadFloat4x4(&g_ViewMatrix);
	XMMATRIX projectionMatrix = XMLoadFloat4x4(&g_ProjectionMatrix);

	XMStoreFloat4x4(&g_DebugCBData.view, XMMatrixTranspose(viewMatrix));
	XMStoreFloat4x4(
		&g_DebugCBData.projection, XMMatrixTranspose(projectionMatrix)
	);
	g_DebugCBData.radius = g_ParticleRadius;

	g_DebugCB.Set(g_Context, &g_DebugCBData);
}

void EnsureDepthResources() {
	if (!g_Device) {
		return;
	}

	D3D11_TEXTURE2D_DESC depthTextureDesc{};
	depthTextureDesc.Width = g_Width;
	depthTextureDesc.Height = g_Height;
	depthTextureDesc.MipLevels = 1;
	depthTextureDesc.ArraySize = 1;
	depthTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthTextureDesc.SampleDesc.Count = 1;
	depthTextureDesc.SampleDesc.Quality = 0;
	depthTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthTextureDesc.CPUAccessFlags = 0;
	depthTextureDesc.MiscFlags = 0;

	DX("Failed to create the depth texture",
	   g_Device->CreateTexture2D(&depthTextureDesc, nullptr, &g_DSTexture));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = depthTextureDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	DX("Failed to create the depth stencil view",
	   g_Device->CreateDepthStencilView(g_DSTexture, &dsvDesc, &g_DSV));

	D3D11_DEPTH_STENCIL_DESC dsDesc{};
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	DX("Failed to create the depth stencil state",
	   g_Device->CreateDepthStencilState(&dsDesc, &g_DSS));
}

void EnsureDebugShadersLoaded() {
	if (!g_Device) {
		return;
	}

	d3d11::ShaderCompileOptions options{};
	options.device = g_Device;
	options.defines = nullptr;

	LOAD_SHADER_INFO(g_DebugRenderVS);
	auto vsResult = d3d11::compile_vertex_shader(options);
	g_DebugRenderVS.Attach(vsResult.shader);
	g_DebugRenderVSBytecode.Attach(vsResult.shaderBlob);

	LOAD_SHADER_INFO(g_DebugRenderGS);
	auto gsResult = d3d11::compile_geometry_shader(options);
	g_DebugRenderGS.Attach(gsResult.shader);

	LOAD_SHADER_INFO(g_DebugRenderPS);
	auto psResult = d3d11::compile_pixel_shader(options);
	g_DebugRenderPS.Attach(psResult.shader);
}

void EnsureParticleLayoutInitialized() {
	if (!g_Device) {
		return;
	}

	D3D11_INPUT_ELEMENT_DESC inputElementDescs[] = {
		{"SV_POSITION",
		 0,
		 DXGI_FORMAT_R32G32B32A32_FLOAT,
		 0,
		 0,
		 D3D11_INPUT_PER_VERTEX_DATA,
		 0},
	};

	DX("Failed to create the particle position input layout",
	   g_Device->CreateInputLayout(
		   inputElementDescs,
		   1,
		   g_DebugRenderVSBytecode->GetBufferPointer(),
		   g_DebugRenderVSBytecode->GetBufferSize(),
		   &g_ParticlePosLayout
	   ));
}

void UpdateCameraMatrices() {
	XMVECTOR cameraPosition = XMLoadFloat3(&g_CameraPosition);
	XMVECTOR cameraDirection = XMLoadFloat3(&g_CameraDirection);
	XMVECTOR worldUp = XMLoadFloat3(&g_WorldUp);

	XMVECTOR cameraRight = XMVector3Cross(worldUp, cameraDirection);
	XMVECTOR cameraUp = XMVector3Cross(cameraDirection, cameraRight);

	XMMATRIX viewMatrix =
		XMMatrixLookToRH(cameraPosition, cameraDirection, cameraUp);
	XMStoreFloat4x4(&g_ViewMatrix, viewMatrix);

	XMMATRIX projectionMatrix = XMMatrixPerspectiveFovRH(
		XMConvertToRadians(g_CameraFOVDeg),
		(float)g_Width / (float)g_Height,
		g_CameraNearPlane,
		g_CameraFarPlane
	);
	XMStoreFloat4x4(&g_ProjectionMatrix, projectionMatrix);
}

void DoArcballUpdate() {
	if (!g_MouseDragging) {
		return;
	}

	// Check if the delta would be zero
	if (g_LastMousePos.x == g_MousePos.x && g_LastMousePos.y == g_MousePos.y) {
		return;
	}

	float dx = g_MousePos.x - g_LastMousePos.x;
	float dy = g_MousePos.y - g_LastMousePos.y;

	float theta = dy * 0.01f;
	float phi = dx * 0.01f;
	float radius = g_CameraPivotRadius;

	float x = radius * sinf(phi) * cosf(theta);
	float y = radius * cosf(phi);
	float z = radius * sinf(phi) * sinf(theta);

	XMVECTOR pivotTranslation = XMVectorSet(x, y, z, 0.0f);
	XMVECTOR dirTowardsPivot =
		XMVectorNegate(XMVector4Normalize(pivotTranslation));

	// Set the camera position to the pivot point
	XMStoreFloat3(&g_CameraPosition, pivotTranslation);
	XMStoreFloat3(&g_CameraDirection, dirTowardsPivot);
}

void EnsureWindowInitialized() {
	if (!g_sdlInitialized) {
		SDL_Init(SDL_INIT_VIDEO);
		g_sdlInitialized = true;
	}

	if (g_Window == nullptr) {
		g_Window = SDL_CreateWindow(
			"Gelly Fluid Sim Test",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			g_Width,
			g_Height,
			SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
		);
	}
}

void EnsureD3D11() {
	if (g_Device != nullptr) {
		return;
	}

	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	SDL_GetWindowWMInfo(g_Window, &info);

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = g_Width;
	swapChainDesc.BufferDesc.Height = g_Height;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = info.info.win.window;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	D3D11_CREATE_DEVICE_FLAG deviceFlags = D3D11_CREATE_DEVICE_DEBUG;

	DX("Failed to create the D3D11 device or swapchain",
	   D3D11CreateDeviceAndSwapChain(
		   nullptr,
		   D3D_DRIVER_TYPE_HARDWARE,
		   nullptr,
		   deviceFlags,
		   &featureLevel,
		   1,
		   D3D11_SDK_VERSION,
		   &swapChainDesc,
		   &g_SwapChain,
		   &g_Device,
		   nullptr,
		   &g_Context
	   ));

	ID3D11Texture2D *backBuffer;
	DX("Failed to get the back buffer",
	   g_SwapChain->GetBuffer(
		   0, __uuidof(ID3D11Texture2D), (void **)&backBuffer
	   ));

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(rtvDesc));

	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	DX("Failed to create the render target view",
	   g_Device->CreateRenderTargetView(backBuffer, &rtvDesc, &g_BackBufferRTV)
	);

	DX("Failed to get the debug message queue",
	   g_Device->QueryInterface(
		   __uuidof(ID3D11InfoQueue), (void **)&g_DebugMsgQueue
	   ));

	EnsureDebugShadersLoaded();
	EnsureParticleLayoutInitialized();
	EnsureDebugCBInitialized();
	EnsureDepthResources();
	EnsureRasterizerInitialized();
}

void OutputD3DMessages() {
	if (g_DebugMsgQueue == nullptr) {
		return;
	}

	UINT64 messageCount = g_DebugMsgQueue->GetNumStoredMessages();
	for (UINT64 i = 0; i < messageCount; i++) {
		SIZE_T messageLength = 0;
		DX("Failed to get the message length",
		   g_DebugMsgQueue->GetMessage(i, nullptr, &messageLength));

		auto *message = (D3D11_MESSAGE *)malloc(messageLength);
		DX("Failed to get the message",
		   g_DebugMsgQueue->GetMessage(i, message, &messageLength));

		OutputDebugStringA(message->pDescription);
		OutputDebugStringA("\n");

		free(message);
	}
}

void EnsureSolverInitialized() {
	if (g_Device == nullptr) {
		return;
	}

	g_SolverContext = new SolverContext(g_Device, g_Context);
	PBFSolverSettings solverSettings{};
	solverSettings.radius = 0.1f;
	solverSettings.maxParticles = g_MaxParticles;

	g_Solver = new PBFSolver(g_SolverContext, solverSettings);
	g_ParticlePositions = g_Solver->GetGPUPositions();
}

void EnsureIMGUI() {
	g_FrameTimeSamples.reserve(g_FrameTimeSampleCount);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui_ImplSDL2_InitForD3D(g_Window);
	ImGui_ImplDX11_Init(g_Device, g_Context);

	ImGui::StyleColorsDark();
}

void EnsureInitialized() {
	EnsureWindowInitialized();
	EnsureD3D11();
	EnsureSolverInitialized();
	EnsureIMGUI();
}

void Shutdown() {
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	if (g_BackBufferRTV) {
		g_BackBufferRTV->Release();
		g_BackBufferRTV = nullptr;
	}

	if (g_SwapChain) {
		g_SwapChain->Release();
		g_SwapChain = nullptr;
	}

	if (g_Context) {
		g_Context->Release();
		g_Context = nullptr;
	}

	if (g_Device) {
		g_Device->Release();
		g_Device = nullptr;
	}

	if (g_Window) {
		SDL_DestroyWindow(g_Window);
		g_Window = nullptr;
	}

	if (g_Solver) {
		delete g_Solver;
		g_Solver = nullptr;
	}

	if (g_SolverContext) {
		delete g_SolverContext;
		g_SolverContext = nullptr;
	}
}

void RenderSimControls() {
	ImGui::Begin("Simulation Controls");
	float fps = ImGui::GetIO().Framerate;
	ImGui::Text("FPS: %.1f", fps);
	float frametimeMs = 1000.0f / fps;
	g_FrameTimeSamples.push_back(frametimeMs);
	if (g_FrameTimeSamples.size() > g_FrameTimeSampleCount) {
		g_FrameTimeSamples.erase(g_FrameTimeSamples.begin());
	}

	ImGui::Text("Frame time: %.2f ms", frametimeMs);
	ImGui::PlotLines(
		"Frame time graph",
		g_FrameTimeSamples.data(),
		(int)g_FrameTimeSamples.size(),
		0,
		nullptr,
		0.0f,
		50.0f,
		ImVec2(0, 80)
	);

	ImGui::Text("Dragging mouse: %s", g_MouseDragging ? "true" : "false");
	ImGui::Text("Mouse pos: (%.2f, %.2f)", g_MousePos.x, g_MousePos.y);

	ImGui::Text(
		"Camera position: (%.2f, %.2f, %.2f)",
		g_CameraPosition.x,
		g_CameraPosition.y,
		g_CameraPosition.z
	);
	ImGui::Text(
		"Camera direction: (%.2f, %.2f, %.2f)",
		g_CameraDirection.x,
		g_CameraDirection.y,
		g_CameraDirection.z
	);

	ImGui::Text("Particle radius: %.2f", g_ParticleRadius);
	ImGui::SliderFloat("Particle radius", &g_ParticleRadius, 0.0f, 1.0f);
	ImGui::End();
}

void RenderParticles() {
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(viewport));
	viewport.Width = (float)g_Width;
	viewport.Height = (float)g_Height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	g_Context->RSSetState(g_RasterizerState);
	g_Context->RSSetViewports(1, &viewport);
	g_Context->OMSetRenderTargets(1, &g_BackBufferRTV, g_DSV);
	g_Context->OMSetDepthStencilState(g_DSS, 0);
	// Clear DSV
	g_Context->ClearDepthStencilView(
		g_DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0
	);

	g_Context->IASetInputLayout(g_ParticlePosLayout);
	g_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	unsigned int stride = sizeof(XMFLOAT4);
	unsigned int offset = 0;
	// recap: It dont work
	g_Context->IASetVertexBuffers(0, 1, &g_ParticlePositions, &stride, &offset);

	g_Context->VSSetShader(g_DebugRenderVS.Get(), nullptr, 0);
	g_Context->GSSetShader(g_DebugRenderGS.Get(), nullptr, 0);
	g_Context->PSSetShader(g_DebugRenderPS.Get(), nullptr, 0);
	g_DebugCB.BindToShaders(g_Context, 0);

	g_Context->Draw(g_MaxParticles, 0);
}

void RenderFrame() {
	UpdateCameraMatrices();
	UpdateDebugCB();
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplSDL2_NewFrame(g_Window);
	ImGui::NewFrame();

	RenderSimControls();

	ImGui::Render();
	float clearColor[4] = {1.0f, 0.0f, 0.0f, 1.0f};
	g_Context->OMSetRenderTargets(1, &g_BackBufferRTV, g_DSV);
	g_Context->ClearRenderTargetView(g_BackBufferRTV, clearColor);
	g_Context->OMSetDepthStencilState(g_DSS, 0);

	RenderParticles();

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	g_SwapChain->Present(1, 0);
}

int main() {
	EnsureInitialized();

	bool quit = false;
	while (!quit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT) {
				quit = true;
			}

			// Drop if ImGUI is processing the mouse
			if (ImGui::GetIO().WantCaptureMouse) {
				continue;
			}

			if (event.type == SDL_MOUSEBUTTONDOWN) {
				if (event.button.button == SDL_BUTTON_LEFT) {
					g_MouseDragging = true;
					g_LastMousePos = g_MousePos;
				}
			} else if (event.type == SDL_MOUSEBUTTONUP) {
				if (event.button.button == SDL_BUTTON_LEFT) {
					g_MouseDragging = false;
				}
			}

			if (event.type == SDL_MOUSEMOTION) {
				g_MousePos = {
					static_cast<float>(event.motion.x),
					static_cast<float>(event.motion.y)};
			}

			// Change pivot radius based on mouse wheel

			if (event.type == SDL_MOUSEWHEEL) {
				g_CameraPivotRadius += (float)event.wheel.y * 0.1f;
			}
		}

		OutputD3DMessages();
		DoArcballUpdate();
		RenderFrame();
	}

	Shutdown();

	return 0;
}
