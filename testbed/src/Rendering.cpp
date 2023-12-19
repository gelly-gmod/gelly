#include "Rendering.h"

#include <GellyD3D.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_sdl2.h>
#include <d3d11.h>
#include <imgui.h>

#include <tracy/Tracy.hpp>
#include <unordered_map>

#include "Gelly.h"
#include "Logging.h"
#include "Scene.h"
#include "Shaders.h"
#include "Textures.h"
#include "Window.h"
#include "ui/TestbedWindow.h"

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

static ID3D11Texture2D *backbuffer = nullptr;
static ID3D11RenderTargetView *backbufferRTV = nullptr;

static ID3D11DepthStencilView *depthStencilView = nullptr;
static ID3D11Texture2D *depthStencilBuffer = nullptr;
static ID3D11DepthStencilState *depthStencilState = nullptr;

static ID3D11RasterizerState *rasterizerState = nullptr;

static ID3D11PixelShader *genericWorldLitPS = nullptr;
static ID3D11VertexShader *genericWorldLitVS = nullptr;
static ID3D11InputLayout *genericWorldLitInputLayout = nullptr;

static d3d11::ConstantBuffer<GenericRenderCBuffer> worldRenderConstants;
static std::unordered_map<MeshReference, D3D11WorldMesh> worldMeshes;

void CreateRasterizerState() {
	if (rasterizerState) {
		rasterizerState->Release();
	}

	const bool wireframe =
		UI_DATA(TestbedWindow, rasterizerFlags) & RASTERIZER_FLAG_WIREFRAME;
	const bool cull =
		!(UI_DATA(TestbedWindow, rasterizerFlags) & RASTERIZER_FLAG_NOCULL);

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
static MeshReference meshReferenceCounter = 0;

void UpdateUI() {
	RENDER_WINDOW(TestbedWindow);
	if (UI_DATA(TestbedWindow, rasterizerFlags) ^
		UI_DATA(TestbedWindow, lastRasterizerFlags)) {
		CreateRasterizerState();
	}
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

	// for the inverse, we have no model matrix, so we can just do
	// invMvp = inv(v * p)

	XMMATRIX invMvpMatrix = XMMatrixMultiply(viewMatrix, projMatrix);
	invMvpMatrix = XMMatrixInverse(nullptr, invMvpMatrix);

	XMStoreFloat4x4(invMvp, invMvpMatrix);
}

void GenerateGellyCameraMatrices(
	const Camera &camera,
	XMFLOAT4X4 *view,
	XMFLOAT4X4 *proj,
	XMFLOAT4X4 *invView,
	XMFLOAT4X4 *invProj
) {
	XMStoreFloat4x4(
		proj,
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

	XMStoreFloat4x4(view, XMMatrixLookToRH(pos, dir, up));

	// Invert
	const XMMATRIX viewMatrix = XMLoadFloat4x4(view);
	const XMMATRIX projMatrix = XMLoadFloat4x4(proj);

	XMStoreFloat4x4(invView, XMMatrixInverse(nullptr, viewMatrix));
	XMStoreFloat4x4(invProj, XMMatrixInverse(nullptr, projMatrix));
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

static ID3D11RenderTargetView *cachedGBufferRTVs[4] = {
	nullptr, nullptr, nullptr, nullptr
};

void CreateGBufferTextures() {
	FeatureTextureInfo albedoTexInfo{};
	albedoTexInfo.width = WINDOW_WIDTH;
	albedoTexInfo.height = WINDOW_HEIGHT;
	albedoTexInfo.format = DXGI_FORMAT_R8G8B8A8_UNORM;

	CreateFeatureTexture(GBUFFER_ALBEDO_TEXNAME, albedoTexInfo);

	FeatureTextureInfo normalTexInfo{};
	normalTexInfo.width = WINDOW_WIDTH;
	normalTexInfo.height = WINDOW_HEIGHT;
	normalTexInfo.format = DXGI_FORMAT_R8G8B8A8_SNORM;

	CreateFeatureTexture(GBUFFER_NORMAL_TEXNAME, normalTexInfo);

	FeatureTextureInfo depthTexInfo{};
	depthTexInfo.width = WINDOW_WIDTH;
	depthTexInfo.height = WINDOW_HEIGHT;
	depthTexInfo.format = DXGI_FORMAT_R32_FLOAT;

	CreateFeatureTexture(GBUFFER_DEPTH_TEXNAME, depthTexInfo);

	FeatureTextureInfo positionTexInfo{};
	positionTexInfo.width = WINDOW_WIDTH;
	positionTexInfo.height = WINDOW_HEIGHT;
	positionTexInfo.format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	CreateFeatureTexture(GBUFFER_POSITION_TEXNAME, positionTexInfo);

	cachedGBufferRTVs[0] = GetTextureRTV(GBUFFER_ALBEDO_TEXNAME);
	cachedGBufferRTVs[1] = GetTextureRTV(GBUFFER_NORMAL_TEXNAME);
	cachedGBufferRTVs[2] = GetTextureRTV(GBUFFER_DEPTH_TEXNAME);
	cachedGBufferRTVs[3] = GetTextureRTV(GBUFFER_POSITION_TEXNAME);
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
	swapchainDesc.BufferUsage =
		DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
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
	io.Fonts->AddFontFromFileTTF("assets/RobotoCondensed-Regular.ttf", 16.0f);
	ImGui::StyleColorsDark();
	ImGui::GetStyle().WindowRounding = 3.f;
	ImGui::GetStyle().FrameRounding = 3.f;

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
	depthStencilStateDesc.StencilEnable = false;

	ERROR_IF_FAILED(
		"Failed to create depth stencil state",
		device->CreateDepthStencilState(
			&depthStencilStateDesc, &depthStencilState
		)
	);

	logger->Info("Creating rasterizer state");

	CreateRasterizerState();

	logger->Info("Renderer initialized");

	InitializeTextureSystem(logger, device);

	logger->Info("Creating GBuffer textures");
	CreateGBufferTextures();

	return device;
}

ID3D11DeviceContext *testbed::GetRendererContext(ID3D11Device *device) {
	if (device != ::device) {
		logger->Error("Device does not match renderer device");
		return nullptr;
	}

	return deviceContext;
}

ID3D11Texture2D *testbed::GetBackBuffer(ID3D11Device *device) {
	if (device != ::device) {
		logger->Error("Device does not match renderer device");
		return nullptr;
	}

	return backbuffer;
}

ID3D11RenderTargetView *testbed::GetBackBufferRTV(ID3D11Device *device) {
	if (device != ::device) {
		logger->Error("Device does not match renderer device");
		return nullptr;
	}

	return backbufferRTV;
}

ID3D11DepthStencilView *testbed::GetDepthBufferDSV(ID3D11Device *device) {
	if (device != ::device) {
		logger->Error("Device does not match renderer device");
		return nullptr;
	}

	return depthStencilView;
}

ID3D11DepthStencilState *testbed::GetDepthBufferState(ID3D11Device *device) {
	if (device != ::device) {
		logger->Error("Device does not match renderer device");
		return nullptr;
	}

	return depthStencilState;
}

GenericRenderCBuffer testbed::CreateGenericRenderCBuffer(const Camera &camera) {
	GenericRenderCBuffer cbuffer = {};
	XMStoreFloat4(&cbuffer.eyePos, XMLoadFloat3(&camera.position));
	cbuffer.windowSize = {
		static_cast<float>(WINDOW_WIDTH),
		static_cast<float>(WINDOW_HEIGHT),
		0.0f,
		0.0f
	};

	XMFLOAT4X4 identityModelMatrix = {};
	XMStoreFloat4x4(&identityModelMatrix, XMMatrixIdentity());

	GenerateCameraMatrices(
		camera, &identityModelMatrix, &cbuffer.mvp, &cbuffer.invMvp
	);

	return cbuffer;
}

void testbed::StartFrame() {
	ZoneScoped;
	{
		ZoneScopedN("ImGUI new frame");
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplSDL2_NewFrame(GetTestbedWindow());
		ImGui::NewFrame();
	}

	{
		ZoneScopedN("ImGUI UI creation");
		UpdateUI();
	}

	{
		ZoneScopedN("Clearing buffers");
		constexpr float color[4] = {0.3f, 0.3f, 1.0f, 1.0f};
		deviceContext->ClearRenderTargetView(backbufferRTV, color);
		// Clear depth stencil
		deviceContext->ClearDepthStencilView(
			depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0
		);
		deviceContext->OMSetRenderTargets(1, &backbufferRTV, nullptr);

		constexpr float nullFeatureColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
		for (auto &cachedGBufferRTV : cachedGBufferRTVs) {
			deviceContext->ClearRenderTargetView(
				cachedGBufferRTV, nullFeatureColor
			);
		}
	}
}

void testbed::EndFrame() {
	ZoneScoped;
	deviceContext->OMSetRenderTargets(1, &backbufferRTV, nullptr);

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Just clear to red
	if (const auto result = swapchain->Present(0, 0); FAILED(result)) {
		logger->Warning("Failed to present. Error code: %d", result);
		logger->Warning("Reviewing to determine if device is lost");

		if (const auto reasonCode = device->GetDeviceRemovedReason();
			reasonCode != S_OK) {
			logger->Warning("Device lost. Reason code: %08X", reasonCode);
			logger->Warning("Attempting to recover");
			InitializeRenderer(logger);
		}
	}
}

MeshReference testbed::CreateWorldMesh(const WorldMesh &mesh) {
	D3D11WorldMesh d3d11Mesh{};

	auto *vertexData = reinterpret_cast<const float3 *>(mesh.vertices.data());
	auto *normalData = reinterpret_cast<const float3 *>(mesh.normals.data());
	auto *indicesData =
		reinterpret_cast<const unsigned short *>(mesh.indices.data());
	d3d11Mesh.vertices = new d3d11::VertexBuffer<float3>(
		device,
		vertexData,
		mesh.vertices.size() / sizeof(float3),
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);

	d3d11Mesh.normals = new d3d11::VertexBuffer<float3>(
		device,
		normalData,
		mesh.normals.size() / sizeof(float3),
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);

	// Have to manually make an index buffer right now
	D3D11_BUFFER_DESC indexBufferDesc{};
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = mesh.indices.size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA indexData{};
	indexData.pSysMem = indicesData;
	indexData.SysMemPitch = 0;

	ERROR_IF_FAILED(
		"Failed to create index buffer",
		device->CreateBuffer(&indexBufferDesc, &indexData, &d3d11Mesh.indices)
	);

	d3d11Mesh.indexCount = mesh.indices.size() / sizeof(unsigned short);

	const auto reference = meshReferenceCounter++;
	worldMeshes[reference] = d3d11Mesh;

	return reference;
}

void testbed::DestroyWorldMesh(const MeshReference &mesh) {
	if (worldMeshes.find(mesh) == worldMeshes.end()) {
		logger->Error("Attempted to destroy non-existent mesh");
		return;
	}

	const auto &[vertices, normals, indices, indexCount] = worldMeshes[mesh];
	delete vertices;
	delete normals;
	indices->Release();
	worldMeshes.erase(mesh);
}

void testbed::RenderWorldList(
	const WorldRenderList &list, const Camera &camera
) {
	ZoneScopedN("World render");
	// Set up cbuffer
	GenericRenderCBuffer cbuffer = {};
	XMStoreFloat4(&cbuffer.eyePos, XMLoadFloat3(&camera.position));
	cbuffer.windowSize = {
		static_cast<float>(WINDOW_WIDTH),
		static_cast<float>(WINDOW_HEIGHT),
		0.0f,
		0.0f
	};

	worldRenderConstants.Set(deviceContext, &cbuffer);

	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(WINDOW_WIDTH);
	viewport.Height = static_cast<float>(WINDOW_HEIGHT);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	deviceContext->RSSetViewports(1, &viewport);
	deviceContext->RSSetState(rasterizerState);

	// Set up shaders
	deviceContext->VSSetShader(genericWorldLitVS, nullptr, 0);
	deviceContext->PSSetShader(genericWorldLitPS, nullptr, 0);
	deviceContext->IASetInputLayout(genericWorldLitInputLayout);

	// Set up depth stencil
	deviceContext->OMSetDepthStencilState(depthStencilState, 0);
	deviceContext->OMSetRenderTargets(4, cachedGBufferRTVs, depthStencilView);

	// Render each object
	for (auto i = list.cbegin(); i != list.cend(); ++i) {
		ZoneScopedN("Object render");
		const auto &object = *i;
		const auto &mesh = worldMeshes[object.mesh];

		// Set up vertex buffers
		{
			ZoneScopedN("Vertex buffer setup");
			mesh.vertices->SetAtSlot(
				deviceContext, 0, genericWorldLitInputLayout
			);
			mesh.normals->SetAtSlot(
				deviceContext, 1, genericWorldLitInputLayout
			);
			deviceContext->IASetIndexBuffer(
				mesh.indices, DXGI_FORMAT_R16_UINT, 0
			);
		}

		// Set up cbuffer
		{
			ZoneScopedN("C-buffer setup");
			GenerateCameraMatrices(
				camera, &object.transform, &cbuffer.mvp, &cbuffer.invMvp
			);
		}

		{
			ZoneScopedN("Unique C-buffer update");
			worldRenderConstants.Set(deviceContext, &cbuffer);
			worldRenderConstants.BindToShaders(deviceContext, 0);
		}

		// Draw
		{
			ZoneScopedN("Draw");
			deviceContext->DrawIndexed(mesh.indexCount, 0, 0);
		}
	}

	{
		ZoneScopedN("Gelly render");
		Gelly::FluidRenderParams params{};
		params.thresholdRatio = UI_DATA(TestbedWindow, thresholdRatio);
		{
			ZoneScopedN("Matrix gen");
			GenerateGellyCameraMatrices(
				camera,
				&params.view,
				&params.proj,
				&params.invView,
				&params.invProj
			);
		}

		{
			ZoneScopedN("Fluid render");
			auto *fluidRenderer = GetGellyFluidRenderer();
			fluidRenderer->SetPerFrameParams(params);
			fluidRenderer->Render();

#ifdef _DEBUG
			GetGellyRenderContext()->PrintDebugInfo();
#endif
		}
	}
}