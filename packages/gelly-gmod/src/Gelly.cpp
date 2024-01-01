// clang-format off
#include <BSPParser.h>
// clang-format on

#include "Gelly.h"

#include <DirectXMath.h>
#include <GMFS.h>

#include <tracy/Tracy.hpp>

#include "CompositePS.h"
#include "LoggingMacros.h"
#include "NDCQuadVS.h"
#include "source/GetCubemap.h"
#include "source/IBaseClientDLL.h"
#include "source/IVRenderView.h"

#ifdef _DEBUG
static const int defaultMaxParticles = 10000;
#else
static const int defaultMaxParticles = 500000;
#endif

void GellyIntegration::CreateShaders() {
	LOG_DX_CALL(
		"Failed to create composite pixel shader",
		device->CreatePixelShader(
			reinterpret_cast<const DWORD *>(gsc::CompositePS::GetBytecode()),
			&shaders.compositePS
		)
	);

	LOG_DX_CALL(
		"Failed to create NDC quad vertex shader",
		device->CreateVertexShader(
			reinterpret_cast<const DWORD *>(gsc::NDCQuadVS::GetBytecode()),
			&shaders.ndcQuadVS
		)
	);
}

void GellyIntegration::CreateBuffers() {
	NDCVertex quadVertices[] = {
		{-1.f, -1.f, 0.f, 1.f, 0.f, 1.f},
		{-1.f, 1.f, 0.f, 1.f, 0.f, 0.f},
		{1.f, -1.f, 0.f, 1.f, 1.f, 1.f},
		{1.f, 1.f, 0.f, 1.f, 1.f, 0.f}
	};

	LOG_DX_CALL(
		"Failed to create NDC quad vertex buffer",
		device->CreateVertexBuffer(
			sizeof(quadVertices),
			D3DUSAGE_WRITEONLY,
			0,
			D3DPOOL_DEFAULT,
			&buffers.ndcQuadVB,
			nullptr
		)
	);

	void *bufferData = nullptr;
	LOG_DX_CALL(
		"Failed to lock NDC quad vertex buffer",
		buffers.ndcQuadVB->Lock(0, sizeof(quadVertices), &bufferData, 0)
	);

	std::memcpy(bufferData, quadVertices, sizeof(quadVertices));

	LOG_DX_CALL(
		"Failed to unlock NDC quad vertex buffer", buffers.ndcQuadVB->Unlock()
	);

	LOG_DX_CALL(
		"Failed to create state block",
		device->CreateStateBlock(D3DSBT_ALL, &stateBlock)
	);
}

void GellyIntegration::CreateTextures() {
	uint16_t width, height;
	renderContext->GetDimensions(width, height);

	LOG_DX_CALL(
		"Failed to create D3D9-side depth texture",
		device->CreateTexture(
			width,
			height,
			1,
			D3DUSAGE_RENDERTARGET,
			D3DFMT_A32B32G32R32F,
			D3DPOOL_DEFAULT,
			&textures.depthTexture,
			&sharedHandles.depthTexture
		)
	);

	LOG_DX_CALL(
		"Failed to create D3D9-side albedo texture",
		device->CreateTexture(
			width,
			height,
			1,
			D3DUSAGE_RENDERTARGET,
			D3DFMT_A32B32G32R32F,
			D3DPOOL_DEFAULT,
			&textures.albedoTexture,
			&sharedHandles.albedoTexture
		)
	);

	LOG_DX_CALL(
		"Failed to create D3D9-side normal texture",
		device->CreateTexture(
			width,
			height,
			1,
			D3DUSAGE_RENDERTARGET,
			D3DFMT_A32B32G32R32F,
			D3DPOOL_DEFAULT,
			&textures.normalTexture,
			&sharedHandles.normalTexture
		)
	);

	LOG_DX_CALL(
		"Failed to create D3D9-side position texture",
		device->CreateTexture(
			width,
			height,
			1,
			D3DUSAGE_RENDERTARGET,
			D3DFMT_A32B32G32R32F,
			D3DPOOL_DEFAULT,
			&textures.positionTexture,
			&sharedHandles.positionTexture
		)
	);

	LOG_DX_CALL(
		"Failed to create D3D9-side thickness texture",
		device->CreateTexture(
			width,
			height,
			1,
			D3DUSAGE_RENDERTARGET,
			D3DFMT_A16B16G16R16F,
			D3DPOOL_DEFAULT,
			&textures.thicknessTexture,
			&sharedHandles.thicknessTexture
		)
	);

	// unrelated to gelly, this is a texture just to store the backbuffer
	LOG_DX_CALL(
		"Failed to create D3D9-side backbuffer texture",
		device->CreateTexture(
			width,
			height,
			1,
			D3DUSAGE_RENDERTARGET,
			D3DFMT_A8R8G8B8,
			D3DPOOL_DEFAULT,
			&textures.backbufferTexture,
			nullptr
		)
	);

	gellyTextures.depthTexture = renderContext->CreateSharedTexture(
		"gelly-gmod/depth", sharedHandles.depthTexture, ContextRenderAPI::D3D9Ex
	);
	gellyTextures.albedoTexture = renderContext->CreateSharedTexture(
		"gelly-gmod/albedo",
		sharedHandles.albedoTexture,
		ContextRenderAPI::D3D9Ex
	);
	gellyTextures.normalTexture = renderContext->CreateSharedTexture(
		"gelly-gmod/normal",
		sharedHandles.normalTexture,
		ContextRenderAPI::D3D9Ex
	);
	gellyTextures.positionTexture = renderContext->CreateSharedTexture(
		"gelly-gmod/position",
		sharedHandles.positionTexture,
		ContextRenderAPI::D3D9Ex
	);
	gellyTextures.thicknessTexture = renderContext->CreateSharedTexture(
		"gelly-gmod/thickness",
		sharedHandles.thicknessTexture,
		ContextRenderAPI::D3D9Ex
	);
}

void GellyIntegration::LinkTextures() const {
	auto *fluidTextures = renderer->GetFluidTextures();
	fluidTextures->SetFeatureTexture(DEPTH, gellyTextures.depthTexture);
	fluidTextures->SetFeatureTexture(ALBEDO, gellyTextures.albedoTexture);
	fluidTextures->SetFeatureTexture(NORMALS, gellyTextures.normalTexture);
	fluidTextures->SetFeatureTexture(POSITIONS, gellyTextures.positionTexture);
	fluidTextures->SetFeatureTexture(THICKNESS, gellyTextures.thicknessTexture);
}

void GellyIntegration::SetCompositeConstants() {
	// At the core, constants are nothing more than contiguous sequences of
	// float4s, so we can use that to our advantage here in D3D9
	device->SetPixelShaderConstantF(
		0,
		reinterpret_cast<const float *>(&compositeConstants),
		sizeof(compositeConstants) / sizeof(float) / 4
	);
}

void GellyIntegration::UpdateRenderParams() {
	ZoneScoped;

	CViewSetup currentView = {};
	GetClientViewSetup(currentView);

	XMFLOAT4X4 viewMatrix = {};
	XMFLOAT4X4 projectionMatrix = {};
	XMFLOAT4X4 _unused1 = {};
	XMFLOAT4X4 _unused2 = {};

	GetMatricesFromView(
		currentView,
		reinterpret_cast<VMatrix *>(&viewMatrix),
		reinterpret_cast<VMatrix *>(&projectionMatrix),
		reinterpret_cast<VMatrix *>(&_unused1),
		reinterpret_cast<VMatrix *>(&_unused2)
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

	// And finally we can transpose all of these matrices
	XMStoreFloat4x4(
		&viewMatrix, XMMatrixTranspose(XMLoadFloat4x4(&viewMatrix))
	);
	XMStoreFloat4x4(
		&projectionMatrix, XMMatrixTranspose(XMLoadFloat4x4(&projectionMatrix))
	);
	XMStoreFloat4x4(
		&inverseViewMatrix,
		XMMatrixTranspose(XMLoadFloat4x4(&inverseViewMatrix))
	);
	XMStoreFloat4x4(
		&inverseProjectionMatrix,
		XMMatrixTranspose(XMLoadFloat4x4(&inverseProjectionMatrix))
	);

	renderParams.view = viewMatrix;
	renderParams.proj = projectionMatrix;
	renderParams.invView = inverseViewMatrix;
	renderParams.invProj = inverseProjectionMatrix;
	renderParams.particleRadius = particleRadius * 1.1f;
	renderParams.thresholdRatio = thresholdRatio;

	renderParams.width = static_cast<float>(currentView.width);
	renderParams.height = static_cast<float>(currentView.height);
	renderParams.farPlane = currentView.zFar;
	renderParams.nearPlane = currentView.zNear;

	compositeConstants.eyePos[0] = currentView.origin.x;
	compositeConstants.eyePos[1] = currentView.origin.y;
	compositeConstants.eyePos[2] = currentView.origin.z;
}

GellyIntegration::GellyIntegration(
	uint16_t width, uint16_t height, IDirect3DDevice9Ex *device
)
	: device(device), textures({}) {
	try {
		renderContext = CreateD3D11FluidRenderContext(width, height);
		LOG_INFO("Created render context");
		renderer = CreateD3D11DebugFluidRenderer(renderContext);
		LOG_INFO("Created renderer");
		simContext = CreateD3D11SimContext(
			static_cast<ID3D11Device *>(renderContext->GetRenderAPIResource(
				RenderAPIResource::D3D11Device
			)),
			static_cast<ID3D11DeviceContext *>(
				renderContext->GetRenderAPIResource(
					RenderAPIResource::D3D11DeviceContext
				)
			)
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
			isSimulationInteractive = true;
		} else {
			LOG_INFO("Interactivity is not supported");
		}

		LOG_INFO("Querying for two-way physics coupling support...");
		if (simulation->CheckFeatureSupport(
				GELLY_FEATURE::FLUIDSIM_CONTACTPLANES
			)) {
			LOG_INFO("Two-way physics coupling is supported");
			entityCollisionSupported = true;
		} else {
			LOG_INFO("Two-way physics coupling is not supported");
		}

		LOG_INFO("Querying for per-particle absorption support...");
		if (renderer->CheckFeatureSupport(
				GELLY_FEATURE::FLUIDRENDER_PER_PARTICLE_ABSORPTION
			)) {
			LOG_INFO("Per-particle absorption is supported");
			perParticleAbsorptionSupported = true;
		} else {
			LOG_INFO("Per-particle absorption is not supported");
		}

		CreateTextures();
		LOG_INFO("Created D3D9-side textures");
		LinkTextures();
		LOG_INFO("Linked D3D9 textures with Gelly");

		CreateShaders();
		LOG_INFO("Created shaders");
		CreateBuffers();
		LOG_INFO("Created buffers");

		ChangeParticleRadius(particleRadius);

#ifdef _DEBUG
		LOG_INFO("Debugging detected, enabling RenderDoc integration...");
		if (const auto success = renderer->EnableRenderDocCaptures();
			!success) {
			LOG_WARNING(
				"Failed to enable captures, maybe RenderDoc is not running or "
				"the API has changed?"
			);
		} else {
			LOG_INFO("RenderDoc captures enabled");
		}
#endif
	} catch (const std::exception &e) {
		LOG_ERROR("Failed to create render context: %s", e.what());
	}
}

void GellyIntegration::Render() {
	ZoneScoped;
	{
		ZoneScopedN("Back buffer copy");
		// copy backbuffer to texture
		IDirect3DSurface9 *backbufferSurface = nullptr;
		IDirect3DSurface9 *backbufferTextureSurface = nullptr;
		LOG_DX_CALL(
			"Failed to get backbuffer surface",
			device->GetRenderTarget(0, &backbufferSurface)
		);
		LOG_DX_CALL(
			"Failed to get backbuffer texture surface",
			textures.backbufferTexture->GetSurfaceLevel(
				0, &backbufferTextureSurface
			)
		);

		LOG_DX_CALL(
			"Failed to copy backbuffer to texture",
			device->StretchRect(
				backbufferSurface,
				nullptr,
				backbufferTextureSurface,
				nullptr,
				D3DTEXF_NONE
			)
		);
	}

	UpdateRenderParams();
	{
		ZoneScopedN("Gelly render");
		{
			ZoneScopedN("Per frame params");
			renderer->SetPerFrameParams(renderParams);
		}
		renderer->Render();
	}

	// get current state
	stateBlock->Capture();

	{
		ZoneScopedN("Composite");
		SetCompositeConstants();
		device->SetVertexShader(shaders.ndcQuadVS);
		device->SetPixelShader(shaders.compositePS);
		device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

		device->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		device->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		device->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		device->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		device->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

		device->SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		device->SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		device->SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		device->SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		device->SetSamplerState(2, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

		device->SetSamplerState(3, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		device->SetSamplerState(3, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		device->SetSamplerState(3, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		device->SetSamplerState(3, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		device->SetSamplerState(3, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

		device->SetSamplerState(4, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		device->SetSamplerState(4, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		device->SetSamplerState(4, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(4, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(4, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

		device->SetSamplerState(5, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		device->SetSamplerState(5, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		device->SetSamplerState(5, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(5, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(5, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

		device->SetSamplerState(6, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		device->SetSamplerState(6, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		device->SetSamplerState(6, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(6, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(6, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

		device->SetTexture(0, textures.depthTexture);
		device->SetTexture(1, textures.normalTexture);
		device->SetTexture(2, textures.positionTexture);
		device->SetTexture(3, textures.backbufferTexture);
		device->SetTexture(4, textures.thicknessTexture);
		device->SetTexture(5, GetCubemap());
		device->SetTexture(6, textures.albedoTexture);

		device->SetStreamSource(0, buffers.ndcQuadVB, 0, sizeof(NDCVertex));
		device->SetFVF(D3DFVF_XYZW | D3DFVF_TEX1);

		device->SetRenderState(D3DRS_ZENABLE, TRUE);
		device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	}

	// Restore state
	stateBlock->Apply();
}

void GellyIntegration::Simulate(float dt) {
	simulation->Update(dt);
	// update
	simulation->GetScene()->Update();
}

void GellyIntegration::LoadMap(const char *mapName) {
	if (!isSimulationInteractive) {
		LOG_WARNING("Simulation is not interactive, cannot load map");
		return;
	}

	if (!FileSystem::Exists(mapName)) {
		LOG_WARNING("Map %s does not exist", mapName);
		return;
	}

	const auto handle = FileSystem::Open(mapName, "rb");
	const auto size = FileSystem::Size(handle);
	const auto buffer = new byte[size];
	FileSystem::Read(buffer, size, handle);

	const BSPMap mapParser(buffer, size);
	if (!mapParser.IsValid()) {
		LOG_ERROR("Failed to parse map %s", mapName);
		throw std::runtime_error("Failed to parse map");
	}

	const auto *vertices = mapParser.GetVertices();
	const auto vertexCount = mapParser.GetNumTris() * 3;

	// maps dont have indices, but they do have to be long since maps can be
	// huge
	auto *indices = new uint32_t[vertexCount];
	for (int i = 0; i < vertexCount; i += 3) {
		// flip winding order
		indices[i] = i + 2;
		indices[i + 1] = i + 1;
		indices[i + 2] = i;
	}

	ObjectCreationParams params = {};
	params.shape = ObjectShape::TRIANGLE_MESH;

	ObjectCreationParams::TriangleMesh mesh = {};
	mesh.indexType = ObjectCreationParams::TriangleMesh::IndexType::UINT32;
	mesh.vertices = reinterpret_cast<const float *>(vertices);
	mesh.indices32 = indices;
	mesh.vertexCount = vertexCount;
	mesh.indexCount = vertexCount;
	mesh.scale[0] = 1.f;
	mesh.scale[1] = 1.f;
	mesh.scale[2] = 1.f;

	params.shapeData = mesh;

	mapHandle = simulation->GetScene()->CreateObject(params);
	LOG_INFO("Loaded map %s with scene handle %d", mapName, mapHandle);

	delete[] buffer;
	delete[] indices;
}

const char *GellyIntegration::GetComputeDeviceName() const {
	return simulation->GetComputeDeviceName();
}

void GellyIntegration::SetFluidParams(const FluidVisualParams &params) {
	std::memcpy(
		compositeConstants.absorption,
		params.absorption,
		sizeof(params.absorption)
	);
	compositeConstants.refractionStrength = params.refractionStrength;
}

void GellyIntegration::ChangeParticleRadius(float radius) {
	particleRadius = radius;

	ISimCommandList *commandList = simulation->CreateCommandList();
	commandList->AddCommand({CHANGE_RADIUS, ChangeRadius{particleRadius}});
	simulation->ExecuteCommandList(commandList);
	simulation->DestroyCommandList(commandList);
	LOG_INFO("Sent particle radius commands to simulation");
}

void GellyIntegration::ChangeThresholdRatio(float ratio) {
	thresholdRatio = ratio;
}

bool GellyIntegration::IsInteractive() const { return isSimulationInteractive; }

bool GellyIntegration::IsEntityCollisionSupported() const {
	return entityCollisionSupported;
}

bool GellyIntegration::IsPerParticleAbsorptionSupported() const {
	return perParticleAbsorptionSupported;
}

IFluidRenderer *GellyIntegration::GetRenderer() const { return renderer; }
IFluidSimulation *GellyIntegration::GetSimulation() const { return simulation; }

XMFLOAT3 GellyIntegration::GetCurrentAbsorption() const {
	const XMFLOAT3 absorption = {
		compositeConstants.absorption[0],
		compositeConstants.absorption[1],
		compositeConstants.absorption[2]
	};

	return absorption;
}

GellyIntegration::~GellyIntegration() {
	if (renderContext) {
		// soon
	}
}