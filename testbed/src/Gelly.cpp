#include "Gelly.h"

#include <GellyFluidSim.h>

#include "D3D11DebugLayer.h"
#include "GellyFluidRender.h"
#include "ILogger.h"
#include "Rendering.h"
#include "Window.h"
#include "fluidrender/IFluidRenderer.h"
#include "fluidrender/IRenderContext.h"

using namespace testbed;

static ILogger *logger = nullptr;
static ISimContext *simContext = nullptr;
static IFluidSimulation *fluidSim = nullptr;
static IRenderContext *renderContext = nullptr;
static IFluidRenderer *fluidRenderer = nullptr;

constexpr int maxParticles = 1000;

void testbed::InitializeGelly(
	ID3D11Device *rendererDevice, ILogger *newLogger
) {
	logger = newLogger;
	try {
		logger->Info("Creating the Gelly sim context...");
		simContext = CreateD3D11SimContext(
			rendererDevice, GetRendererContext(rendererDevice)
		);

		logger->Info("Creating the Gelly fluid simulation...");
		fluidSim = CreateD3D11DebugFluidSimulation(simContext);

		logger->Info("Creating the Gelly render context...");
		renderContext =
			CreateD3D11FluidRenderContext(WINDOW_WIDTH, WINDOW_HEIGHT);

		logger->Info("Creating the Gelly fluid renderer...");
		fluidRenderer = CreateD3D11DebugFluidRenderer(renderContext);

		logger->Info("Linking the Gelly fluid simulation and renderer...");
		fluidSim->SetMaxParticles(maxParticles);
		fluidRenderer->SetSimData(fluidSim->GetSimulationData());

		fluidSim->Initialize();
	} catch (const std::exception &e) {
#ifdef _DEBUG
		renderContext->PrintDebugInfo();
#endif
		logger->Error("Failed to call Gelly: %s", e.what());
		throw;
	}
	logger->Info("Gelly initialized");
}

IFluidSimulation *testbed::GetGellyFluidSim() { return fluidSim; }