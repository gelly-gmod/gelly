#ifndef GELLY_CD3D11DEBUGFLUIDSIMULATION_H
#define GELLY_CD3D11DEBUGFLUIDSIMULATION_H

#include "CD3D11CPUSimData.h"
#include "IFluidSimulation.h"

/**
 * Fluid simulation which really doesnt simulate anything at all and generates
 * a random particle cloud at attach time.
 */
class CD3D11DebugFluidSimulation : public IFluidSimulation {
private:
	CD3D11CPUSimData *simData;
	int maxParticles;

	void GenerateRandomParticles();

public:
	explicit CD3D11DebugFluidSimulation(int maxParticles);
	~CD3D11DebugFluidSimulation() override;

	ISimData *GetSimulationData() override;
	void AttachToContext(ISimContext *context) override;
	FluidSimCompute GetComputeType() override;
	void Update(float deltaTime) override;
};

#endif	// GELLY_CD3D11DEBUGFLUIDSIMULATION_H
