#include "fluidrender/CD3D11DebugFluidRenderer.h"

#include <d3d11.h>

#include <stdexcept>

CD3D11DebugFluidRenderer::CD3D11DebugFluidRenderer()
	: context(nullptr), simData(nullptr), buffers({}) {}

void CD3D11DebugFluidRenderer::CreateBuffers() {
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
	positionBufferDesc.initialData = nullptr;

	buffers.positions = context->CreateBuffer(positionBufferDesc);
}

void CD3D11DebugFluidRenderer::CreateTextures() {
	TextureDesc unfilteredDepthDesc = {};
	unfilteredDepthDesc.isFullscreen = true;
	unfilteredDepthDesc.access = TextureAccess::READ | TextureAccess::WRITE;
	unfilteredDepthDesc.format = TextureFormat::R32G32B32A32_FLOAT;

	internalTextures.unfilteredDepth = context->CreateTexture(
		"splatrenderer/unfilteredDepth", unfilteredDepthDesc
	);
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

	// .. unimplemented as there is still a few things to figure out.
}

void CD3D11DebugFluidRenderer::SetSettings(
	const Gelly::FluidRenderSettings &settings
) {
	this->settings = settings;
}