#include "fluidrender/CD3D11DebugFluidRenderer.h"

#include <d3d11.h>

#include <stdexcept>

#include "EncodeDepthPS.h"
#include "EstimateNormalPS.h"
#include "FilterDepthPS.h"
#include "FilterThicknessPS.h"
#include "ScreenQuadVS.h"
#include "SplattingGS.h"
#include "SplattingPS.h"
#include "SplattingVS.h"
#include "ThicknessGS.h"
#include "ThicknessPS.h"
#include "ThicknessVS.h"
#include "fluidrender/util/CBuffers.h"
#include "fluidrender/util/ScreenQuadVB.h"

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

#ifdef _DEBUG
#include <windows.h>
#endif

CD3D11DebugFluidRenderer::CD3D11DebugFluidRenderer()
	: context(nullptr), simData(nullptr), buffers({}), views({}) {}

void CD3D11DebugFluidRenderer::CreateShaders() {
	shaders.splattingGS = context->CreateShader(
		gsc::SplattingGS::GetBytecode(),
		gsc::SplattingGS::GetBytecodeSize(),
		ShaderType::Geometry
	);

	shaders.splattingPS = context->CreateShader(
		gsc::SplattingPS::GetBytecode(),
		gsc::SplattingPS::GetBytecodeSize(),
		ShaderType::Pixel
	);

	shaders.splattingVS = context->CreateShader(
		gsc::SplattingVS::GetBytecode(),
		gsc::SplattingVS::GetBytecodeSize(),
		ShaderType::Vertex
	);

	shaders.thicknessGS = context->CreateShader(
		gsc::ThicknessGS::GetBytecode(),
		gsc::ThicknessGS::GetBytecodeSize(),
		ShaderType::Geometry
	);

	shaders.thicknessPS = context->CreateShader(
		gsc::ThicknessPS::GetBytecode(),
		gsc::ThicknessPS::GetBytecodeSize(),
		ShaderType::Pixel
	);

	shaders.thicknessVS = context->CreateShader(
		gsc::ThicknessVS::GetBytecode(),
		gsc::ThicknessVS::GetBytecodeSize(),
		ShaderType::Vertex
	);

	shaders.screenQuadVS = context->CreateShader(
		gsc::ScreenQuadVS::GetBytecode(),
		gsc::ScreenQuadVS::GetBytecodeSize(),
		ShaderType::Vertex
	);

	shaders.filterDepthPS = context->CreateShader(
		gsc::FilterDepthPS::GetBytecode(),
		gsc::FilterDepthPS::GetBytecodeSize(),
		ShaderType::Pixel
	);

	shaders.estimateNormalPS = context->CreateShader(
		gsc::EstimateNormalPS::GetBytecode(),
		gsc::EstimateNormalPS::GetBytecodeSize(),
		ShaderType::Pixel
	);

	shaders.filterThicknessPS = context->CreateShader(
		gsc::FilterThicknessPS::GetBytecode(),
		gsc::FilterThicknessPS::GetBytecodeSize(),
		ShaderType::Pixel
	);

	shaders.encodeDepthPS = context->CreateShader(
		gsc::EncodeDepthPS::GetBytecode(),
		gsc::EncodeDepthPS::GetBytecodeSize(),
		ShaderType::Pixel
	);
}

void CD3D11DebugFluidRenderer::CreateBuffers() {
	if (!shaders.splattingVS) {
		throw std::logic_error(
			"CD3D11DebugFluidRenderer::CreateBuffers: shaders.splattingVS is "
			"null."
		);
	}

	if (maxParticles <= 0) {
		throw std::logic_error(
			"CD3D11DebugFluidRenderer::CreateBuffers: maxParticles is not set."
		);
	}

	BufferDesc positionBufferDesc = {};
	positionBufferDesc.type = BufferType::VERTEX;
	positionBufferDesc.usage = BufferUsage::DEFAULT;
	positionBufferDesc.format = BufferFormat::R32G32B32A32_FLOAT;
	positionBufferDesc.byteWidth =
		sizeof(SimFloat4) * simData->GetMaxParticles();
	positionBufferDesc.stride = sizeof(SimFloat4);

	buffers.positions = context->CreateBuffer(positionBufferDesc);
	buffers.fluidRenderCBuffer =
		util::CreateCBuffer<FluidRenderParams>(context);

	BufferLayoutDesc positionLayoutDesc = {};
	positionLayoutDesc.items[0] = {
		0, "SV_POSITION", 0, BufferLayoutFormat::FLOAT4
	};
	positionLayoutDesc.itemCount = 1;
	positionLayoutDesc.vertexShader = shaders.splattingVS;
	positionLayoutDesc.topology = BufferLayoutTopology::POINTS;

	buffers.positionsLayout = context->CreateBufferLayout(positionLayoutDesc);
	buffers.positionsLayout->AttachBufferAtSlot(buffers.positions, 0);

	BufferDesc particleAbsorptionBufferDesc = {};
	particleAbsorptionBufferDesc.type = BufferType::SHADER_RESOURCE;
	particleAbsorptionBufferDesc.usage = BufferUsage::DYNAMIC;
	// Any higher and it's overkill, any lower and we get artifacts.
	particleAbsorptionBufferDesc.format = BufferFormat::R32G32B32A32_FLOAT;
	particleAbsorptionBufferDesc.byteWidth =
		sizeof(SimFloat4) * simData->GetMaxParticles();
	particleAbsorptionBufferDesc.stride = sizeof(SimFloat4);

	buffers.particleAbsorption =
		context->CreateBuffer(particleAbsorptionBufferDesc);

	DepthBufferDesc depthBufferDesc = {};
	depthBufferDesc.format = DepthFormat::D24S8;
	depthBufferDesc.depthOp = DepthOp::LESS_EQUAL;

	buffers.depthBuffer = context->CreateDepthBuffer(depthBufferDesc);

	std::tie(buffers.screenQuad, buffers.screenQuadLayout) =
		util::GenerateScreenQuad(context, shaders.screenQuadVS);
}

void CD3D11DebugFluidRenderer::CreateTextures() {
	uint16_t width = 0, height = 0;
	context->GetDimensions(width, height);

	const auto quarterWidth = static_cast<uint16_t>(width / 4);
	const auto quarterHeight = static_cast<uint16_t>(height / 4);

	TextureDesc unfilteredDepthDesc = {};
	unfilteredDepthDesc.width = width;
	unfilteredDepthDesc.height = height;
	unfilteredDepthDesc.access = TextureAccess::READ | TextureAccess::WRITE;
	unfilteredDepthDesc.format = TextureFormat::R32G32_FLOAT;

	internalTextures.unfilteredDepth = context->CreateTexture(
		"splatrenderer/unfilteredDepth", unfilteredDepthDesc
	);

	TextureDesc untransformedDepthDesc = {};
	untransformedDepthDesc.width = width;
	untransformedDepthDesc.height = height;
	untransformedDepthDesc.access = TextureAccess::READ | TextureAccess::WRITE;
	untransformedDepthDesc.format = TextureFormat::R32G32_FLOAT;

	internalTextures.untransformedDepth = context->CreateTexture(
		"splatrenderer/untransformedDepth", untransformedDepthDesc
	);

	TextureDesc unfilteredThicknessDesc = {};
	unfilteredThicknessDesc.filter = TextureFilter::LINEAR;	 // hides pixels
	unfilteredThicknessDesc.width = quarterWidth;
	unfilteredThicknessDesc.height = quarterHeight;
	unfilteredThicknessDesc.access = TextureAccess::READ | TextureAccess::WRITE;
	unfilteredThicknessDesc.format = TextureFormat::R16G16B16A16_FLOAT;

	internalTextures.unfilteredThickness = context->CreateTexture(
		"splatrenderer/unfilteredThickness", unfilteredThicknessDesc
	);

	TextureDesc unfilteredAlbedoDesc = {};
	unfilteredAlbedoDesc.filter = TextureFilter::LINEAR;  // hides pixels
	unfilteredAlbedoDesc.width = width;
	unfilteredAlbedoDesc.height = height;
	unfilteredAlbedoDesc.access = TextureAccess::READ | TextureAccess::WRITE;
	unfilteredAlbedoDesc.format = TextureFormat::R32G32B32A32_FLOAT;

	internalTextures.unfilteredAlbedo = context->CreateTexture(
		"splatrenderer/unfilteredAlbedo", unfilteredAlbedoDesc
	);

	constexpr float clearColor[4] = {1.0f, 1.0f, 0.0f, 1.0f};
	internalTextures.unfilteredDepth->Clear(clearColor);
	internalTextures.unfilteredThickness->Clear(clearColor);
}

void CD3D11DebugFluidRenderer::SetSimData(GellyObserverPtr<ISimData> simData) {
	if (simData == nullptr) {
		throw std::invalid_argument(
			"CD3D11DebugFluidRenderer::SetSimData: simData cannot be null."
		);
	}

	if (simData->GetAPI() != SimContextAPI::D3D11) {
		throw std::logic_error(
			"CD3D11DebugFluidRenderer::SetSimData: simData must be backed "
			"by "
			"D3D11"
		);
	}

	this->simData = simData;
	maxParticles = simData->GetMaxParticles();

	CreateBuffers();
	simData->LinkBuffer(
		SimBufferType::POSITION, buffers.positions->GetBufferResource()
	);
}

void CD3D11DebugFluidRenderer::AttachToContext(
	GellyObserverPtr<IRenderContext> context
) {
	if (context == nullptr) {
		throw std::invalid_argument(
			"CD3D11DebugFluidRenderer::AttachToContext: context cannot be "
			"null."
		);
	}

	if (context->GetRenderAPI() != ContextRenderAPI::D3D11) {
		throw std::logic_error(
			"CD3D11DebugFluidRenderer::AttachToContext: context must be "
			"backed "
			"by D3D11"
		);
	}

	this->context = context;
	CreateTextures();
	CreateShaders();
	uint16_t width = 0, height = 0;
	context->GetDimensions(width, height);

	cbufferData.width = static_cast<float>(width);
	cbufferData.height = static_cast<float>(height);
}

GellyObserverPtr<IFluidTextures> CD3D11DebugFluidRenderer::GetFluidTextures() {
	return &outputTextures;
}

constexpr float depthClearColor[4] = {1.0f, 1.0f, 0.0f, 0.0f};
constexpr float genericClearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};

void CD3D11DebugFluidRenderer::RenderUnfilteredDepth() {
#ifdef TRACY_ENABLE
	ZoneScopedN("Unfiltered depth render");
#endif
	buffers.depthBuffer->Clear(1.0f);
	auto *depthTexture = internalTextures.unfilteredDepth;
	depthTexture->Clear(depthClearColor);

	depthTexture->BindToPipeline(
		TextureBindStage::RENDER_TARGET_OUTPUT, 0, std::nullopt
	);

	buffers.positionsLayout->BindAsVertexBuffer();

	shaders.splattingGS->Bind();
	shaders.splattingPS->Bind();
	shaders.splattingVS->Bind();

	buffers.fluidRenderCBuffer->BindToPipeline(ShaderType::Pixel, 0);
	buffers.fluidRenderCBuffer->BindToPipeline(ShaderType::Vertex, 0);
	buffers.fluidRenderCBuffer->BindToPipeline(ShaderType::Geometry, 0);

	context->Draw(simData->GetActiveParticles(), 0);
	// we're not using a swapchain, so we need to queue up work manually
	context->ResetPipeline();
}

void CD3D11DebugFluidRenderer::RenderFilteredDepth() {
#ifdef TRACY_ENABLE
	ZoneScopedN("Filtered depth render");
#endif
	auto *depthTextureA = internalTextures.unfilteredDepth;
	auto *depthTextureB = lowBitMode ? internalTextures.untransformedDepth
									 : outputTextures.GetFeatureTexture(DEPTH);

	depthTextureB->Clear(depthClearColor);

	for (int i = 0; i < settings.filterIterations; i++) {
#ifdef TRACY_ENABLE
		ZoneScopedN("Depth filter iteration");
#endif
		// We flip between the two textures to avoid having to copy the
		// filtered depth back to the unfiltered depth texture.
		shaders.screenQuadVS->Bind();
		shaders.filterDepthPS->Bind();

		buffers.fluidRenderCBuffer->BindToPipeline(ShaderType::Pixel, 0);
		buffers.screenQuadLayout->BindAsVertexBuffer();

		depthTextureB->BindToPipeline(
			TextureBindStage::RENDER_TARGET_OUTPUT, 0, std::nullopt
		);

		depthTextureA->BindToPipeline(
			TextureBindStage::PIXEL_SHADER_READ, 0, std::nullopt
		);

		context->Draw(4, 0);
		context->ResetPipeline();

		// Swap the textures.
		std::swap(depthTextureA, depthTextureB);
	}

	if (settings.filterIterations == 0) {
		// Just copy the unfiltered depth to the filtered depth without any
		// filtering.
		auto *depthTexture =
			outputTextures.GetFeatureTexture(FluidFeatureType::DEPTH);

		internalTextures.unfilteredDepth->CopyToTexture(depthTexture);
		context->SubmitWork();
		context->ResetPipeline();
	}
}

void CD3D11DebugFluidRenderer::RenderNormals() {
#ifdef TRACY_ENABLE
	ZoneScopedN("Normal estimation render");
#endif
	auto *normalsTexture =
		outputTextures.GetFeatureTexture(FluidFeatureType::NORMALS);

	auto *positionsTexture =
		outputTextures.GetFeatureTexture(FluidFeatureType::POSITIONS);

	auto *depthTexture = internalTextures.unfilteredDepth;

	normalsTexture->Clear(genericClearColor);
	positionsTexture->Clear(genericClearColor);

	IManagedTexture *renderTargets[] = {normalsTexture, positionsTexture};
	context->BindMultipleTexturesAsOutput(
		renderTargets, ARRAYSIZE(renderTargets), std::nullopt
	);

	depthTexture->BindToPipeline(
		TextureBindStage::PIXEL_SHADER_READ, 0, std::nullopt
	);

	shaders.screenQuadVS->Bind();
	shaders.estimateNormalPS->Bind();

	buffers.fluidRenderCBuffer->BindToPipeline(ShaderType::Pixel, 0);
	buffers.screenQuadLayout->BindAsVertexBuffer();

	context->Draw(4, 0);
	context->ResetPipeline();
}

void CD3D11DebugFluidRenderer::RenderThickness() {
#ifdef TRACY_ENABLE
	ZoneScopedN("Thickness render");
#endif
	auto *thicknessTexture = internalTextures.unfilteredThickness;
	auto *albedoTexture = internalTextures.unfilteredAlbedo;

	thicknessTexture->Clear(genericClearColor);
	// No depth buffer needed for this pass.
	IManagedTexture *renderTargets[] = {thicknessTexture, albedoTexture};
	context->BindMultipleTexturesAsOutput(
		renderTargets, ARRAYSIZE(renderTargets), std::nullopt
	);

	buffers.positionsLayout->BindAsVertexBuffer();

	shaders.thicknessGS->Bind();
	shaders.thicknessPS->Bind();
	shaders.thicknessVS->Bind();

	buffers.fluidRenderCBuffer->BindToPipeline(ShaderType::Pixel, 0);
	buffers.fluidRenderCBuffer->BindToPipeline(ShaderType::Vertex, 0);
	buffers.fluidRenderCBuffer->BindToPipeline(ShaderType::Geometry, 0);

	buffers.particleAbsorption->BindToPipeline(ShaderType::Vertex, 0);

	context->UseTextureResForNextDraw(thicknessTexture);
	context->Draw(simData->GetActiveParticles(), 0, true);
	// we're not using a swapchain, so we need to queue up work manually
	context->ResetPipeline();
}

void CD3D11DebugFluidRenderer::RenderGenericBlur(
	GellyInterfaceVal<IManagedTexture> texA,
	GellyInterfaceVal<IManagedTexture> texB
) {
#ifdef TRACY_ENABLE
	ZoneScopedN("Generic blur render");
#endif
	auto *textureA = texA;
	auto *textureB = texB;

	for (int i = 0; i < settings.thicknessFilterIterations; i++) {
#ifdef TRACY_ENABLE
		ZoneScopedN("Generic blur filter iteration");
#endif
		textureB->Clear(genericClearColor);
		textureB->BindToPipeline(
			TextureBindStage::RENDER_TARGET_OUTPUT, 0, std::nullopt
		);

		textureA->BindToPipeline(
			TextureBindStage::PIXEL_SHADER_READ, 0, std::nullopt
		);

		shaders.screenQuadVS->Bind();
		shaders.filterThicknessPS->Bind();

		buffers.fluidRenderCBuffer->BindToPipeline(ShaderType::Pixel, 0);
		buffers.screenQuadLayout->BindAsVertexBuffer();

		// sometimes the thickness is rendered at quarter res, so we need to
		// account for that

		context->UseTextureResForNextDraw(textureB);
		context->Draw(4, 0);
		context->ResetPipeline();

		// Swap
		std::swap(textureA, textureB);
	}
}

void CD3D11DebugFluidRenderer::EncodeDepth() {
#ifdef TRACY_ENABLE
	ZoneScopedN("Depth encoding render");
#endif
	auto *depthTexture = outputTextures.GetFeatureTexture(DEPTH);
	auto *untransformedDepthTexture = internalTextures.untransformedDepth;

	depthTexture->Clear(depthClearColor);

	depthTexture->BindToPipeline(
		TextureBindStage::RENDER_TARGET_OUTPUT, 0, std::nullopt
	);

	untransformedDepthTexture->BindToPipeline(
		TextureBindStage::PIXEL_SHADER_READ, 0, std::nullopt
	);

	buffers.screenQuadLayout->BindAsVertexBuffer();

	buffers.fluidRenderCBuffer->BindToPipeline(ShaderType::Pixel, 0);
	buffers.fluidRenderCBuffer->BindToPipeline(ShaderType::Vertex, 0);

	shaders.screenQuadVS->Bind();
	shaders.encodeDepthPS->Bind();

	context->Draw(4, 0);
	context->ResetPipeline();
}

void CD3D11DebugFluidRenderer::Render() {
#ifdef TRACY_ENABLE
	ZoneScopedN("Fluid render");
#endif
	if (context == nullptr) {
		throw std::logic_error(
			"CD3D11DebugFluidRenderer::Render: context is null."
		);
	}

	if (simData == nullptr) {
		throw std::logic_error(
			"CD3D11DebugFluidRenderer::Render: simData is null."
		);
	}

	if (simData->GetActiveParticles() <= 0) {
		return;
	}

	if (!outputTextures.IsInitialized()) {
		throw std::logic_error(
			"CD3D11DebugFluidRenderer::Render: outputTextures is not "
			"initialized."
		);
	}

	context->SetRasterizerFlags(RasterizerFlags::DISABLE_CULL);

#ifdef _DEBUG
	auto *device = static_cast<ID3D11Device *>(
		context->GetRenderAPIResource(RenderAPIResource::D3D11Device)
	);

	if (renderDocApi != nullptr) {
		renderDocApi->StartFrameCapture(device, nullptr);
	}
#endif

	RenderUnfilteredDepth();
	RenderFilteredDepth();
	RenderNormals();
	RenderThickness();
	RenderGenericBlur(
		internalTextures.unfilteredThickness,
		outputTextures.GetFeatureTexture(FluidFeatureType::THICKNESS)
	);
	RenderGenericBlur(
		internalTextures.unfilteredAlbedo,
		outputTextures.GetFeatureTexture(FluidFeatureType::ALBEDO)
	);

	context->SubmitWork();

	if (lowBitMode) {
		EncodeDepth();
	}

#ifdef _DEBUG
	if (renderDocApi != nullptr) {
		renderDocApi->EndFrameCapture(device, nullptr);
	}
#endif
}

void CD3D11DebugFluidRenderer::EnableLowBitMode() { lowBitMode = true; }

void CD3D11DebugFluidRenderer::SetSettings(
	const Gelly::FluidRenderSettings &settings
) {
	this->settings = settings;
}

void CD3D11DebugFluidRenderer::SetPerFrameParams(
	const Gelly::FluidRenderParams &params
) {
	// Copy it, but overwrite the automatically filled out members.
	cbufferData = params;
	uint16_t width = 0, height = 0;
	context->GetDimensions(width, height);

	// TODO: Can't the shader just use a uint instead of a float?
	cbufferData.width = static_cast<float>(width);
	cbufferData.height = static_cast<float>(height);

	util::UpdateCBuffer(&cbufferData, buffers.fluidRenderCBuffer);
}

void CD3D11DebugFluidRenderer::PullPerParticleData() {
	views.absorptionView =
		context->CreateMappedBufferView(buffers.particleAbsorption);
}

void CD3D11DebugFluidRenderer::SetPerParticleAbsorption(
	const uint particleIndex, const float absorption[3]
) {
	views.absorptionView->Write(
		particleIndex,
		SimFloat4{absorption[0], absorption[1], absorption[2], 0.0}
	);
}

void CD3D11DebugFluidRenderer::PushPerParticleData() {
	views.absorptionView.reset();
}

bool CD3D11DebugFluidRenderer::CheckFeatureSupport(GELLY_FEATURE feature) {
	switch (feature) {
		case GELLY_FEATURE::FLUIDRENDER_PER_PARTICLE_ABSORPTION:
			return true;
		default:
			return false;
	}
}

#ifdef _DEBUG
bool CD3D11DebugFluidRenderer::EnableRenderDocCaptures() {
	const HMODULE renderDocModule = GetModuleHandle("renderdoc.dll");
	if (renderDocModule == nullptr) {
		return false;
	}

	const auto RENDERDOC_GetAPI = reinterpret_cast<pRENDERDOC_GetAPI>(
		GetProcAddress(renderDocModule, "RENDERDOC_GetAPI")
	);

	if (const auto ret = RENDERDOC_GetAPI(
			eRENDERDOC_API_Version_1_1_2,
			reinterpret_cast<void **>(&renderDocApi)
		);
		ret != 1) {
		return false;
	}

	return true;
}
#endif