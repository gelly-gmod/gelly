#include "Rendering.h"

#include <GellyD3D.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_sdl2.h>
#include <d3d11.h>
#include <imgui.h>

#include <unordered_map>

#include "Logging.h"
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

	GetLogger()->Info("Renderer initialized");
}

void testbed::StartFrame() {
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplSDL2_NewFrame(GetTestbedWindow());
	ImGui::NewFrame();

	bool show_demo_window = true;
	ImGui::ShowDemoWindow(&show_demo_window);
}

void testbed::EndFrame() {
	ImGui::Render();
	float color[4] = {1.0f, 0.0f, 0.0f, 1.0f};
	deviceContext->ClearRenderTargetView(backbufferRTV, color);
	deviceContext->OMSetRenderTargets(1, &backbufferRTV, nullptr);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Just clear to red
	swapchain->Present(0, 0);
}