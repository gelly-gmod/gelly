#ifndef GELLY_GELLYFLUIDSIM_H
#define GELLY_GELLYFLUIDSIM_H

#include <d3d11.h>

#include "fluidsim/CD3D11DebugFluidSimulation.h"
#include "fluidsim/ISimContext.h"

ISimContext *GFluidSim_CreateD3D11SimContext(
	ID3D11Device *device, ID3D11DeviceContext *deviceContext
);

void GFluidSim_DestroyD3D11SimContext(ISimContext *context);

CD3D11DebugFluidSimulation *GFluidSim_CreateD3D11DebugFluidSimulation(
	int maxParticles
);

void GFluidSim_DestroyD3D11DebugFluidSimulation(
	CD3D11DebugFluidSimulation *simulation
);

#endif	// GELLY_GELLYFLUIDSIM_H
