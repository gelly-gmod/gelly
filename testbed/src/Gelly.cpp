#include "Gelly.h"

#include <GellyFluidSim.h>

#include "ILogger.h"
#include "Rendering.h"

using namespace testbed;

static ILogger *logger = nullptr;
static ISimContext *simContext = nullptr;
static IFluidSimulation *fluidSim = nullptr;
constexpr int maxParticles = 1000;

void testbed::InitializeGelly(
	ID3D11Device *rendererDevice, ILogger *newLogger
) {
	logger = newLogger;
	try {
		logger->Info("Creating the Gelly context...");
		simContext = CreateD3D11SimContext(
			rendererDevice, GetRendererContext(rendererDevice)
		);
		logger->Info("Creating the Gelly fluid simulation...");
		fluidSim = CreateD3D11DebugFluidSimulation(simContext, maxParticles);
	} catch (const std::exception &e) {
		logger->Error("Failed to call Gelly: %s", e.what());
		throw;
	}
	logger->Info("Gelly initialized");
}

IFluidSimulation *testbed::GetGellyFluidSim() { return fluidSim; }