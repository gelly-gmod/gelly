#ifndef GELLY_IFLUIDTEXTURES_H
#define GELLY_IFLUIDTEXTURES_H

#include <GellyInterface.h>

#include "GellyObserverPtr.h"
#include "IManagedTexture.h"

namespace Gelly {
enum FluidFeatureType {
	ALBEDO,
	NORMALS,
	DEPTH,
	// The rest beyond these will likely depend on the renderer.
	// The above are considered critical for rendering.
};
}

using namespace Gelly;

/**
 * Interface for accessing the set of textures which make up the fluid surface.
 * There are three critical textures which must be provided:
 * - Albedo
 * - Normals
 * - Depth
 * The rest are optional, and depend on the renderer.
 * This design allows for the end user to implement their own
 * uber-shader which can use different features depending on which
 * renderer is chosen.
 */
gelly_interface IFluidTextures {
public:
	virtual ~IFluidTextures() = default;

	/**
	 * Sets the texture for the given feature. More often than not,
	 * this is gonna be a shared texture. Nonetheless, there are a few
	 * requirements. The texture must be read/write, and it must be
	 * able to be used as a render target.
	 *
	 * @note The texture must be created by the user, and through the render
	 * context, the same one as the fluid renderer is using.
	 * @param type
	 * @param texture
	 */
	virtual void SetFeatureTexture(
		FluidFeatureType type, GellyObserverPtr<IManagedTexture> texture
	) = 0;

	/**
	 * Returns the texture for the given feature.
	 * @param type
	 * @return
	 */
	virtual GellyObserverPtr<IManagedTexture> GetFeatureTexture(
		FluidFeatureType type
	) = 0;

	/**
	 * Subclasses which are used by the target rendering technique should only
	 * return true in this function if all of the features are initialized for
	 * that specific technique.
	 * @return
	 */
	virtual bool IsInitialized() = 0;
};

#endif	// GELLY_IFLUIDTEXTURES_H
