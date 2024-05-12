#ifndef GELLYRESOURCES_H
#define GELLYRESOURCES_H

#include "fluidrender/IFluidRenderer.h"
#include "fluidrender/IFluidTextures.h"

struct GellyResources {
	IFluidTextures *textures;
	std::shared_ptr<IFluidRenderer> renderer;
	std::shared_ptr<IRenderContext> context;
};

#endif	// GELLYRESOURCES_H
