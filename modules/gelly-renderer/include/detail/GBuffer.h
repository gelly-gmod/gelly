#ifndef GELLY_GBUFFER_H
#define GELLY_GBUFFER_H

#include <GellyD3D.h>

struct GBuffer {
	d3d11::Texture depth_low;
	d3d11::Texture depth_high;
	d3d11::Texture normal;
};

#endif	// GELLY_GBUFFER_H