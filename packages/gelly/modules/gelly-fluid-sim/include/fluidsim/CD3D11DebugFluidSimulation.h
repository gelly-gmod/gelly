#ifndef GELLY_CD3D11DEBUGFLUIDSIMULATION_H
#define GELLY_CD3D11DEBUGFLUIDSIMULATION_H

#include "CD3D11CPUSimData.h"
#include "CSimpleSimCommandList.h"
#include "IFluidSimulation.h"

/**
 * Fluid simulation which really doesnt simulate anything at all and generates
 * a random particle cloud at attach time.
 */
class CD3D11DebugFluidSimulation : public IFluidSimulation {
private:
	constexpr static SimCommandType supportedCommands = SimCommandType::RESET;

	GellyObserverPtr<ISimContext> context;
	CD3D11CPUSimData *simData;
	ID3D11Buffer *positionBuffer;

	int maxParticles;

	std::vector<CSimpleSimCommandList *> commandLists;

	void CreateBuffers();
	void GenerateRandomParticles();

public:
	explicit CD3D11DebugFluidSimulation();
	~CD3D11DebugFluidSimulation() override;

	void SetMaxParticles(int maxParticles) override;
	void Initialize() override;
	ISimData *GetSimulationData() override;
	gelly::simulation::Scene *GetNewScene() override { return nullptr; }

	SimContextAPI GetComputeAPI() override;

	ISimCommandList *CreateCommandList() override;
	void DestroyCommandList(ISimCommandList *commandList) override;
	void ExecuteCommandList(ISimCommandList *commandList) override;

	void AttachToContext(GellyObserverPtr<ISimContext> context) override;
	void Update(float deltaTime) override;
	void SetTimeStepMultiplier(float timeStepMultiplier) override{};

	const char *GetComputeDeviceName() override;
	bool CheckFeatureSupport(GELLY_FEATURE feature) override;

	unsigned int GetRealActiveParticleCount() override { return 0; }

	void VisitLatestContactPlanes(ContactPlaneVisitor visitor) override{};
};

#endif	// GELLY_CD3D11DEBUGFLUIDSIMULATION_H
