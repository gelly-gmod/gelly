#ifndef GELLY_IFLUIDSIMULATION_H
#define GELLY_IFLUIDSIMULATION_H

#include "GellyInterface.h"
#include "ISimContext.h"
#include "ISimData.h"

gelly_interface IFluidSimulation {
public:
	virtual ~IFluidSimulation() = default;

	virtual ISimData *GetSimulationData() = 0;
	virtual void AttachToContext(ISimContext * context) = 0;
	virtual void Update(float deltaTime) = 0;
};

#endif	// GELLY_IFLUIDSIMULATION_H
