#include <BSPParser.h>
#include "Gelly.h"

#include <DirectXMath.h>
#include <GMFS.h>

#include "CompositePS.h"
#include "LoggingMacros.h"
#include "NDCQuadVS.h"
#include "source/IBaseClientDLL.h"
#include "source/IVRenderView.h"

static const int defaultMaxParticles = 1000000;

void GellyIntegration::CreateShaders() {
	LOG_DX_CALL("Failed to create composite pixel shader",
		device->CreatePixelShader(
			reinterpret_cast<const DWORD*>(gsc::CompositePS::GetBytecode()), &shaders.compositePS
		));

	LOG_DX_CALL("Failed to create NDC quad vertex shader",
		device->CreateVertexShader(
			reinterpret_cast<const DWORD*>(gsc::NDCQuadVS::GetBytecode()), &shaders.ndcQuadVS
		));
}

void GellyIntegration::CreateBuffers() {
	NDCVertex quadVertices[] = {
		{ -1.f, -1.f, 0.f, 1.f, 0.f, 1.f },
		{ -1.f, 1.f, 0.f, 1.f, 0.f, 0.f },
		{ 1.f, -1.f, 0.f, 1.f, 1.f, 1.f },
		{ 1.f, 1.f, 0.f, 1.f, 1.f, 0.f }
	};

	LOG_DX_CALL("Failed to create NDC quad vertex buffer",
		device->CreateVertexBuffer(
			sizeof(quadVertices), D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &buffers.ndcQuadVB, nullptr
		));

	void* bufferData = nullptr;
	LOG_DX_CALL("Failed to lock NDC quad vertex buffer",
		buffers.ndcQuadVB->Lock(0, sizeof(quadVertices), &bufferData, 0));

	std::memcpy(bufferData, quadVertices, sizeof(quadVertices));

	LOG_DX_CALL("Failed to unlock NDC quad vertex buffer",
		buffers.ndcQuadVB->Unlock());

	LOG_DX_CALL("Failed to create state block",
		device->CreateStateBlock(D3DSBT_ALL, &stateBlock));

}


void GellyIntegration::CreateTextures() {
	uint16_t width, height;
	renderContext->GetDimensions(width, height);

	LOG_DX_CALL("Failed to create D3D9-side depth texture",
		device->CreateTexture(
			width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT, &textures.depthTexture, &sharedHandles.depthTexture
		));

	LOG_DX_CALL("Failed to create D3D9-side albedo texture",
		device->CreateTexture(
			width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT, &textures.albedoTexture, &sharedHandles.albedoTexture
		));

	LOG_DX_CALL("Failed to create D3D9-side normal texture",
		device->CreateTexture(
			width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT, &textures.normalTexture, &sharedHandles.normalTexture
		));

	LOG_DX_CALL("Failed to create D3D9-side position texture",
		device->CreateTexture(
			width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT, &textures.positionTexture, &sharedHandles.positionTexture
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

	// And finally we can transpose all of these matrices
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(XMLoadFloat4x4(&viewMatrix)));
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(XMLoadFloat4x4(&projectionMatrix)));
	XMStoreFloat4x4(&inverseViewMatrix, XMMatrixTranspose(XMLoadFloat4x4(&inverseViewMatrix)));
	XMStoreFloat4x4(&inverseProjectionMatrix, XMMatrixTranspose(XMLoadFloat4x4(&inverseProjectionMatrix)));

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
#ifdef _DEBUG
		simulation = CreateD3D11DebugFluidSimulation(simContext);
#else
		simulation = CreateD3D11FlexFluidSimulation(simContext);
#endif
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

		CreateTextures();
		LOG_INFO("Created D3D9-side textures");
		LinkTextures();
		LOG_INFO("Linked D3D9 textures with Gelly");

		CreateShaders();
		LOG_INFO("Created shaders");
		CreateBuffers();
		LOG_INFO("Created buffers");

		ISimCommandList *commandList = simulation->CreateCommandList();
		commandList->AddCommand({CHANGE_RADIUS, ChangeRadius{ particleRadius }});
		simulation->ExecuteCommandList(commandList);
		simulation->DestroyCommandList(commandList);
		LOG_INFO("Sent initialization commands to simulation");

		renderer->EnableLowBitMode();
		LOG_INFO("Enabled low bit mode");

#ifdef _DEBUG
		LOG_INFO("Debugging detected, enabling RenderDoc integration...");
		if (const auto success = renderer->EnableRenderDocCaptures(); !success) {
			LOG_WARNING("Failed to enable captures, maybe RenderDoc is not running or the API has changed?");
		} else {
			LOG_INFO("RenderDoc captures enabled");
		}
#endif
	} catch (const std::exception &e) {
		LOG_ERROR("Failed to create render context: %s", e.what());
	}
}

void GellyIntegration::Render() {
	UpdateRenderParams();
	renderer->SetPerFrameParams(renderParams);
	renderer->Render();

	// get current state
	stateBlock->Capture();

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

	device->SetTexture(0, textures.depthTexture);
	device->SetTexture(1, textures.normalTexture);

	device->SetStreamSource(0, buffers.ndcQuadVB, 0, sizeof(NDCVertex));
	device->SetFVF(D3DFVF_XYZW | D3DFVF_TEX1);

	device->SetRenderState(D3DRS_ZENABLE, TRUE);
	device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

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

	const auto* vertices = mapParser.GetVertices();
	const auto vertexCount = mapParser.GetNumTris() * 3;

	// maps dont have indices, but they do have to be long since maps can be huge
	auto* indices = new uint32_t[vertexCount];
	for (int i = 0; i < vertexCount; i+=3) {
		// flip winding order
		indices[i] = i + 2;
		indices[i + 1] = i + 1;
		indices[i + 2] = i;
	}

	ObjectCreationParams params = {};
	params.shape = ObjectShape::TRIANGLE_MESH;

	ObjectCreationParams::TriangleMesh mesh = {};
	mesh.indexType = ObjectCreationParams::TriangleMesh::IndexType::UINT32;
	mesh.vertices = reinterpret_cast<const float*>(vertices);
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

IFluidSimulation *GellyIntegration::GetSimulation() const {
	return simulation;
}

GellyIntegration::~GellyIntegration() {
	if (renderContext) {
		// soon
	}
}