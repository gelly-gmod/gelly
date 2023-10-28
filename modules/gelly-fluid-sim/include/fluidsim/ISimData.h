#ifndef GELLY_ISIMDATA_H
#define GELLY_ISIMDATA_H

#include "GellyInterface.h"

enum class SimBuffer {
	Position,
	Velocity,
};

gelly_interface ISimData {
public:
	virtual ~ISimData() = default;

	/**
	 * Returns a buffer created in a rendering API. The exact one is determined
	 * by the implementation.
	 * @param buffer
	 * @return
	 */
	virtual void *GetRenderBuffer(SimBuffer buffer) = 0;
};

#endif	// GELLY_ISIMDATA_H
