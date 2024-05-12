#ifndef UNOWNEDRESOURCES_H
#define UNOWNEDRESOURCES_H

#include <d3d9.h>

/**
 * @brief Resources owned by Garry's Mod that are forcibly forwarded to the
 * pipeline. It's the responsibility of the pipeline to take care of the
 * resources that are instantiated via these pointers. Anything that is created
 * which is not released will stay in memory until the game is closed.
 *
 * Also, the pipeline MUST add/remove hooks according to the lifetime of the
 * pipeline object. These pointers may be kept alive for the entire lifetime of
 * the game, which means failure to remove a hook could cause a MinHook
 * exception.
 */
struct UnownedResources {
	IDirect3D9 *d3d9;
	IDirect3DDevice9Ex *device;
};

#endif	// UNOWNEDRESOURCES_H
