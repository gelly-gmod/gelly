#ifndef GELLY_ISIMDATA_H
#define GELLY_ISIMDATA_H

#include "GellyInterface.h"

struct SimFloat4 {
	float x, y, z, w;
};

enum class SimBuffer {
	Position,
	Velocity,
};

gelly_interface ISimData {
public:
	/**
	 * Destroys the underlying buffers.
	 */
	virtual ~ISimData() = 0;

	virtual void Initialize(int maxParticles) = 0;

	/**
	 * Returns a buffer created in a rendering API. The exact one is determined
	 * by the implementation.
	 * @param buffer
	 * @return
	 */
	virtual void *GetRenderBuffer(SimBuffer buffer) = 0;

	virtual SimFloat4 *MapBuffer(SimBuffer buffer) = 0;
	virtual void UnmapBuffer(SimBuffer buffer) = 0;
};

#endif	// GELLY_ISIMDATA_H
