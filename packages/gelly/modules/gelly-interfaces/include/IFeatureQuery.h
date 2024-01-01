#ifndef IFEATUREQUERY_H
#define IFEATUREQUERY_H

#include "GellyInterface.h"

enum class GELLY_FEATURE {
	FLUIDSIM_CONTACTPLANES,
	FLUIDRENDER_PER_PARTICLE_ABSORPTION,
};

gelly_interface IFeatureQuery {
public:
	virtual ~IFeatureQuery() = default;

	virtual bool CheckFeatureSupport(GELLY_FEATURE feature) = 0;
};

#endif	// IFEATUREQUERY_H
