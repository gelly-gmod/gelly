#ifndef GELLY_IFLUIDSIMULATION_H
#define GELLY_IFLUIDSIMULATION_H

#include "GellyInterface.h"
#include "ISimContext.h"
#include "ISimData.h"

enum class FluidSimCompute {
	/**
	 * CPU-based simulation also uses D3D11.
	 */
	D3D11,
};

gelly_interface IFluidSimulation {
public:
	virtual ~IFluidSimulation() = default;

	virtual ISimData *GetSimulationData() = 0;
	virtual FluidSimCompute GetComputeType() = 0;
	virtual void AttachToContext(ISimContext * context) = 0;
	virtual void Update(float deltaTime) = 0;
};

#endif	// GELLY_IFLUIDSIMULATION_H
