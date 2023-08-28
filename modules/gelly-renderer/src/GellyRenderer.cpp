#include <GellyRenderer.h>

#include "detail/ErrorHandling.h"
#include "rendering/techniques/ParticleRendering.h"

RendererResources::RendererResources(
	ID3D11Device *device, const RendererInitParams &params
)
	: gbuffer({.normal = nullptr}) {
	ID3D11Resource *normalResource = nullptr;
	DX("Failed to open the texture in D3D11 from D3D9!",
	   device->OpenSharedResource(
		   *params.sharedTextures.normal,
		   __uuidof(ID3D11Resource),
		   (void **)&normalResource
	   ));

	DX("Failed to query D3D11Texture2D from the shared resource!",
	   normalResource->QueryInterface(
		   __uuidof(ID3D11Texture2D), (void **)gbuffer.normal.GetAddressOf()
	   ));

	DX("Failed to release the shared resource!", normalResource->Release());

	// Create render target views for the gbuffer textures

	D3D11_RENDER_TARGET_VIEW_DESC normalRTVDesc;
	ZeroMemory(&normalRTVDesc, sizeof(normalRTVDesc));

	normalRTVDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	normalRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	normalRTVDesc.Texture2D.MipSlice = 0;

	DX("Failed to make normal RTV",
	   device->CreateRenderTargetView(
		   gbuffer.normal.Get(),
		   &normalRTVDesc,
		   gbuffer.normalRTV.GetAddressOf()
	   ));
}

GellyRenderer::GellyRenderer(const RendererInitParams &params)
	: device(nullptr),
	  deviceContext(nullptr),
	  resources(nullptr),
	  params(params),
	  pipeline({}) {
	D3D_FEATURE_LEVEL featureLevel[1] = {D3D_FEATURE_LEVEL_11_1};

	UINT flags = 0;
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

	InitializePipeline();
	resources = new RendererResources(device.Get(), params);

	camera.SetPerspective(
		0.5, (float)params.width / (float)params.height, 0.01f, 1000.0f
	);
	camera.SetPosition(0.0f, 0.0f, 0.0f);
	camera.SetRotation(0.0f, 0.0f, 0.0f);

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

	TechniqueRTs rts{// float2 for compatability with SHADERed.
					 .width = static_cast<float>(params.width),
					 .height = static_cast<float>(params.height),
					 .normal = resources->gbuffer.normalRTV.Get()};

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
	particles = particleRendering->GetParticleBuffer();
	pipeline.particleRendering = particleRendering;
}

void GellyRenderer::SetActiveParticles(int newActiveParticles) {
	activeParticles = newActiveParticles;
}

GellyRenderer *GellyRenderer_Create(const RendererInitParams &params) {
	return new GellyRenderer(params);
}

void GellyRenderer_Destroy(GellyRenderer *renderer) { delete renderer; }
