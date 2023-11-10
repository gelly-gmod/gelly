#include "Rendering.h"

#include <GellyD3D.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_sdl2.h>
#include <d3d11.h>
#include <imgui.h>

#include <unordered_map>

#include "Logging.h"
#include "Scene.h"
#include "Shaders.h"
#include "Window.h"

static testbed::ILogger *logger = nullptr;

#define ERROR_IF_FAILED(msg, hr) \
	if (FAILED(hr)) {            \
		logger->Error(msg);      \
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

static unsigned int rasterizerFlags = 0b00;
static unsigned int lastRasterizerFlags = 0b00;
static const unsigned int RASTERIZER_FLAG_WIREFRAME = 0b01;
static const unsigned int RASTERIZER_FLAG_NOCULL = 0b10;

void CreateRasterizerState() {
	if (rasterizerState) {
		rasterizerState->Release();
	}

	bool wireframe = rasterizerFlags & RASTERIZER_FLAG_WIREFRAME;
	bool cull = !(rasterizerFlags & RASTERIZER_FLAG_NOCULL);

	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode =
		wireframe ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = cull ? D3D11_CULL_FRONT : D3D11_CULL_NONE;

	ERROR_IF_FAILED(
		"Failed to create rasterizer state",
		device->CreateRasterizerState(&rasterizerDesc, &rasterizerState)
	);
}

// Mesh reference counter is used to generate unique mesh references.
// It's really just a monotonically increasing integer, and we're definitely
// not going to run out of them.
static unsigned int meshReferenceCounter = 0;

void CreateImGUIElements() {
	ImGui::Begin("Testbed");
	if (ImGui::CollapsingHeader("Frame Stats")) {
		ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
	}

	if (ImGui::CollapsingHeader("Gelly Integration")) {
		ImGui::Text("Gelly renderer backend: D3D11");
		ImGui::Text("Gelly simulation backend: FleX using D3D11");
	}

	if (ImGui::CollapsingHeader("Scene Info")) {
		auto sceneMetadata = GetCurrentSceneMetadata();
		ImGui::Text("Scene file: %s", sceneMetadata.filepath);
		ImGui::Text("Triangles: %d", sceneMetadata.triangles);
	}

	if (ImGui::CollapsingHeader("Rendering")) {
		if (ImGui::Button("Toggle wireframe")) {
			rasterizerFlags ^= RASTERIZER_FLAG_WIREFRAME;
		}

		if (ImGui::Button("Toggle no culling")) {
			rasterizerFlags ^= RASTERIZER_FLAG_NOCULL;
		}

		if (rasterizerFlags ^ lastRasterizerFlags) {
			CreateRasterizerState();
		}

		lastRasterizerFlags = rasterizerFlags;
	}

	ImGui::End();
}

void GenerateCameraMatrices(
	const Camera &camera,
	const XMFLOAT4X4 *model,
	XMFLOAT4X4 *mvp,
	XMFLOAT4X4 *invMvp
) {
	XMFLOAT4X4 view = {};
	XMFLOAT4X4 proj = {};
	XMStoreFloat4x4(
		&proj,
		XMMatrixPerspectiveFovRH(
			XMConvertToRadians(camera.fov),
			camera.aspectRatio,
			camera.nearPlane,
			camera.farPlane
		)
	);

	const XMVECTOR dir = XMLoadFloat3(&camera.dir);
	const XMVECTOR pos = XMLoadFloat3(&camera.position);
	const XMVECTOR up = XMLoadFloat3(&UP_VECTOR);

	XMStoreFloat4x4(&view, XMMatrixLookToRH(pos, dir, up));

	// Pre multiply
	const XMMATRIX viewMatrix = XMLoadFloat4x4(&view);
	const XMMATRIX projMatrix = XMLoadFloat4x4(&proj);

	// order:
	// mvp = m * v * p
	XMMATRIX mvpMatrix = XMMatrixMultiply(XMLoadFloat4x4(model), viewMatrix);
	mvpMatrix = XMMatrixMultiply(mvpMatrix, projMatrix);

	XMStoreFloat4x4(mvp, mvpMatrix);
	XMStoreFloat4x4(invMvp, XMMatrixInverse(nullptr, mvpMatrix));
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
		 0}
	};

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

ID3D11Device *testbed::InitializeRenderer(ILogger *newLogger) {
	logger = newLogger;

	logger->Info("Initializing renderer");
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

	UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	ERROR_IF_FAILED(
		"Failed to create D3D11 device and swapchain",
		D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			flags,
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

	logger->Info("Creating world render constant buffer");
	worldRenderConstants.Init(device);

	logger->Info("Pre-allocating world mesh space");
	worldMeshes.reserve(100);

	logger->Info("Initializing ImGUI");
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForD3D(GetTestbedWindow());
	ImGui_ImplDX11_Init(device, deviceContext);

	AddEventInterceptor(ImGuiSDLEventInterceptor);

	logger->Info("ImGUI initialized");

	logger->Info("Loading generic world lit shaders");
	LoadGenericWorldLit();

	logger->Info("Creating depth stencil buffer");
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

	logger->Info("Creating depth stencil state");
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

	logger->Info("Creating rasterizer state");

	CreateRasterizerState();

	logger->Info("Renderer initialized");

	return device;
}

void testbed::StartFrame() {
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplSDL2_NewFrame(GetTestbedWindow());
	ImGui::NewFrame();

	CreateImGUIElements();

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
	indexBufferDesc.ByteWidth = sizeof(unsigned short) * mesh.indexCount;
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
		logger->Error("Attempted to destroy non-existent mesh");
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
	XMStoreFloat4(&cbuffer.eyePos, XMLoadFloat3(&camera.position));
	cbuffer.windowSize = {
		static_cast<float>(WINDOW_WIDTH),
		static_cast<float>(WINDOW_HEIGHT),
		0.0f,
		0.0f
	};
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
		GenerateCameraMatrices(
			camera, &object.transform, &cbuffer.mvp, &cbuffer.invMvp
		);
		worldRenderConstants.Set(deviceContext, &cbuffer);
		worldRenderConstants.BindToShaders(deviceContext, 0);
		// Draw
		deviceContext->DrawIndexed(mesh.indexCount, 0, 0);
	}
}