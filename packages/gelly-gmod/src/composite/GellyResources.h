#ifndef GELLYRESOURCES_H
#define GELLYRESOURCES_H

#include <device.h>
#include <renderers/splatting/splatting-renderer.h>

using gelly::renderer::Device;
using gelly::renderer::splatting::SplattingRenderer;

struct GellyResources {
	std::shared_ptr<Device> device;
	std::shared_ptr<SplattingRenderer> splattingRenderer;
};

#endif	// GELLYRESOURCES_H
