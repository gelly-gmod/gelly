#ifndef GELLY_ISHAREDTEXTURE_H
#define GELLY_ISHAREDTEXTURE_H

#include <GellyInterface.h>

#include "IManagedTexture.h"

gelly_interface ISharedTexture : public IManagedTexture {
public:
	/**
	 * Sets the shared rendering API handle of the texture.
	 * For D3D11, this will be the result of GetSharedHandle() of IDXGIResource
	 * For D3D9, this will be the result of the "sharedHandle" parameter of
	 * CreateTexture()
	 * @param handle
	 */
	virtual void SetSharedHandle(void *handle) = 0;
};

#endif	// GELLY_ISHAREDTEXTURE_H
