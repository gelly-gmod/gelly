#ifndef CD3D11RTFRFLUIDSIMULATION_H
#define CD3D11RTFRFLUIDSIMULATION_H

#include "CD3D11CPUSimData.h"
#include "CSimpleSimCommandList.h"
#include "IFluidSimulation.h"
#include "rtfr/Dataset.h"

class CD3D11RTFRFluidSimulation : public IFluidSimulation {
private:
	constexpr static SimCommandType supportedCommands = SimCommandType::RESET;

	GellyObserverPtr<ISimContext> context{};
	CD3D11CPUSimData *simData;
	ID3D11Buffer *positionBuffer;

	int maxParticles;
	int activeParticles;
	uint currentFrameIndex;

	rtfr::DatasetInfo datasetInfo;
	rtfr::Dataset dataset;

	std::vector<CSimpleSimCommandList *> commandLists;

	void CreateBuffers();
	void LoadFrameIntoBuffers();

public:
	CD3D11RTFRFluidSimulation();
	~CD3D11RTFRFluidSimulation() override;

	void SetMaxParticles(int maxParticles) override;
	void Initialize() override;

	void LoadDatasetFromFolder(const std::filesystem::path &folderPath);

	ISimData *GetSimulationData() override;
	ISimScene *GetScene() override;
	SimContextAPI GetComputeAPI() override;

	ISimCommandList *CreateCommandList() override;
	void DestroyCommandList(ISimCommandList *commandList) override;
	void ExecuteCommandList(ISimCommandList *commandList) override;

	void AttachToContext(GellyObserverPtr<ISimContext> context) override;

	void Update(float deltaTime) override;
	void SetTimeStepMultiplier(float timeStepMultiplier) override{};
	const char *GetComputeDeviceName() override;
	bool CheckFeatureSupport(GELLY_FEATURE feature) override;

	void VisitLatestContactPlanes(ContactPlaneVisitor visitor) override{};
};

#endif	// CD3D11RTFRFLUIDSIMULATION_H
