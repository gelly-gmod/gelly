#ifndef GELLY_GBUFFER_H
#define GELLY_GBUFFER_H

#include <GellyD3D.h>

struct GBuffer {
	float width, height = 0;

	// The rest of these are internally created by the renderer, so that they're
	// 32-bit and have custom formats unlike the ones in output which are 16-bit
	// and have fixed formats.

	d3d11::Texture depth;
	d3d11::Texture filteredDepth;
	d3d11::Texture normal;

	struct {
		d3d11::Texture depth;
		d3d11::Texture normal;
	} output;
};

#endif	// GELLY_GBUFFER_H