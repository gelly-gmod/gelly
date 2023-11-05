#ifndef GELLY_CD3D11DEBUGFLUIDRENDERER_H
#define GELLY_CD3D11DEBUGFLUIDRENDERER_H

#include <GellyFluidSim.h>

#include "CD3D11DebugFluidTextures.h"
#include "CD3D11ManagedBuffer.h"
#include "IFluidRenderer.h"

class CD3D11DebugFluidRenderer : public IFluidRenderer {
private:
	GellyObserverPtr<IRenderContext> context;
	/**
	 * The particle data comes from here, but the renderer does not own it.
	 */
	GellyObserverPtr<ISimData> simData;
	IFluidTextures *fluidTextures;

	Gelly::FluidRenderSettings settings;
};

#endif	// GELLY_CD3D11DEBUGFLUIDRENDERER_H
