#include "fluidrender/CD3D11DebugFluidRenderer.h"

#include <d3d11.h>

#include <stdexcept>

#include "FilterDepthPS.h"
#include "ScreenQuadVS.h"
#include "SplattingGS.h"
#include "SplattingPS.h"
#include "SplattingVS.h"
#include "fluidrender/util/CBuffers.h"
#include "fluidrender/util/ScreenQuadVB.h"

#ifdef _DEBUG
#include <windows.h>
#endif

CD3D11DebugFluidRenderer::CD3D11DebugFluidRenderer()
	: context(nullptr), simData(nullptr), buffers({}) {}

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
		util::CreateCBuffer<FluidRenderCBuffer>(context);

	BufferLayoutDesc positionLayoutDesc = {};
	positionLayoutDesc.items[0] = {
		0, "SV_POSITION", 0, BufferLayoutFormat::FLOAT4
	};
	positionLayoutDesc.itemCount = 1;
	positionLayoutDesc.vertexShader = shaders.splattingVS;
	positionLayoutDesc.topology = BufferLayoutTopology::POINTS;

	buffers.positionsLayout = context->CreateBufferLayout(positionLayoutDesc);
	buffers.positionsLayout->AttachBufferAtSlot(buffers.positions, 0);

	DepthBufferDesc depthBufferDesc = {};
	depthBufferDesc.format = DepthFormat::D24S8;
	depthBufferDesc.depthOp = DepthOp::LESS_EQUAL;

	buffers.depthBuffer = context->CreateDepthBuffer(depthBufferDesc);

	std::tie(buffers.screenQuad, buffers.screenQuadLayout) =
		util::GenerateScreenQuad(context, shaders.screenQuadVS);
}

void CD3D11DebugFluidRenderer::CreateTextures() {
	TextureDesc unfilteredDepthDesc = {};
	unfilteredDepthDesc.isFullscreen = true;
	unfilteredDepthDesc.access = TextureAccess::READ | TextureAccess::WRITE;
	unfilteredDepthDesc.format = TextureFormat::R32G32B32A32_FLOAT;

	internalTextures.unfilteredDepth = context->CreateTexture(
		"splatrenderer/unfilteredDepth", unfilteredDepthDesc
	);

	constexpr float clearColor[4] = {1.0f, 0.0f, 0.0f, 1.0f};
	internalTextures.unfilteredDepth->Clear(clearColor);
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

constexpr float clearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};

void CD3D11DebugFluidRenderer::RenderUnfilteredDepth() {
	// First we'll render out the depth.
	buffers.depthBuffer->Clear(1.0f);
	auto *depthTexture = internalTextures.unfilteredDepth;

	depthTexture->Clear(clearColor);
	depthTexture->BindToPipeline(
		TextureBindStage::RENDER_TARGET_OUTPUT, 0, buffers.depthBuffer
	);

	buffers.positionsLayout->BindAsVertexBuffer();

	shaders.splattingGS->Bind();
	shaders.splattingPS->Bind();
	shaders.splattingVS->Bind();

	buffers.fluidRenderCBuffer->BindToPipeline(ShaderType::Pixel, 0);
	buffers.fluidRenderCBuffer->BindToPipeline(ShaderType::Vertex, 0);
	buffers.fluidRenderCBuffer->BindToPipeline(ShaderType::Geometry, 0);

	context->Draw(maxParticles, 0);
	// we're not using a swapchain, so we need to queue up work manually
	context->SubmitWork();
	context->ResetPipeline();
}

void CD3D11DebugFluidRenderer::Render() {
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

	// Now we'll filter the depth.
	auto *depthTexture =
		outputTextures.GetFeatureTexture(FluidFeatureType::DEPTH);

	depthTexture->Clear(clearColor);
	depthTexture->BindToPipeline(
		TextureBindStage::RENDER_TARGET_OUTPUT, 0, buffers.depthBuffer
	);

	internalTextures.unfilteredDepth->BindToPipeline(
		TextureBindStage::PIXEL_SHADER_READ, 0, nullptr
	);

	shaders.screenQuadVS->Bind();
	shaders.filterDepthPS->Bind();

	buffers.fluidRenderCBuffer->BindToPipeline(ShaderType::Pixel, 0);
	buffers.screenQuadLayout->BindAsVertexBuffer();

	context->Draw(4, 0);
	context->SubmitWork();

#ifdef _DEBUG
	if (renderDocApi != nullptr) {
		renderDocApi->EndFrameCapture(device, nullptr);
	}
#endif
}

void CD3D11DebugFluidRenderer::SetSettings(
	const Gelly::FluidRenderSettings &settings
) {
	this->settings = settings;
}

void CD3D11DebugFluidRenderer::SetPerFrameParams(
	const Gelly::FluidRenderParams &params
) {
	cbufferData.view = params.view;
	cbufferData.proj = params.proj;
	cbufferData.invView = params.invView;
	cbufferData.invProj = params.invProj;

	util::UpdateCBuffer(&cbufferData, buffers.fluidRenderCBuffer);
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