#include <GellyRenderer.h>

#include "detail/ErrorHandling.h"
#include "rendering/techniques/ParticleRendering.h"

RendererResources::RendererResources(
	ID3D11Device *device, const RendererInitParams &params
)
	: gbuffer(
		  {.depth_low =
			   d3d11::Texture(*params.sharedTextures.depth_low, device),
		   .depth_high =
			   d3d11::Texture(*params.sharedTextures.depth_high, device),
		   .normal = d3d11::Texture(*params.sharedTextures.normal, device)}
	  ) {
	// Create depth stencil buffer
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

	// Stencil state
	D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
	ZeroMemory(&depthStencilStateDesc, sizeof(depthStencilStateDesc));
	// MENTAL RECAP: THIS MAKES EVERYTHING BLACK. WHY?
	depthStencilStateDesc.DepthEnable = true;
	depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilStateDesc.StencilEnable = false;
	depthStencilStateDesc.StencilReadMask = 0xFF;
	depthStencilStateDesc.StencilWriteMask = 0xFF;

	depthStencilStateDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilStateDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilStateDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilStateDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilStateDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilStateDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilStateDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilStateDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	DX("Failed to create depth stencil state",
	   device->CreateDepthStencilState(
		   &depthStencilStateDesc, depthStencil.state.GetAddressOf()
	   ));

	// Depth stencil view

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

GellyRenderer::GellyRenderer(const RendererInitParams &params)
	: device(nullptr),
	  deviceContext(nullptr),
	  resources(nullptr),
	  params(params),
	  pipeline({}) {
	D3D_FEATURE_LEVEL featureLevel[1] = {D3D_FEATURE_LEVEL_11_1};

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

	resources = new RendererResources(device.Get(), params);
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
}

void GellyRenderer::Render() {
	// Set up the viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(viewport));
	viewport.Width = (float)params.width;
	viewport.Height = (float)params.height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	deviceContext->RSSetViewports(1, &viewport);
	deviceContext->RSSetState(rasterizerState.Get());

	// Set depth state
	deviceContext->OMSetDepthStencilState(
		resources->depthStencil.state.Get(), 1
	);

	TechniqueRTs rts{
		.width = static_cast<float>(params.width),
		.height = static_cast<float>(params.height),
		.gbuffer = &resources->gbuffer,
		.dsv = resources->depthStencil.view.Get()};

	pipeline.particleRendering->activeParticles = activeParticles;
	pipeline.particleRendering->RunForFrame(deviceContext.Get(), &rts, camera);
}

ID3D11Buffer *GellyRenderer::GetD3DParticleBuffer() const {
	return particles.Get();
}

GellyRenderer::~GellyRenderer() {
	delete resources;
	delete pipeline.particleRendering;
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
	particles.Attach(particleRendering->GetParticleBuffer());
	pipeline.particleRendering = particleRendering;
}

void GellyRenderer::SetActiveParticles(int newActiveParticles) {
	activeParticles = newActiveParticles;
}

ID3D11Device *GellyRenderer::GetD3DDevice() const { return device.Get(); }

GellyRenderer *GellyRenderer_Create(const RendererInitParams &params) {
	return new GellyRenderer(params);
}

void GellyRenderer_Destroy(GellyRenderer *renderer) { delete renderer; }
