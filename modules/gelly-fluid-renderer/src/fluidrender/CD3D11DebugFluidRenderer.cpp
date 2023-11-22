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
	positionBufferDesc.byteWidth = sizeof(SimFloat4) * maxParticles;
	positionBufferDesc.stride = sizeof(SimFloat4);
	positionBufferDesc.initialData = nullptr;

	buffers.positions = context->CreateBuffer(positionBufferDesc);
}

void CD3D11DebugFluidRenderer::SetSimData(GellyObserverPtr<ISimData> simData) {
	if (simData == nullptr) {
		throw std::invalid_argument(
			"CD3D11DebugFluidRenderer::SetSimData: simData cannot be null."
		);
	}

	if (simData->GetAPI() != SimContextAPI::D3D11) {
		throw std::logic_error(
			"CD3D11DebugFluidRenderer::SetSimData: simData must be backed by "
			"D3D11"
		);
	}

	if (buffers.positions == nullptr) {
		throw std::logic_error(
			"CD3D11DebugFluidRenderer::SetSimData: buffers.positions is null. "
			"Cannot link."
		);
	}

	this->simData = simData;
	simData->LinkBuffer(
		SimBufferType::POSITION, buffers.positions->GetBufferResource()
	);
}

void CD3D11DebugFluidRenderer::SetMaxParticles(int maxParticles) {
	if (maxParticles <= 0) {
		throw std::invalid_argument(
			"CD3D11DebugFluidRenderer::SetMaxParticles: maxParticles must be "
			"greater than 0."
		);
	}

	this->maxParticles = maxParticles;
}

void CD3D11DebugFluidRenderer::AttachToContext(
	GellyObserverPtr<IRenderContext> context
) {
	if (context == nullptr) {
		throw std::invalid_argument(
			"CD3D11DebugFluidRenderer::AttachToContext: context cannot be null."
		);
	}

	if (context->GetRenderAPI() != ContextRenderAPI::D3D11) {
		throw std::logic_error(
			"CD3D11DebugFluidRenderer::AttachToContext: context must be backed "
			"by D3D11"
		);
	}

	this->context = context;
	CreateBuffers();
}

GellyObserverPtr<IFluidTextures> CD3D11DebugFluidRenderer::GetFluidTextures() {
	return &textures;
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