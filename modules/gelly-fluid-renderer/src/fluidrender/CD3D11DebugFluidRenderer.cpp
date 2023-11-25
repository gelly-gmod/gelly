#include "fluidrender/CD3D11DebugFluidRenderer.h"

#include <d3d11.h>

#include <stdexcept>

#include "SplattingGS.h"
#include "SplattingPS.h"
#include "SplattingVS.h"
#include "fluidrender/util/CBuffers.h"

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
}

GellyObserverPtr<IFluidTextures> CD3D11DebugFluidRenderer::GetFluidTextures() {
	return &outputTextures;
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

	// First we'll render out the depth.
	outputTextures.GetFeatureTexture(FluidFeatureType::DEPTH)
		->BindToPipeline(TextureBindStage::RENDER_TARGET_OUTPUT, 0);

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
