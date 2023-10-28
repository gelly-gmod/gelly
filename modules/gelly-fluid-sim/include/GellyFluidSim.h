#ifndef GELLY_GELLYFLUIDSIM_H
#define GELLY_GELLYFLUIDSIM_H

#include <d3d11.h>

#include "fluidsim/ISimContext.h"

ISimContext *GFluidSim_CreateD3D11SimContext(
	ID3D11Device *device, ID3D11DeviceContext *deviceContext
);

void GFluidSim_DestroyD3D11SimContext(ISimContext *context);

#endif	// GELLY_GELLYFLUIDSIM_H
