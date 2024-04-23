#ifndef GELLYHANDLES_H
#define GELLYHANDLES_H

#include <GellyFluidRender.h>
#include <GellyFluidSim.h>

// A data transfer object for grabbing the currently instantiated Gelly
// interfaces.
struct GellyHandles {
	IFluidRenderer *renderer;
	IFluidSimulation *simulation;
	IRenderContext *renderContext;
	ISimContext *simContext;
};

#endif	// GELLYHANDLES_H
