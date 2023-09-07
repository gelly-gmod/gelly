#ifndef GELLY_GBUFFER_H
#define GELLY_GBUFFER_H

#include <GellyD3D.h>

struct GBuffer {
	d3d11::Texture depth;
	d3d11::Texture normal;
};

#endif	// GELLY_GBUFFER_H