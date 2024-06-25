#ifndef GELLYRESOURCES_H
#define GELLYRESOURCES_H

#include <device.h>
#include <renderers/splatting/splatting-renderer.h>

#include "fluidrender/IFluidRenderer.h"
#include "fluidrender/IFluidTextures.h"

using gelly::renderer::Device;
using gelly::renderer::splatting::SplattingRenderer;

struct GellyResources {
	IFluidTextures *textures;
	std::shared_ptr<Device> device;
	std::shared_ptr<SplattingRenderer> splattingRenderer;
};

#endif	// GELLYRESOURCES_H
