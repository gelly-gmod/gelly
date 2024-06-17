#include <d3d11.h>

#include <stdexcept>

#include "EncodeDepthPS.h"
#include "EstimateNormalPS.h"
#include "FilterDepthPS.h"
#include "FilterThicknessPS.h"
#include "FoamGS.h"
#include "FoamPS.h"
#include "FoamVS.h"
#include "ScreenQuadVS.h"
#include "SplattingGS.h"
#include "SplattingPS.h"
#include "SplattingVS.h"
#include "ThicknessGS.h"
#include "ThicknessPS.h"
#include "ThicknessVS.h"
#include "fluidrender/splatting/CD3D11SplattingFluidRenderer.h"
#include "fluidrender/util/CBuffers.h"
#include "fluidrender/util/ScreenQuadVB.h"

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

#ifdef GELLY_ENABLE_RENDERDOC_CAPTURES
#include <windows.h>
#endif

CD3D11SplattingFluidRenderer::CD3D11SplattingFluidRenderer()
	: context(nullptr), simData(nullptr), buffers({}), views({}) {}

void CD3D11SplattingFluidRenderer::CreateShaders() {
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

	shaders.foamGS = context->CreateShader(
		gsc::FoamGS::GetBytecode(),
		gsc::FoamGS::GetBytecodeSize(),
		ShaderType::Geometry
	);

	shaders.foamPS = context->CreateShader(
		gsc::FoamPS::GetBytecode(),
		gsc::FoamPS::GetBytecodeSize(),
		ShaderType::Pixel
	);

	shaders.foamVS = context->CreateShader(
		gsc::FoamVS::GetBytecode(),
		gsc::FoamVS::GetBytecodeSize(),
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

void CD3D11SplattingFluidRenderer::CreateBuffers() {
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

	BufferDesc foamPositionBufferDesc = {};
	foamPositionBufferDesc.type = BufferType::VERTEX;
	foamPositionBufferDesc.usage = BufferUsage::DEFAULT;
	foamPositionBufferDesc.format = BufferFormat::R32G32B32A32_FLOAT;
	foamPositionBufferDesc.byteWidth =
		sizeof(SimFloat4) * simData->GetMaxFoamParticles();
	foamPositionBufferDesc.stride = sizeof(SimFloat4);

	buffers.foamPositions = context->CreateBuffer(foamPositionBufferDesc);

	BufferDesc foamVelocityBufferDesc = foamPositionBufferDesc;
	buffers.foamVelocities = context->CreateBuffer(foamVelocityBufferDesc);

	BufferDesc anisotropyDesc = {};
	anisotropyDesc.type = BufferType::VERTEX;
	anisotropyDesc.usage = BufferUsage::DEFAULT;
	anisotropyDesc.format = BufferFormat::R32G32B32A32_FLOAT;
	anisotropyDesc.byteWidth = sizeof(SimFloat4) * simData->GetMaxParticles();
	anisotropyDesc.stride = sizeof(SimFloat4);

	buffers.anisotropyQ1 = context->CreateBuffer(anisotropyDesc);
	buffers.anisotropyQ2 = context->CreateBuffer(anisotropyDesc);
	buffers.anisotropyQ3 = context->CreateBuffer(anisotropyDesc);

	buffers.fluidRenderCBuffer =
		util::CreateCBuffer<FluidRenderParams>(context);

	BufferLayoutDesc splattingLayoutDesc = {};
	splattingLayoutDesc.items[0] = {
		0,
		"SV_POSITION",
		0,
		BufferLayoutFormat::FLOAT4,
	};

	splattingLayoutDesc.items[1] = {
		0, "ANISOTROPY", 1, BufferLayoutFormat::FLOAT4
	};
	splattingLayoutDesc.items[2] = {
		1, "ANISOTROPY", 2, BufferLayoutFormat::FLOAT4
	};
	splattingLayoutDesc.items[3] = {
		2, "ANISOTROPY", 3, BufferLayoutFormat::FLOAT4
	};

	splattingLayoutDesc.itemCount = 4;
	splattingLayoutDesc.vertexShader = shaders.splattingVS;
	splattingLayoutDesc.topology = BufferLayoutTopology::POINTS;

	buffers.splattingLayout = context->CreateBufferLayout(splattingLayoutDesc);
	buffers.splattingLayout->AttachBufferAtSlot(buffers.positions, 0);
	buffers.splattingLayout->AttachBufferAtSlot(buffers.anisotropyQ1, 1);
	buffers.splattingLayout->AttachBufferAtSlot(buffers.anisotropyQ2, 2);
	buffers.splattingLayout->AttachBufferAtSlot(buffers.anisotropyQ3, 3);

	BufferLayoutDesc foamLayoutDesc = {};
	foamLayoutDesc.items[0] = {
		0,
		"SV_POSITION",
		0,
		BufferLayoutFormat::FLOAT4,
	};

	foamLayoutDesc.items[1] = {0, "VELOCITY", 1, BufferLayoutFormat::FLOAT4};
	foamLayoutDesc.itemCount = 2;
	foamLayoutDesc.vertexShader = shaders.foamVS;
	foamLayoutDesc.topology = BufferLayoutTopology::POINTS;

	buffers.foamLayout = context->CreateBufferLayout(foamLayoutDesc);
	buffers.foamLayout->AttachBufferAtSlot(buffers.foamPositions, 0);
	buffers.foamLayout->AttachBufferAtSlot(buffers.foamVelocities, 1);

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

	DepthBufferDesc foamDepthBufferDesc = depthBufferDesc;

	buffers.foamDepthBuffer = context->CreateDepthBuffer(foamDepthBufferDesc);

	std::tie(buffers.screenQuad, buffers.screenQuadLayout) =
		util::GenerateScreenQuad(context, shaders.screenQuadVS);
}

void CD3D11SplattingFluidRenderer::CreateTextures() {
	uint16_t width = 0, height = 0;
	context->GetDimensions(width, height);

	const auto quarterWidth = static_cast<uint16_t>(width / 4);
	const auto quarterHeight = static_cast<uint16_t>(height / 4);

	TextureDesc unfilteredDepthDesc = {};
	unfilteredDepthDesc.width = width;
	unfilteredDepthDesc.height = height;
	unfilteredDepthDesc.access = TextureAccess::READ | TextureAccess::WRITE;
	unfilteredDepthDesc.format = TextureFormat::R32G32_FLOAT;
	unfilteredDepthDesc.filter = TextureFilter::LINEAR;
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
	unfilteredThicknessDesc.width = width;
	unfilteredThicknessDesc.height = height;
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

#ifndef PRODUCTION_BUILD
	TextureDesc aovDesc = {};
	aovDesc.filter =
		TextureFilter::LINEAR;	// doesn't really matter, they're write-only
	aovDesc.width = width;
	aovDesc.height = height;
	aovDesc.access = TextureAccess::READ | TextureAccess::WRITE;
	aovDesc.format = TextureFormat::R32G32B32A32_FLOAT;

	for (int aovIndex = 0; aovIndex < randomAccessAOVs.size(); aovIndex++) {
		std::string name = "splatrenderer/aov" + std::to_string(aovIndex);
		randomAccessAOVs[aovIndex] =
			context->CreateTexture(name.c_str(), aovDesc);
	}
#endif
}

void CD3D11SplattingFluidRenderer::SetSimData(GellyObserverPtr<ISimData> simData
) {
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
	simData->LinkBuffer(
		SimBufferType::FOAM_POSITION, buffers.foamPositions->GetBufferResource()
	);
	simData->LinkBuffer(
		SimBufferType::FOAM_VELOCITY,
		buffers.foamVelocities->GetBufferResource()
	);
	simData->LinkBuffer(
		SimBufferType::ANISOTROPY_Q1, buffers.anisotropyQ1->GetBufferResource()
	);
	simData->LinkBuffer(
		SimBufferType::ANISOTROPY_Q2, buffers.anisotropyQ2->GetBufferResource()
	);
	simData->LinkBuffer(
		SimBufferType::ANISOTROPY_Q3, buffers.anisotropyQ3->GetBufferResource()
	);
}

void CD3D11SplattingFluidRenderer::AttachToContext(
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

	perfMarker = context->CreatePerfMarker();
}

GellyObserverPtr<IFluidTextures> CD3D11SplattingFluidRenderer::GetFluidTextures(
) {
	return &outputTextures;
}

constexpr float depthClearColor[4] = {1.0f, 1.0f, 0.0f, 0.0f};
constexpr float genericClearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};

void CD3D11SplattingFluidRenderer::RenderUnfilteredDepth() {
#ifdef TRACY_ENABLE
	ZoneScopedN("Unfiltered depth render");
#endif
	perfMarker->BeginEvent("Splatting ellipsoid depth");
	buffers.depthBuffer->Clear(1.0f);
	internalTextures.unfilteredThickness->Clear(genericClearColor);
	internalTextures.unfilteredAlbedo->Clear(genericClearColor);

	auto *depthTexture = internalTextures.unfilteredDepth;
	depthTexture->Clear(depthClearColor);

	IManagedTexture *renderTargets[] = {
		depthTexture, internalTextures.unfilteredAlbedo
	};

	IManagedTexture *uavs[] = {internalTextures.unfilteredThickness};

	context->BindMRTAndUAVs(
		renderTargets,
		ARRAYSIZE(renderTargets),
		uavs,
		ARRAYSIZE(uavs),
		2,
		buffers.depthBuffer
	);

	buffers.splattingLayout->BindAsVertexBuffer();

	shaders.splattingGS->Bind();
	shaders.splattingPS->Bind();
	shaders.splattingVS->Bind();

	buffers.fluidRenderCBuffer->BindToPipeline(ShaderType::Pixel, 0);
	buffers.fluidRenderCBuffer->BindToPipeline(ShaderType::Vertex, 0);
	buffers.fluidRenderCBuffer->BindToPipeline(ShaderType::Geometry, 0);

	buffers.particleAbsorption->BindToPipeline(ShaderType::Vertex, 0);

	context->Draw(simData->GetActiveParticles(), 0);
	// we're not using a swapchain, so we need to queue up work manually
	context->ResetPipeline();
	perfMarker->EndEvent();
}

void CD3D11SplattingFluidRenderer::RenderFilteredDepth() {
#ifdef TRACY_ENABLE
	ZoneScopedN("Filtered depth render");
#endif
	auto *depthTextureA = internalTextures.unfilteredDepth;
	auto *depthTextureB = lowBitMode ? internalTextures.untransformedDepth
									 : outputTextures.GetFeatureTexture(DEPTH);

	depthTextureB->Clear(depthClearColor);

#ifndef PRODUCTION_BUILD
	randomAccessAOVs[0]->Clear(genericClearColor);
#endif

	perfMarker->BeginEvent("Filtering depth");
	for (int i = 0; i < settings.filterIterations; i++) {
		perfMarker->BeginEvent("Depth filter iteration");
#ifdef TRACY_ENABLE
		ZoneScopedN("Depth filter iteration");
#endif
		// We flip between the two textures to avoid having to copy the
		// filtered depth back to the unfiltered depth texture.
		shaders.screenQuadVS->Bind();
		shaders.filterDepthPS->Bind();

		buffers.fluidRenderCBuffer->BindToPipeline(ShaderType::Pixel, 0);
		buffers.screenQuadLayout->BindAsVertexBuffer();

#ifdef PRODUCTION_BUILD
		depthTextureB->BindToPipeline(
			TextureBindStage::RENDER_TARGET_OUTPUT, 0, std::nullopt
		);
#else
		GellyInterfaceVal<IManagedTexture> renderTargets[] = {
			depthTextureB, randomAccessAOVs[0]
		};

		context->BindMultipleTexturesAsOutput(
			renderTargets, ARRAYSIZE(renderTargets), std::nullopt
		);
#endif

		depthTextureA->BindToPipeline(
			TextureBindStage::PIXEL_SHADER_READ, 0, std::nullopt
		);

		context->Draw(4, 0);
		context->ResetPipeline();

		// Swap the textures.
		std::swap(depthTextureA, depthTextureB);
		perfMarker->EndEvent();
	}

	if (settings.filterIterations == 0) {
		perfMarker->BeginEvent("Depth copy (dummy filter)");
		// Just copy the unfiltered depth to the filtered depth without any
		// filtering.
		auto *depthTexture =
			outputTextures.GetFeatureTexture(FluidFeatureType::DEPTH);

		internalTextures.unfilteredDepth->CopyToTexture(depthTexture);
		context->SubmitWork();
		context->ResetPipeline();
		perfMarker->EndEvent();
	}
	perfMarker->EndEvent();
}

void CD3D11SplattingFluidRenderer::RenderNormals() {
#ifdef TRACY_ENABLE
	ZoneScopedN("Normal estimation render");
#endif
	perfMarker->BeginEvent("Estimating normals");
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
	perfMarker->EndEvent();
}

void CD3D11SplattingFluidRenderer::RenderThickness() {
#ifdef TRACY_ENABLE
	ZoneScopedN("Thickness render");
#endif
	perfMarker->BeginEvent("Splatting isotropic thickness");
	auto *thicknessTexture = internalTextures.unfilteredThickness;
	auto *albedoTexture = internalTextures.unfilteredAlbedo;

	thicknessTexture->Clear(genericClearColor);
	albedoTexture->Clear(genericClearColor);

	// No depth buffer needed for this pass.
	IManagedTexture *renderTargets[] = {thicknessTexture, albedoTexture};
	context->BindMultipleTexturesAsOutput(
		renderTargets, ARRAYSIZE(renderTargets), std::nullopt
	);

	buffers.splattingLayout->BindAsVertexBuffer();

	shaders.thicknessGS->Bind();
	shaders.thicknessPS->Bind();
	shaders.thicknessVS->Bind();

	buffers.fluidRenderCBuffer->BindToPipeline(ShaderType::Pixel, 0);
	buffers.fluidRenderCBuffer->BindToPipeline(ShaderType::Vertex, 0);
	buffers.fluidRenderCBuffer->BindToPipeline(ShaderType::Geometry, 0);

	buffers.particleAbsorption->BindToPipeline(ShaderType::Vertex, 0);

	context->UseTextureResForNextDraw(thicknessTexture);
	context->Draw(simData->GetActiveParticles(), 0, AccumulateType::CLASSIC);
	// we're not using a swapchain, so we need to queue up work manually
	context->ResetPipeline();
	perfMarker->EndEvent();
}

void CD3D11SplattingFluidRenderer::RenderFoam(bool depthOnly) {
	std::string eventName = "Rendering foam";
	if (depthOnly) {
		eventName += " (depth only)";
	}

	perfMarker->BeginEvent(eventName.c_str());
	auto *foamTexture =
		outputTextures.GetFeatureTexture(FluidFeatureType::FOAM);

	// We need to output non-accumulated depth, and accumulated thickness--so we
	// split it into 2 passes.
	if (!depthOnly) {
		foamTexture->Clear(depthClearColor);
	} else {
		buffers.foamDepthBuffer->Clear(1.0f);
	}

	foamTexture->BindToPipeline(
		TextureBindStage::RENDER_TARGET_OUTPUT,
		0,
		depthOnly ? std::optional(buffers.foamDepthBuffer) : std::nullopt
	);

	buffers.foamLayout->BindAsVertexBuffer();

	shaders.foamVS->Bind();
	shaders.foamGS->Bind();
	shaders.foamPS->Bind();

	buffers.fluidRenderCBuffer->BindToPipeline(ShaderType::Pixel, 0);
	buffers.fluidRenderCBuffer->BindToPipeline(ShaderType::Vertex, 0);
	buffers.fluidRenderCBuffer->BindToPipeline(ShaderType::Geometry, 0);

	// We don't accumulate depth, only the foam.
	context->Draw(
		simData->GetActiveFoamParticles(),
		0,
		depthOnly ? AccumulateType::DEPTH_G_ONLY
				  : AccumulateType::ALPHA_ACCUMULATE
	);
	context->ResetPipeline();
	perfMarker->EndEvent();
}

void CD3D11SplattingFluidRenderer::RenderGenericBlur(
	GellyInterfaceVal<IManagedTexture> texA,
	GellyInterfaceVal<IManagedTexture> texB
) {
#ifdef TRACY_ENABLE
	ZoneScopedN("Generic blur render");
#endif
	perfMarker->BeginEvent("Generic blur filter");
	auto *textureA = texA;
	auto *textureB = texB;

	for (int i = 0; i < 1; i++) {
		perfMarker->BeginEvent("Generic blur filter iteration");
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
		perfMarker->EndEvent();
	}
	perfMarker->EndEvent();
}

void CD3D11SplattingFluidRenderer::EncodeDepth() {
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

void CD3D11SplattingFluidRenderer::Render() {
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

	if (!outputTextures.IsInitialized()) {
		throw std::logic_error(
			"CD3D11DebugFluidRenderer::Render: outputTextures is not "
			"initialized."
		);
	}

	if (simData->GetActiveParticles() <= 0) {
		// Just clear the textures and return.
		outputTextures.GetFeatureTexture(DEPTH)->Clear(depthClearColor);
		outputTextures.GetFeatureTexture(FOAM)->Clear(depthClearColor);
		outputTextures.GetFeatureTexture(FluidFeatureType::NORMALS)
			->Clear(genericClearColor);

		return;
	}

	context->SetRasterizerFlags(RasterizerFlags::DISABLE_CULL);

#ifdef GELLY_ENABLE_RENDERDOC_CAPTURES
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
	// RenderThickness();
	RenderFoam(false);
	RenderFoam(true);
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

#ifdef GELLY_ENABLE_RENDERDOC_CAPTURES
	if (renderDocApi != nullptr) {
		renderDocApi->EndFrameCapture(device, nullptr);
	}
#endif
}

void CD3D11SplattingFluidRenderer::EnableLowBitMode() { lowBitMode = true; }

void CD3D11SplattingFluidRenderer::SetSettings(
	const Gelly::FluidRenderSettings &settings
) {
	this->settings = settings;
}

void CD3D11SplattingFluidRenderer::SetPerFrameParams(
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

void CD3D11SplattingFluidRenderer::PullPerParticleData() {
	views.absorptionView =
		context->CreateMappedBufferView(buffers.particleAbsorption);
}

void CD3D11SplattingFluidRenderer::SetPerParticleAbsorption(
	const uint particleIndex, const float absorption[3]
) {
	views.absorptionView->Write(
		particleIndex,
		SimFloat4{absorption[0], absorption[1], absorption[2], 0.0}
	);
}

void CD3D11SplattingFluidRenderer::PushPerParticleData() {
	views.absorptionView.reset();
}

bool CD3D11SplattingFluidRenderer::CheckFeatureSupport(GELLY_FEATURE feature) {
	switch (feature) {
		case GELLY_FEATURE::FLUIDRENDER_PER_PARTICLE_ABSORPTION:
			return true;
		default:
			return false;
	}
}

#ifdef GELLY_ENABLE_RENDERDOC_CAPTURES
bool CD3D11SplattingFluidRenderer::EnableRenderDocCaptures() {
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