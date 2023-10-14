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

XMFLOAT3 g_CameraPosition = {0.0f, 0.0f, -5.0f};
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

SolverContext *g_SolverContext = nullptr;
PBFSolver *g_Solver = nullptr;
const int g_MaxParticles = 10000;
float g_ParticleRadius = 0.1f;

ID3D11Buffer *g_ParticlePositions = nullptr;
ID3D11InputLayout *g_ParticlePosLayout = nullptr;
ComPtr<ID3D11VertexShader> g_DebugRenderVS;
ComPtr<ID3DBlob> g_DebugRenderVSBytecode;
ComPtr<ID3D11GeometryShader> g_DebugRenderGS;
ComPtr<ID3D11PixelShader> g_DebugRenderPS;
struct DebugCB {
	XMFLOAT4X4 view;
	XMFLOAT4X4 projection;
	float radius;
	float pad1;
	float pad2;
	float pad3;
};
DebugCB g_DebugCBData{};
d3d11::ConstantBuffer<DebugCB> g_DebugCB;

const char *g_DebugRenderVSCode =
	#include "shaders/d3d11/fluidsim/DebugRender.vs.embed.hlsl";
const char *g_DebugRenderGSCode = nullptr;
const char *g_DebugRenderPSCode = nullptr;

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
		{"POSITION",
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

	XMFLOAT3 startPointOnBall = {
		(g_LastMousePos.x / (float)g_Width) * 2.0f - 1.0f,
		(1.f - g_LastMousePos.y / (float)g_Height) * 2.0f - 1.0f,
		0.0f};

	XMFLOAT3 endPointOnBall = {
		(g_MousePos.x / (float)g_Width) * 2.0f - 1.0f,
		(1.f - g_MousePos.y / (float)g_Height) * 2.0f - 1.0f,
		0.0f};

	// Calculate Z using pythagorean theorem
	float startPointOnBallLength =
		XMVectorGetX(XMVector3Length(XMLoadFloat3(&startPointOnBall)));

	startPointOnBall.z =
		sqrtf(1.0f - startPointOnBallLength * startPointOnBallLength);

	float endPointOnBallLength =
		XMVectorGetX(XMVector3Length(XMLoadFloat3(&endPointOnBall)));

	endPointOnBall.z =
		sqrtf(1.0f - endPointOnBallLength * endPointOnBallLength);

	XMVECTOR startPointOnBallVector = XMLoadFloat3(&startPointOnBall);
	XMVECTOR endPointOnBallVector = XMLoadFloat3(&endPointOnBall);

	XMVECTOR rotationAxis =
		XMVector3Cross(startPointOnBallVector, endPointOnBallVector);

	float rotationAngle = XMVectorGetX(XMVector3AngleBetweenVectors(
		startPointOnBallVector, endPointOnBallVector
	));

	XMMATRIX rotationMatrix = XMMatrixRotationAxis(rotationAxis, rotationAngle);

	XMVECTOR cameraPosition = XMLoadFloat3(&g_CameraPosition);
	XMVECTOR cameraDirection = XMLoadFloat3(&g_CameraDirection);

	XMVECTOR rotatedCameraDirection =
		XMVector3TransformNormal(cameraDirection, rotationMatrix);

	XMVECTOR rotatedCameraPosition =
		XMVector3Transform(cameraPosition, rotationMatrix);

	XMStoreFloat3(&g_CameraDirection, rotatedCameraDirection);
	XMStoreFloat3(&g_CameraPosition, rotatedCameraPosition);
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

	DX("Failed to create the D3D11 device or swapchain",
	   D3D11CreateDeviceAndSwapChain(
		   nullptr,
		   D3D_DRIVER_TYPE_HARDWARE,
		   nullptr,
		   0,
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

	EnsureDebugShadersLoaded();
	EnsureParticleLayoutInitialized();
	EnsureDebugCBInitialized();
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

	ImGui::End();
}

void RenderParticles() {
	g_Context->IASetInputLayout(g_ParticlePosLayout);
	g_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	g_Context->IASetVertexBuffers(0, 1, &g_ParticlePositions, nullptr, nullptr);

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
	float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
	g_Context->OMSetRenderTargets(1, &g_BackBufferRTV, nullptr);
	g_Context->ClearRenderTargetView(g_BackBufferRTV, clearColor);

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

			if (event.type == SDL_MOUSEBUTTONDOWN) {
				if (event.button.button == SDL_BUTTON_LEFT) {
					g_MouseDragging = true;
				}
			} else if (event.type == SDL_MOUSEBUTTONUP) {
				if (event.button.button == SDL_BUTTON_LEFT) {
					g_MouseDragging = false;
				}
			}

			if (event.type == SDL_MOUSEMOTION) {
				g_LastMousePos = g_MousePos;
				g_MousePos = {
					static_cast<float>(event.motion.x),
					static_cast<float>(event.motion.y)};
			}
		}

		DoArcballUpdate();
		RenderFrame();
	}

	Shutdown();

	return 0;
}
