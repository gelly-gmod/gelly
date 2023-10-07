#include <GellyRenderer.h>

#include "detail/ErrorHandling.h"
#include "rendering/techniques/ParticleRendering.h"

GellyRenderer::GellyRenderer(const RendererInitParams &params)
	: device(nullptr), deviceContext(nullptr), params(params), pipeline({}) {
	D3D_FEATURE_LEVEL featureLevel[1] = {D3D_FEATURE_LEVEL_11_0};

	UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	DX("Failed to create the D3D11 device.",
	   D3D11CreateDevice(
		   nullptr,
		   D3D_DRIVER_TYPE_HARDWARE,
		   nullptr,
		   flags,
		   featureLevel,
		   1,
		   D3D11_SDK_VERSION,
		   device.GetAddressOf(),
		   nullptr,
		   deviceContext.GetAddressOf()
	   ));

	perFrameCB.Init(device.Get());
	InitializeGBuffer();
	InitializeDepthStencil();
	InitializePipeline();

	camera.SetPerspective(
		80, (float)params.width, (float)params.height, 1.0f, 100.0f
	);

	camera.SetDirection(0.0f, 0.0f, 1.0f);
	camera.SetPosition(0.0f, 0.0f, 0.0f);

	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	device->CreateRasterizerState(
		&rasterizerDesc, rasterizerState.GetAddressOf()
	);

	ZeroMemory(&viewport, sizeof(viewport));
	viewport.Width = (float)params.width;
	viewport.Height = (float)params.height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
}

void GellyRenderer::InitializeGBuffer() {
	gbuffer.width = (float)params.width;
	gbuffer.height = (float)params.height;

	gbuffer.output.depth =
		d3d11::Texture(*params.sharedTextures.depth, device.Get());

	gbuffer.output.normal =
		d3d11::Texture(*params.sharedTextures.normal, device.Get());

	gbuffer.depth = d3d11::Texture(
		params.width,
		params.height,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		device.Get()
	);

	gbuffer.filteredDepth = d3d11::Texture(
		params.width,
		params.height,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		device.Get()
	);

	gbuffer.normal = d3d11::Texture(
		params.width,
		params.height,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		device.Get()
	);
}

void GellyRenderer::InitializeDepthStencil() {
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.Width = params.width;
	depthStencilDesc.Height = params.height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	DX("Failed to create depth buffer",
	   device->CreateTexture2D(
		   &depthStencilDesc, nullptr, depthStencil.buffer.GetAddressOf()
	   ));

	D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
	ZeroMemory(&depthStencilStateDesc, sizeof(depthStencilStateDesc));
	depthStencilStateDesc.DepthEnable = true;
	depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS;

	DX("Failed to create depth stencil state",
	   device->CreateDepthStencilState(
		   &depthStencilStateDesc, depthStencil.state.GetAddressOf()
	   ));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	DX("Failed to create depth stencil view",
	   device->CreateDepthStencilView(
		   depthStencil.buffer.Get(), &dsvDesc, depthStencil.view.GetAddressOf()
	   ));
}

void GellyRenderer::Render() {
	// Set up the viewport
	deviceContext->RSSetViewports(1, &viewport);
	deviceContext->RSSetState(rasterizerState.Get());

	// Set depth state
	deviceContext->OMSetDepthStencilState(depthStencil.state.Get(), 1);

	PerFrameCBuffer perFrameData = {
		.res = {gbuffer.width, gbuffer.height},
		.padding = 0.f,
		.fov = camera.GetFOV(),
		.projection = camera.GetProjectionMatrix(),
		.view = camera.GetViewMatrix(),
		.invProj = camera.GetInvProjectionMatrix(),
		.invView = camera.GetInvViewMatrix(),
		.eye = camera.GetPosition(),
		.particleRadius = particleRadius};

	perFrameCB.Set(deviceContext.Get(), &perFrameData);

	TechniqueResources resources{
		.perFrameCB = &perFrameCB,
		.camera = &camera,
		.gbuffer = &gbuffer,
		.dsv = depthStencil.view.Get()};

	pipeline.particleRendering->activeParticles = activeParticles;
	pipeline.particleRendering->RunForFrame(deviceContext.Get(), &resources);
	pipeline.isosurfaceExtraction->RunForFrame(deviceContext.Get(), &resources);
	pipeline.outputEncoder->RunForFrame(deviceContext.Get(), &resources);
}

ID3D11Buffer *GellyRenderer::GetD3DParticleBuffer() const {
	return pipeline.particleRendering->GetParticleBuffer();
}

ID3D11Buffer *GellyRenderer::GetD3DNeighborBuffer() const {
	return pipeline.isosurfaceExtraction->GetNeighborBuffer();
}

ID3D11Buffer *GellyRenderer::GetD3DNeighborCountBuffer() const {
	return pipeline.isosurfaceExtraction->GetNeighborCountBuffer();
}

ID3D11Buffer *GellyRenderer::GetD3DInternalToAPIBuffer() const {
	return pipeline.isosurfaceExtraction->GetInternalToAPIBuffer();
}

ID3D11Buffer *GellyRenderer::GetD3DAPIToInternalBuffer() const {
	return pipeline.isosurfaceExtraction->GetAPIToInternalBuffer();
}

GellyRenderer::~GellyRenderer() {
	delete pipeline.particleRendering;
	delete pipeline.isosurfaceExtraction;
	delete pipeline.outputEncoder;
}

#ifdef _DEBUG
void GellyRenderer::PrintDebugMessages() {
	// Lazily initialize the debug message queue
	if (debugMsgQueue == nullptr) {
		DX("Failed to get debug message queue",
		   device.Get()->QueryInterface(
			   __uuidof(ID3D11InfoQueue), (void **)debugMsgQueue.GetAddressOf()
		   ));
	}

	// Print all the messages
	UINT64 messageCount = debugMsgQueue->GetNumStoredMessages();
	for (UINT64 i = 0; i < messageCount; i++) {
		SIZE_T messageLength = 0;
		DX("Failed to get message length",
		   debugMsgQueue->GetMessage(i, nullptr, &messageLength));
		auto *message = new D3D11_MESSAGE[messageLength];
		DX("Failed to get message",
		   debugMsgQueue->GetMessage(i, message, &messageLength));
		printf("[D3D11 Debug Layer]: %s\n", message->pDescription);
		delete[] message;
	}
}
#endif

void GellyRenderer::InitializePipeline() {
	auto *particleRendering =
		new ParticleRendering(device.Get(), params.maxParticles);
	pipeline.particleRendering = particleRendering;

	auto *isosurfaceExtraction = new IsosurfaceExtraction(
		device.Get(),
		particleRendering->GetParticleBuffer(),
		params.maxParticles
	);
	pipeline.isosurfaceExtraction = isosurfaceExtraction;

	auto *depthSmoothing = new DepthSmoothing(device.Get());
	pipeline.depthSmoothing = depthSmoothing;

	auto *normalEstimation = new NormalEstimation(device.Get());
	pipeline.normalEstimation = normalEstimation;

	auto *outputEncoder = new OutputEncoder(device.Get());
	pipeline.outputEncoder = outputEncoder;
}

void GellyRenderer::SetActiveParticles(int newActiveParticles) {
	activeParticles = newActiveParticles;
}

void GellyRenderer::SetParticleRadius(float newParticleRadius) {
	particleRadius = newParticleRadius;
}

ID3D11Device *GellyRenderer::GetD3DDevice() const { return device.Get(); }

GellyRenderer *GellyRenderer_Create(const RendererInitParams &params) {
	return new GellyRenderer(params);
}

void GellyRenderer_Destroy(GellyRenderer *renderer) { delete renderer; }
