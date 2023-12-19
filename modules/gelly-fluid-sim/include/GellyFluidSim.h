#ifndef GELLY_GELLYFLUIDSIM_H
#define GELLY_GELLYFLUIDSIM_H

#include <d3d11.h>

#include <filesystem>

#include "fluidsim/CD3D11DebugFluidSimulation.h"
#include "fluidsim/ISimContext.h"

namespace Gelly {
ISimContext *CreateD3D11SimContext(
	ID3D11Device *device, ID3D11DeviceContext *deviceContext
);

IFluidSimulation *CreateD3D11DebugFluidSimulation(
	GellyObserverPtr<ISimContext> context
);

IFluidSimulation *CreateD3D11RTFRFluidSimulation(
	GellyObserverPtr<ISimContext> context,
	const std::filesystem::path &folderPath
);
}  // namespace Gelly
#endif	// GELLY_GELLYFLUIDSIM_H
