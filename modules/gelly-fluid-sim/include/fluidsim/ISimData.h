#ifndef GELLY_ISIMDATA_H
#define GELLY_ISIMDATA_H

#include "GellyInterface.h"

namespace Gelly {
struct SimFloat4 {
	float x, y, z, w;
};

enum class SimBufferType {
	POSITION,
	VELOCITY,
};
}  // namespace Gelly

using namespace Gelly;

gelly_interface ISimData {
public:
	/**
	 * Destroys the underlying buffers.
	 */
	virtual ~ISimData() = 0;

	/**
	 * Sets the underlying pointer to a buffer resource in the same rendering
	 * API as the parent simulation is in.
	 *
	 * Basically, this function links a buffer to the simulation data, which is
	 * useful for rendering without having to perform CPU readbacks.
	 */
	virtual void LinkBuffer(SimBufferType type, void *buffer);
	virtual bool IsBufferLinked(SimBufferType type);

	virtual void *GetLinkedBuffer(SimBufferType type);
};

#endif	// GELLY_ISIMDATA_H
