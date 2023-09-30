#ifndef GELLY_PASSRESOURCES_H
#define GELLY_PASSRESOURCES_H

struct PassGBuffer {
	d3d9::Texture framebuffer;
	SharedTextures shared;
};

struct PassResources {
	IDirect3DDevice9 *device;
	PassGBuffer *gbuffer;
};

#endif	// GELLY_PASSRESOURCES_H
