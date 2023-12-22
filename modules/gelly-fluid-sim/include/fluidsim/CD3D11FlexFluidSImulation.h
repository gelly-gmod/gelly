#ifndef CD3D11FLEXFLUIDSIMULATION_H
#define CD3D11FLEXFLUIDSIMULATION_H

#include "CD3D11CPUSimData.h"
#include "CSimpleSimCommandList.h"
#include "IFluidSimulation.h"

class CD3D11FlexFluidSimulation : public IFluidSimulation {
private:
	CD3D11CPUSimData *simData;
	GellyObserverPtr<ISimContext> context{};
	int maxParticles;

	std::vector<CSimpleSimCommandList *> commandLists;

public:
	CD3D11FlexFluidSimulation();
	~CD3D11FlexFluidSimulation() override;

	void SetMaxParticles(int maxParticles) override;
	void Initialize() override;

	ISimData *GetSimulationData() override;
	ISimScene *GetScene() override;
	SimContextAPI GetComputeAPI() override;

	void AttachToContext(GellyObserverPtr<ISimContext> context) override;

	ISimCommandList *CreateCommandList() override;
	void DestroyCommandList(ISimCommandList *commandList) override;
	void ExecuteCommandList(ISimCommandList *commandList) override;

	void Update(float deltaTime) override;
};

#endif	// CD3D11FLEXFLUIDSIMULATION_H
