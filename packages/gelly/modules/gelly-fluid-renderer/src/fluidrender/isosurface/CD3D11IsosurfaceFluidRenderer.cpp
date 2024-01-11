#include "fluidrender/isosurface/CD3D11IsosurfaceFluidRenderer.h"

#include "ClearBuffersCS.h"
#include "ConstructBDGCS.h"
#include "IsoSplattingEntryPS.h"
#include "IsoSplattingExitPS.h"
#include "IsoSplattingGS.h"
#include "IsoSplattingVS.h"
#include "RaymarchCS.h"
#include "VoxelizeCS.h"
#include "fluidrender/util/CBuffers.h"
#include "fluidrender/util/Exceptions.h"

CD3D11IsosurfaceFluidRenderer::CD3D11IsosurfaceFluidRenderer()
	: m_kernels({}) {}

void CD3D11IsosurfaceFluidRenderer::CreateBuffers() {
	THROW_IF_FALSY(
		m_simData,
		"Renderer must be linked to simulation data before creating buffers"
	);

	THROW_IF_FALSY(m_context, "Renderer must be attached to a context");

	const uint maxParticles = m_simData->GetMaxParticles();
	const uint totalVoxels = m_settings.special.isosurface.domainWidth *
							 m_settings.special.isosurface.domainHeight *
							 m_settings.special.isosurface.domainDepth;
	BufferDesc particlePositionDesc = {};
	particlePositionDesc.format = BufferFormat::R32G32B32A32_FLOAT;
	particlePositionDesc.stride = sizeof(SimFloat4);
	particlePositionDesc.byteWidth = maxParticles * sizeof(SimFloat4);
	particlePositionDesc.type =
		BufferType::SHADER_RESOURCE | BufferType::VERTEX;

	m_buffers.positions = m_context->CreateBuffer(particlePositionDesc);

	BufferDesc particlesInVoxelsDesc = {};
	particlesInVoxelsDesc.format = BufferFormat::R32_UINT;
	particlesInVoxelsDesc.stride = sizeof(uint32_t);
	particlesInVoxelsDesc.byteWidth =
		sizeof(uint32_t) * totalVoxels *
		m_settings.special.isosurface.maxParticlesInVoxel;
	particlesInVoxelsDesc.type = BufferType::UNORDERED_ACCESS;

	m_buffers.particlesInVoxels =
		m_context->CreateBuffer(particlesInVoxelsDesc);

	BufferLayoutDesc particlePositionLayoutDesc = {};
	particlePositionLayoutDesc.items[0] = {
		0, "SV_POSITION", 0, BufferLayoutFormat::FLOAT4
	};
	particlePositionLayoutDesc.itemCount = 1;
	particlePositionLayoutDesc.vertexShader = m_shaders.splattingVS;
	particlePositionLayoutDesc.topology = BufferLayoutTopology::POINTS;

	m_buffers.positionsLayout =
		m_context->CreateBufferLayout(particlePositionLayoutDesc);

	m_buffers.positionsLayout->AttachBufferAtSlot(m_buffers.positions, 0);

	m_buffers.fluidRenderCBuffer =
		util::CreateCBuffer<FluidRenderParams>(m_context);
	m_buffers.voxelCBuffer = util::CreateCBuffer<VoxelCBData>(m_context);

	DepthBufferDesc frontDesc = {};
	frontDesc.format = DepthFormat::D24S8;
	frontDesc.depthOp = DepthOp::LESS_EQUAL;

	m_depthBuffers.front = m_context->CreateDepthBuffer(frontDesc);

	DepthBufferDesc backDesc = {};
	backDesc.format = DepthFormat::D24S8;
	backDesc.depthOp = DepthOp::GREATER_EQUAL;

	m_depthBuffers.back = m_context->CreateDepthBuffer(backDesc);
}

void CD3D11IsosurfaceFluidRenderer::CreateTextures() {
	THROW_IF_FALSY(m_context, "Renderer must be attached to a context");

	if (m_textures.frontDepth) {
		m_context->DestroyTexture("isosurfacerenderer/frontdepth");
	}

	if (m_textures.backDepth) {
		m_context->DestroyTexture("isosurfacerenderer/backdepth");
	}

	if (m_textures.normal) {
		m_context->DestroyTexture("isosurfacerenderer/normal");
	}

	if (m_textures.bdg) {
		m_context->DestroyTexture("isosurfacerenderer/bdg");
	}

	if (m_textures.particleCount) {
		m_context->DestroyTexture("isosurfacerenderer/particlecount");
	}

	uint16_t width, height;
	m_context->GetDimensions(width, height);

	TextureDesc frontDepth = {};
	// Some algorithms perform far better in eye space, so we store eye and proj
	frontDepth.format = TextureFormat::R32G32_FLOAT;
	frontDepth.width = width;
	frontDepth.height = height;
	frontDepth.isFullscreen = true;
	frontDepth.access = TextureAccess::READ | TextureAccess::WRITE;

	m_textures.frontDepth =
		m_context->CreateTexture("isosurfacerenderer/frontdepth", frontDepth);

	const TextureDesc backDepth = frontDepth;
	m_textures.backDepth =
		m_context->CreateTexture("isosurfacerenderer/backdepth", backDepth);

	TextureDesc normalDesc = {};
	normalDesc.format = TextureFormat::R32G32B32A32_FLOAT;
	normalDesc.width = width;
	normalDesc.height = height;
	normalDesc.isFullscreen = true;
	normalDesc.access = TextureAccess::READ | TextureAccess::WRITE;

	m_textures.normal =
		m_context->CreateTexture("isosurfacerenderer/normal", normalDesc);

	TextureDesc bdgDesc = {};
	bdgDesc.format = TextureFormat::R32G32_FLOAT;
	bdgDesc.width = m_settings.special.isosurface.domainWidth;
	bdgDesc.height = m_settings.special.isosurface.domainHeight;
	bdgDesc.depth = m_settings.special.isosurface.domainDepth;
	bdgDesc.access = TextureAccess::READ | TextureAccess::WRITE;
	// We need trilinear filtering for the raymarch kernel
	bdgDesc.filter = TextureFilter::LINEAR;

	m_textures.bdg =
		m_context->CreateTexture("isosurfacerenderer/bdg", bdgDesc);

	TextureDesc particleCountDesc = {};
	particleCountDesc.format = TextureFormat::R32_UINT;
	particleCountDesc.width = m_settings.special.isosurface.domainWidth;
	particleCountDesc.height = m_settings.special.isosurface.domainHeight;
	particleCountDesc.depth = m_settings.special.isosurface.domainDepth;
	particleCountDesc.access = TextureAccess::READ | TextureAccess::WRITE;

	m_textures.particleCount = m_context->CreateTexture(
		"isosurfacerenderer/particlecount", particleCountDesc
	);
}

void CD3D11IsosurfaceFluidRenderer::CreateKernels() {
	THROW_IF_FALSY(m_context, "Renderer must be attached to a context");

	uint16_t width, height;
	m_context->GetDimensions(width, height);

	const uint domainWidth = m_settings.special.isosurface.domainWidth;
	const uint domainHeight = m_settings.special.isosurface.domainHeight;
	const uint domainDepth = m_settings.special.isosurface.domainDepth;

	const uint voxelDispatchX =
		domainWidth / 4;  // The BDG construction kernel runs in 4x4x4 groups
	const uint voxelDispatchY = domainHeight / 4;
	const uint voxelDispatchZ = domainDepth / 4;

	const uint raymarchX = width / 8;  // The raymarch kernel runs in 8x8 groups
	const uint raymarchY = height / 8;

	m_kernels.voxelize.Initialize(
		m_context,
		gsc::VoxelizeCS::GetBytecode(),
		gsc::VoxelizeCS::GetBytecodeSize(),
		{64, 128, 1}  // This causes the amount of threads to be about 528k
					  // which is how many particles we are supporting.
	);

	m_kernels.constructBDG.Initialize(
		m_context,
		gsc::ConstructBDGCS::GetBytecode(),
		gsc::ConstructBDGCS::GetBytecodeSize(),
		{voxelDispatchX, voxelDispatchY, voxelDispatchZ}
	);

	m_kernels.raymarch.Initialize(
		m_context,
		gsc::RaymarchCS::GetBytecode(),
		gsc::RaymarchCS::GetBytecodeSize(),
		{raymarchX, raymarchY, 1}
	);

	m_kernels.clearBuffers.Initialize(
		m_context,
		gsc::ClearBuffersCS::GetBytecode(),
		gsc::ClearBuffersCS::GetBytecodeSize(),
		{voxelDispatchX, voxelDispatchY, voxelDispatchZ}
	);

	m_kernels.voxelize.SetInput(0, m_buffers.positions);
	m_kernels.voxelize.SetOutput(1, m_textures.particleCount);
	m_kernels.voxelize.SetOutput(2, m_buffers.particlesInVoxels);
	m_kernels.voxelize.SetCBuffer(0, m_buffers.voxelCBuffer);

	m_kernels.constructBDG.SetInput(0, m_textures.particleCount);
	m_kernels.constructBDG.SetInput(1, m_buffers.particlesInVoxels);
	m_kernels.constructBDG.SetInput(2, m_buffers.positions);
	m_kernels.constructBDG.SetOutput(3, m_textures.bdg);
	m_kernels.constructBDG.SetCBuffer(0, m_buffers.voxelCBuffer);

	m_kernels.raymarch.SetInput(0, m_textures.bdg);
	m_kernels.raymarch.SetInput(1, m_textures.particleCount);
	m_kernels.raymarch.SetInput(2, m_buffers.particlesInVoxels);
	m_kernels.raymarch.SetInput(3, m_buffers.positions);
	m_kernels.raymarch.SetOutput(4, m_outputTextures.GetFeatureTexture(DEPTH));
	m_kernels.raymarch.SetOutput(
		5, m_outputTextures.GetFeatureTexture(NORMALS)
	);
	m_kernels.raymarch.SetCBuffer(0, m_buffers.voxelCBuffer);
	m_kernels.raymarch.SetCBuffer(1, m_buffers.fluidRenderCBuffer);

	m_kernels.clearBuffers.SetOutput(0, m_textures.particleCount);
	m_kernels.clearBuffers.SetOutput(1, m_buffers.particlesInVoxels);
	m_kernels.clearBuffers.SetCBuffer(0, m_buffers.voxelCBuffer);
};

void CD3D11IsosurfaceFluidRenderer::CreateShaders() {
	m_shaders.splattingVS = m_context->CreateShader(
		gsc::IsoSplattingVS::GetBytecode(),
		gsc::IsoSplattingVS::GetBytecodeSize(),
		ShaderType::Vertex
	);

	m_shaders.splattingGS = m_context->CreateShader(
		gsc::IsoSplattingGS::GetBytecode(),
		gsc::IsoSplattingGS::GetBytecodeSize(),
		ShaderType::Geometry
	);

	m_shaders.splattingEntryPS = m_context->CreateShader(
		gsc::IsoSplattingEntryPS::GetBytecode(),
		gsc::IsoSplattingEntryPS::GetBytecodeSize(),
		ShaderType::Pixel
	);

	m_shaders.splattingExitPS = m_context->CreateShader(
		gsc::IsoSplattingExitPS::GetBytecode(),
		gsc::IsoSplattingExitPS::GetBytecodeSize(),
		ShaderType::Pixel
	);
}

void CD3D11IsosurfaceFluidRenderer::ConstructMarchingBuffers() {
	m_voxelCBData.activeParticles = m_simData->GetActiveParticles();
	util::UpdateCBuffer(&m_voxelCBData, m_buffers.voxelCBuffer);

	// We need to voxelize and construct the BDG before we can raymarch
	m_kernels.clearBuffers.Invoke();
	m_context->SubmitWork();  // we need to wait for the clear to finish

	m_kernels.voxelize.Invoke();
	m_kernels.constructBDG.Invoke();
}

void CD3D11IsosurfaceFluidRenderer::Raymarch() { m_kernels.raymarch.Invoke(); }

void CD3D11IsosurfaceFluidRenderer::Render() {
	THROW_IF_FALSY(m_context, "Renderer must be attached to a context");
	THROW_IF_FALSY(m_simData, "Renderer must be linked to simulation data");
	THROW_IF_FALSY(
		m_outputTextures.IsInitialized(),
		"Fluid textures must be initialized before rendering"
	);

#ifdef _DEBUG
	auto *device = static_cast<ID3D11Device *>(
		m_context->GetRenderAPIResource(RenderAPIResource::D3D11Device)
	);

	if (m_renderDoc != nullptr) {
		m_renderDoc->StartFrameCapture(device, nullptr);
	}
#endif

	ConstructMarchingBuffers();
	Raymarch();
	m_context->SubmitWork();

#ifdef _DEBUG
	if (m_renderDoc != nullptr) {
		m_renderDoc->EndFrameCapture(device, nullptr);
	}
#endif
}

void CD3D11IsosurfaceFluidRenderer::AttachToContext(
	GellyObserverPtr<IRenderContext> context
) {
	m_context = context;
}

void CD3D11IsosurfaceFluidRenderer::SetSimData(
	GellyObserverPtr<ISimData> simData
) {
	m_simData = simData;

	CreateShaders();
	CreateBuffers();
	CreateTextures();
	CreateKernels();

	m_voxelCBData.domainSize = {
		m_settings.special.isosurface.domainWidth,
		m_settings.special.isosurface.domainHeight,
		m_settings.special.isosurface.domainDepth
	};

	m_voxelCBData.voxelSize = m_settings.special.isosurface.voxelSize;
	m_voxelCBData.maxParticlesInVoxel =
		m_settings.special.isosurface.maxParticlesInVoxel;
	m_voxelCBData.maxParticles = m_simData->GetMaxParticles();
	util::UpdateCBuffer(&m_voxelCBData, m_buffers.voxelCBuffer);

	// link
	m_simData->LinkBuffer(
		SimBufferType::POSITION, m_buffers.positions->GetBufferResource()
	);
}

void CD3D11IsosurfaceFluidRenderer::SetSettings(
	const Gelly::FluidRenderSettings &settings
) {
	m_settings = settings;
}

GellyObserverPtr<IFluidTextures>
CD3D11IsosurfaceFluidRenderer::GetFluidTextures() {
	return &m_outputTextures;
}

void CD3D11IsosurfaceFluidRenderer::EnableLowBitMode() {
	// Nothing
	// TODO: Feature-gate this so it's not called unless the feature is
	// supported
}

void CD3D11IsosurfaceFluidRenderer::PullPerParticleData() {}
void CD3D11IsosurfaceFluidRenderer::PushPerParticleData() {}
void CD3D11IsosurfaceFluidRenderer::SetPerParticleAbsorption(
	uint particleIndex, const float absorption[3]
) {}

void CD3D11IsosurfaceFluidRenderer::SetPerFrameParams(
	const Gelly::FluidRenderParams &params
) {
	m_perFrameData = params;
	uint16_t width, height;
	m_context->GetDimensions(width, height);

	m_perFrameData.width = static_cast<float>(width);
	m_perFrameData.height = static_cast<float>(height);

	util::UpdateCBuffer(&m_perFrameData, m_buffers.fluidRenderCBuffer);
}

bool CD3D11IsosurfaceFluidRenderer::CheckFeatureSupport(GELLY_FEATURE feature) {
	switch (feature) {
		default:
			return false;
	}
}

#ifdef _DEBUG
bool CD3D11IsosurfaceFluidRenderer::EnableRenderDocCaptures() {
	const HMODULE renderDocModule = GetModuleHandle("renderdoc.dll");
	if (renderDocModule == nullptr) {
		return false;
	}

	const auto RENDERDOC_GetAPI = reinterpret_cast<pRENDERDOC_GetAPI>(
		GetProcAddress(renderDocModule, "RENDERDOC_GetAPI")
	);

	if (const auto ret = RENDERDOC_GetAPI(
			eRENDERDOC_API_Version_1_1_2,
			reinterpret_cast<void **>(&m_renderDoc)
		);
		ret != 1) {
		return false;
	}

	return true;
}
#endif