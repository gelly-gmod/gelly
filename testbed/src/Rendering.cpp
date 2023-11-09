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
	}

using namespace testbed;

/**
 * The type of mesh is explicit since the input layout has to match the world
 * shader.
 */
struct D3D11WorldMesh {
	// We *could* at some point in the future
	// have this struct actually own the class,
	// but there is no default constructor for
	// VertexBuffer, so we'd have to use a pointer to a heap-allocated instance.
	d3d11::VertexBuffer<float3> *vertices;
	d3d11::VertexBuffer<float3> *normals;

	ID3D11Buffer *indices;
	int indexCount;
};

static ID3D11Device *device = nullptr;
static IDXGISwapChain *swapchain = nullptr;
static ID3D11DeviceContext *deviceContext = nullptr;
static ID3D11RenderTargetView *backbufferRTV = nullptr;

static ID3D11DepthStencilView *depthStencilView = nullptr;
static ID3D11Texture2D *depthStencilBuffer = nullptr;
static ID3D11DepthStencilState *depthStencilState = nullptr;

static ID3D11RasterizerState *rasterizerState = nullptr;

static ID3D11PixelShader *genericWorldLitPS = nullptr;
static ID3D11VertexShader *genericWorldLitVS = nullptr;
static ID3D11InputLayout *genericWorldLitInputLayout = nullptr;

static d3d11::ConstantBuffer<WorldRenderCBuffer> worldRenderConstants;
static std::unordered_map<MeshReference, D3D11WorldMesh> worldMeshes;

// Mesh reference counter is used to generate unique mesh references.
// It's really just a monotonically increasing integer, and we're definitely
// not going to run out of them.
static unsigned int meshReferenceCounter = 0;

void GenerateCameraMatrices(
	const Camera &camera, XMFLOAT4X4 *view, XMFLOAT4X4 *proj
) {
	XMStoreFloat4x4(
		proj,
		XMMatrixPerspectiveFovLH(
			XMConvertToRadians(camera.fov),
			camera.aspectRatio,
			camera.nearPlane,
			camera.farPlane
		)
	);

	XMVECTOR dir = XMLoadFloat3(&camera.dir);
	XMVECTOR pos = XMLoadFloat3(&camera.position);
	XMVECTOR up = XMLoadFloat3(&UP_VECTOR);

	XMStoreFloat4x4(view, XMMatrixLookToLH(pos, dir, up));

	// Transpose matrices because HLSL is column-major
	XMStoreFloat4x4(view, (XMLoadFloat4x4(view)));
	XMStoreFloat4x4(proj, (XMLoadFloat4x4(proj)));
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

	AddEventInterceptor(ImGuiSDLEventInterceptor);

	GetLogger()->Info("ImGUI initialized");

	GetLogger()->Info("Loading generic world lit shaders");
	LoadGenericWorldLit();

	GetLogger()->Info("Creating depth stencil buffer");
	D3D11_TEXTURE2D_DESC depthStencilDesc = {};
	depthStencilDesc.Width = WINDOW_WIDTH;
	depthStencilDesc.Height = WINDOW_HEIGHT;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	ERROR_IF_FAILED(
		"Failed to create depth stencil buffer",
		device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilBuffer)
	);

	ERROR_IF_FAILED(
		"Failed to create depth stencil view",
		device->CreateDepthStencilView(
			depthStencilBuffer, nullptr, &depthStencilView
		)
	);

	GetLogger()->Info("Creating depth stencil state");
	D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc = {};
	depthStencilStateDesc.DepthEnable = true;
	depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS;

	ERROR_IF_FAILED(
		"Failed to create depth stencil state",
		device->CreateDepthStencilState(
			&depthStencilStateDesc, &depthStencilState
		)
	);

	GetLogger()->Info("Creating rasterizer state");

	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;

	ERROR_IF_FAILED(
		"Failed to create rasterizer state",
		device->CreateRasterizerState(&rasterizerDesc, &rasterizerState)
	);

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

	float color[4] = {1.0f, 0.0f, 0.0f, 1.0f};
	deviceContext->ClearRenderTargetView(backbufferRTV, color);
	// Clear depth stencil
	deviceContext->ClearDepthStencilView(
		depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0
	);
	deviceContext->OMSetRenderTargets(1, &backbufferRTV, nullptr);
}

void testbed::EndFrame() {
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Just clear to red
	ERROR_IF_FAILED("Failed to present swapchain", swapchain->Present(0, 0));
}

MeshReference testbed::CreateWorldMesh(const WorldMesh &mesh) {
	D3D11WorldMesh d3d11Mesh = {};
	d3d11Mesh.vertices = new d3d11::VertexBuffer<float3>(
		device,
		mesh.vertices,
		mesh.vertexCount,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);

	d3d11Mesh.normals = new d3d11::VertexBuffer<float3>(
		device,
		mesh.normals,
		mesh.normalCount,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);

	// Have to manually make an index buffer right now
	D3D11_BUFFER_DESC indexBufferDesc = {};
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * mesh.indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = mesh.indices;
	indexData.SysMemPitch = 0;

	ERROR_IF_FAILED(
		"Failed to create index buffer",
		device->CreateBuffer(&indexBufferDesc, &indexData, &d3d11Mesh.indices)
	);

	d3d11Mesh.indexCount = mesh.indexCount;

	MeshReference reference = meshReferenceCounter++;
	worldMeshes[reference] = d3d11Mesh;

	return reference;
}

void testbed::DestroyWorldMesh(MeshReference mesh) {
	if (worldMeshes.find(mesh) == worldMeshes.end()) {
		GetLogger()->Error("Attempted to destroy non-existent mesh");
		return;
	}

	D3D11WorldMesh d3d11Mesh = worldMeshes[mesh];
	delete d3d11Mesh.vertices;
	delete d3d11Mesh.normals;
	d3d11Mesh.indices->Release();
	worldMeshes.erase(mesh);
}

void testbed::RenderWorldList(
	const testbed::WorldRenderList &list, const testbed::Camera &camera
) {
	// Set up cbuffer
	WorldRenderCBuffer cbuffer = {};
	GenerateCameraMatrices(camera, &cbuffer.view, &cbuffer.projection);

	worldRenderConstants.Set(deviceContext, &cbuffer);

	D3D11_VIEWPORT viewport = {};
	viewport.Width = (float)WINDOW_WIDTH;
	viewport.Height = (float)WINDOW_HEIGHT;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	deviceContext->RSSetViewports(1, &viewport);
	deviceContext->RSSetState(rasterizerState);

	// Set up shaders
	deviceContext->VSSetShader(genericWorldLitVS, nullptr, 0);
	deviceContext->PSSetShader(genericWorldLitPS, nullptr, 0);
	deviceContext->IASetInputLayout(genericWorldLitInputLayout);

	// Set up depth stencil
	deviceContext->OMSetDepthStencilState(depthStencilState, 1);
	deviceContext->OMSetRenderTargets(1, &backbufferRTV, depthStencilView);

	// Render each object
	for (unsigned int i = 0; i < list.objectCount; i++) {
		WorldRenderObject object = list.objects[i];
		D3D11WorldMesh mesh = worldMeshes[object.mesh];

		// Set up vertex buffers
		mesh.vertices->SetAtSlot(deviceContext, 0, genericWorldLitInputLayout);
		mesh.normals->SetAtSlot(deviceContext, 1, genericWorldLitInputLayout);
		deviceContext->IASetIndexBuffer(mesh.indices, DXGI_FORMAT_R16_UINT, 0);

		// Set up cbuffer
		XMStoreFloat4x4(&cbuffer.model, XMLoadFloat4x4(&object.transform));
		worldRenderConstants.Set(deviceContext, &cbuffer);
		worldRenderConstants.BindToShaders(deviceContext, 0);
		// Draw
		deviceContext->DrawIndexed(mesh.indexCount, 0, 0);
	}
}