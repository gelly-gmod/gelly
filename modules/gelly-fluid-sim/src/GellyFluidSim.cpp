#include "GellyFluidSim.h"

#include "fluidsim/CD3D11SimContext.h"

ISimContext *GFluidSim_CreateD3D11SimContext(
	ID3D11Device *device, ID3D11DeviceContext *deviceContext
) {
	return new CD3D11SimContext(device, deviceContext);
}

void GFluidSim_DestroyD3D11SimContext(ISimContext *context) { delete context; }

CD3D11DebugFluidSimulation *GFluidSim_CreateD3D11DebugFluidSimulation(
	int maxParticles
) {
	return new CD3D11DebugFluidSimulation(maxParticles);
}

void GFluidSim_DestroyD3D11DebugFluidSimulation(
	CD3D11DebugFluidSimulation *simulation
) {
	delete simulation;
}