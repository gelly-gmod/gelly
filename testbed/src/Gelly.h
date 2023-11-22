#ifndef GELLY_H
#define GELLY_H

#include <GellyFluidSim.h>

#include "ILogger.h"

namespace testbed {
void InitializeGelly(ID3D11Device *rendererDevice, ILogger *newLogger);
IFluidSimulation *GetGellyFluidSim();
}  // namespace testbed
#endif	// GELLY_H
