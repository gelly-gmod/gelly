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
	GellyObserverPtr<ISimContext> context;
	CD3D11CPUSimData *simData;
	ID3D11Buffer *positionBuffer;

	int maxParticles;

	void CreateBuffers();
	void GenerateRandomParticles();

public:
	explicit CD3D11DebugFluidSimulation();
	~CD3D11DebugFluidSimulation() override;

	void SetMaxParticles(int maxParticles) override;
	void Initialize() override;
	ISimData *GetSimulationData() override;
	ISimScene *GetScene() override;
	SimContextAPI GetComputeAPI() override;
	void AttachToContext(GellyObserverPtr<ISimContext> context) override;
	void Update(float deltaTime) override;
};

#endif	// GELLY_CD3D11DEBUGFLUIDSIMULATION_H
