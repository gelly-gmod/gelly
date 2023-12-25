#include "Gelly.h"

#include <DirectXMath.h>

#include "LoggingMacros.h"
#include "source/IBaseClientDLL.h"
#include "source/IVRenderView.h"

static const int defaultMaxParticles = 100000;

void GellyIntegration::CreateTextures() {
	uint16_t width, height;
	renderContext->GetDimensions(width, height);

	LOG_DX_CALL("Failed to create D3D9-side depth texture",
		device->CreateTexture(
			width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &textures.depthTexture, &sharedHandles.depthTexture
		));

	LOG_DX_CALL("Failed to create D3D9-side albedo texture",
		device->CreateTexture(
			width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &textures.albedoTexture, &sharedHandles.albedoTexture
		));

	LOG_DX_CALL("Failed to create D3D9-side normal texture",
		device->CreateTexture(
			width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &textures.normalTexture, &sharedHandles.normalTexture
		));

	LOG_DX_CALL("Failed to create D3D9-side position texture",
		device->CreateTexture(
			width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &textures.positionTexture, &sharedHandles.positionTexture
		));

	LOG_DX_CALL("Failed to create D3D9-side thickness texture",
		device->CreateTexture(
			width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &textures.thicknessTexture, &sharedHandles.thicknessTexture
		));

	gellyTextures.depthTexture = renderContext->CreateSharedTexture("gelly-gmod/depth", sharedHandles.depthTexture, ContextRenderAPI::D3D9Ex);
	gellyTextures.albedoTexture = renderContext->CreateSharedTexture("gelly-gmod/albedo", sharedHandles.albedoTexture, ContextRenderAPI::D3D9Ex);
	gellyTextures.normalTexture = renderContext->CreateSharedTexture("gelly-gmod/normal", sharedHandles.normalTexture, ContextRenderAPI::D3D9Ex);
	gellyTextures.positionTexture = renderContext->CreateSharedTexture("gelly-gmod/position", sharedHandles.positionTexture, ContextRenderAPI::D3D9Ex);
	gellyTextures.thicknessTexture = renderContext->CreateSharedTexture("gelly-gmod/thickness", sharedHandles.thicknessTexture, ContextRenderAPI::D3D9Ex);
}

void GellyIntegration::LinkTextures() const {
	auto* fluidTextures = renderer->GetFluidTextures();
	fluidTextures->SetFeatureTexture(DEPTH, gellyTextures.depthTexture);
	fluidTextures->SetFeatureTexture(ALBEDO, gellyTextures.albedoTexture);
	fluidTextures->SetFeatureTexture(NORMALS, gellyTextures.normalTexture);
	fluidTextures->SetFeatureTexture(POSITIONS, gellyTextures.positionTexture);
	fluidTextures->SetFeatureTexture(THICKNESS, gellyTextures.thicknessTexture);
}

void GellyIntegration::UpdateRenderParams() {
	CViewSetup currentView = {};
	GetClientViewSetup(currentView);

	XMFLOAT4X4 viewMatrix = {};
	XMFLOAT4X4 projectionMatrix = {};
	XMFLOAT4X4 _unused1 = {};
	XMFLOAT4X4 _unused2 = {};

	GetMatricesFromView(
		currentView,
		reinterpret_cast<VMatrix*>(&viewMatrix),
		reinterpret_cast<VMatrix*>(&projectionMatrix),
		reinterpret_cast<VMatrix*>(&_unused1),
		reinterpret_cast<VMatrix*>(&_unused2)
	);

	XMFLOAT4X4 inverseViewMatrix = {};
	XMFLOAT4X4 inverseProjectionMatrix = {};

	XMStoreFloat4x4(
		&inverseViewMatrix,
		XMMatrixInverse(nullptr, XMLoadFloat4x4(&viewMatrix))
	);

	XMStoreFloat4x4(
		&inverseProjectionMatrix,
		XMMatrixInverse(nullptr, XMLoadFloat4x4(&projectionMatrix))
	);

	renderParams.view = viewMatrix;
	renderParams.proj = projectionMatrix;
	renderParams.invView = inverseViewMatrix;
	renderParams.invProj = inverseProjectionMatrix;
	renderParams.particleRadius = particleRadius;
	renderParams.thresholdRatio = thresholdRatio;

	renderParams.width = static_cast<float>(currentView.width);
	renderParams.height = static_cast<float>(currentView.height);
	renderParams.farPlane = currentView.zFar;
	renderParams.nearPlane = currentView.zNear;
}

GellyIntegration::GellyIntegration(uint16_t width, uint16_t height, IDirect3DDevice9Ex *device) :
	device(device),
	textures({})
{
	try {
		renderContext = CreateD3D11FluidRenderContext(width, height);
		LOG_INFO("Created render context");
		renderer = CreateD3D11DebugFluidRenderer(renderContext);
		LOG_INFO("Created renderer");
		simContext = CreateD3D11SimContext(
			static_cast<ID3D11Device *>(renderContext->GetRenderAPIResource(RenderAPIResource::D3D11Device)),
			static_cast<ID3D11DeviceContext *>(renderContext->GetRenderAPIResource(RenderAPIResource::D3D11DeviceContext))
		);
		LOG_INFO("Created simulation context");
		simulation = CreateD3D11FlexFluidSimulation(simContext);
		LOG_INFO("Created FleX simulation");

		simulation->SetMaxParticles(defaultMaxParticles);
		renderer->SetSimData(simulation->GetSimulationData());

		simulation->Initialize();
		LOG_INFO("Linked simulation and renderer");

		LOG_INFO("Querying for interactivity support...");
		if (simulation->GetScene()) {
			LOG_INFO("Interactivity is supported");
		}

		CreateTextures();
		LOG_INFO("Created D3D9-side textures");
		LinkTextures();
		LOG_INFO("Linked D3D9 textures with Gelly");
	} catch (const std::exception &e) {
		LOG_ERROR("Failed to create render context: %s", e.what());
	}
}

void GellyIntegration::Render() {
	renderer->SetPerFrameParams(renderParams);
	renderer->Render();
}

GellyIntegration::~GellyIntegration() {
	if (renderContext) {
		// soon
	}
}