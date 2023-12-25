#include "Gelly.h"
#include "LoggingMacros.h"

static const int defaultMaxParticles = 100000;
GellyIntegration::GellyIntegration() {
	try {
		renderContext = CreateD3D11FluidRenderContext(800, 600);
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
	} catch (const std::exception &e) {
		LOG_ERROR("Failed to create render context: %s", e.what());
	}
}

GellyIntegration::~GellyIntegration() {
	if (renderContext) {
		// soon
	}
}