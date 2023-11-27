#ifndef GELLY_H
#define GELLY_H

#include <GellyFluidSim.h>

#include "ILogger.h"
#include "Memory.h"
#include "fluidrender/IFluidRenderer.h"

#define GELLY_ALBEDO_TEXNAME "gelly/albedo"
#define GELLY_DEPTH_TEXNAME "gelly/depth"
#define GELLY_NORMAL_TEXNAME "gelly/normals"

namespace testbed {
void InitializeGelly(ID3D11Device *rendererDevice, ILogger *newLogger);
IFluidSimulation *GetGellyFluidSim();
IFluidRenderer *GetGellyFluidRenderer();
IRenderContext *GetGellyRenderContext();
}  // namespace testbed
#endif	// GELLY_H
