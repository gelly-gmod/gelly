#include "Rendering.h"

#include <GellyD3D.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_sdl2.h>
#include <d3d11.h>
#include <imgui.h>

#include <unordered_map>

#include "Logging.h"
#include "Shaders.h"
#include "Window.h"

#define ERROR_IF_FAILED(msg, hr) \
	if (FAILED(hr)) {            \
		GetLogger()->Error(msg); \
		return;                  \
	}

using namespace testbed;

/**
 * The type of mesh is explicit since the input layout has to match the world
 * shader.
 */
struct D3D11WorldMesh {
	d3d11::VertexBuffer<float3> vertices;
	d3d11::VertexBuffer<float3> normals;
	ID3D11InputLayout *inputLayout = nullptr;
};

static ID3D11Device *device = nullptr;
static IDXGISwapChain *swapchain = nullptr;
static ID3D11DeviceContext *deviceContext = nullptr;
static ID3D11RenderTargetView *backbufferRTV = nullptr;

static ID3D11PixelShader *genericWorldLitPS = nullptr;
static ID3D11VertexShader *genericWorldLitVS = nullptr;
static ID3D11InputLayout *genericWorldLitInputLayout = nullptr;

static d3d11::ConstantBuffer<WorldRenderCBuffer> worldRenderConstants;
static std::unordered_map<MeshReference, D3D11WorldMesh> worldMeshes;

void GenerateCameraMatrices(
	const Camera &camera, XMFLOAT4X4 &view, XMFLOAT4X4 &proj
) {
	XMStoreFloat4x4(
		&proj,
		XMMatrixPerspectiveFovLH(
			camera.fov, camera.aspectRatio, camera.nearPlane, camera.farPlane
		)
	);

	XMVECTOR dir = XMLoadFloat3(&camera.dir);
	XMVECTOR pos = XMLoadFloat3(&camera.position);
	XMVECTOR up = XMLoadFloat3(&UP_VECTOR);

	XMStoreFloat4x4(&view, XMMatrixLookToLH(pos, dir, up));
}

void LoadGenericWorldLit() {
	genericWorldLitPS = GetPixelShaderFromFile(
		device, "shaders/GenericLitWorld.ps50.hlsl.dxbc"
	);
	genericWorldLitVS = GetVertexShaderFromFile(
		device, "shaders/GenericLitWorld.vs50.hlsl.dxbc"
	);

	D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[] = {
		{"POSITION",
		 0,
		 DXGI_FORMAT_R32G32B32_FLOAT,
		 0,
		 D3D11_APPEND_ALIGNED_ELEMENT,
		 D3D11_INPUT_PER_VERTEX_DATA,
		 0},
		{"NORMAL",
		 0,
		 DXGI_FORMAT_R32G32B32_FLOAT,
		 1,	 // per-vertex normals are supplied from a second vertex buffer
		 D3D11_APPEND_ALIGNED_ELEMENT,
		 D3D11_INPUT_PER_VERTEX_DATA,
		 0}};

	ShaderBuffer vsBuffer =
		LoadShaderBytecodeFromFile("shaders/GenericLitWorld.vs50.hlsl.dxbc");

	ERROR_IF_FAILED(
		"Failed to create input layout",
		device->CreateInputLayout(
			inputLayoutDesc,
			2,
			vsBuffer.buffer,
			vsBuffer.size,
			&genericWorldLitInputLayout
		)
	);

	FreeShaderBuffer(vsBuffer);
}

void ImGuiSDLEventInterceptor(SDL_Event *event) {
	ImGui_ImplSDL2_ProcessEvent(event);
}

void testbed::InitializeRenderer() {
	GetLogger()->Info("Initializing renderer");
	DXGI_SWAP_CHAIN_DESC swapchainDesc = {};
	swapchainDesc.BufferCount = 1;
	swapchainDesc.BufferDesc.Width = WINDOW_WIDTH;
	swapchainDesc.BufferDesc.Height = WINDOW_HEIGHT;
	swapchainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapchainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchainDesc.OutputWindow = GetTestbedWindowHandle();
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.SampleDesc.Quality = 0;
	swapchainDesc.Windowed = TRUE;

	ERROR_IF_FAILED(
		"Failed to create D3D11 device and swapchain",
		D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			0,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			&swapchainDesc,
			&swapchain,
			&device,
			nullptr,
			&deviceContext
		)
	);

	ID3D11Texture2D *backbuffer = nullptr;
	ERROR_IF_FAILED(
		"Failed to get backbuffer",
		swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&backbuffer)
	);

	ERROR_IF_FAILED(
		"Failed to create render target view",
		device->CreateRenderTargetView(backbuffer, nullptr, &backbufferRTV)
	);

	GetLogger()->Info("Creating world render constant buffer");
	worldRenderConstants.Init(device);

	GetLogger()->Info("Pre-allocating world mesh space");
	worldMeshes.reserve(100);

	GetLogger()->Info("Initializing ImGUI");
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForD3D(GetTestbedWindow());
	ImGui_ImplDX11_Init(device, deviceContext);

	SetEventInterceptor(ImGuiSDLEventInterceptor);

	GetLogger()->Info("ImGUI initialized");

	GetLogger()->Info("Loading generic world lit shaders");
	LoadGenericWorldLit();

	GetLogger()->Info("Renderer initialized");
}

void testbed::StartFrame() {
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplSDL2_NewFrame(GetTestbedWindow());
	ImGui::NewFrame();

	ImGui::Begin("Testbed");
	ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
	ImGui::Text("Gelly renderer backend: D3D11");
	ImGui::Text("Gelly simulation backend: FleX using D3D11");
	ImGui::End();
}

void testbed::EndFrame() {
	ImGui::Render();
	float color[4] = {1.0f, 0.0f, 0.0f, 1.0f};
	deviceContext->ClearRenderTargetView(backbufferRTV, color);
	deviceContext->OMSetRenderTargets(1, &backbufferRTV, nullptr);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Just clear to red
	ERROR_IF_FAILED("Failed to present swapchain", swapchain->Present(0, 0));
}