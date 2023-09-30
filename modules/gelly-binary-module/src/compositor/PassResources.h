#ifndef GELLY_PASSRESOURCES_H
#define GELLY_PASSRESOURCES_H

#include <GellyRenderer.h>

struct PassGBuffer {
	d3d9::Texture framebuffer;
	SharedTextures shared;
};

struct PassResources {
	IDirect3DDevice9Ex *device;
	PassGBuffer *gbuffer;
	Camera *camera;
};

#endif	// GELLY_PASSRESOURCES_H
