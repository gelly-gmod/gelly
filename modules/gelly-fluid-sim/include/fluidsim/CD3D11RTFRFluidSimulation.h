#ifndef CD3D11RTFRFLUIDSIMULATION_H
#define CD3D11RTFRFLUIDSIMULATION_H

#include "CD3D11CPUSimData.h"
#include "IFluidSimulation.h"
#include "rtfr/Dataset.h"

class CD3D11RTFRFluidSimulation : public IFluidSimulation {
private:
	GellyObserverPtr<ISimContext> context{};
	CD3D11CPUSimData *simData;
	ID3D11Buffer *positionBuffer;

	int maxParticles;
	int activeParticles;
	uint currentFrameIndex;

	rtfr::DatasetInfo datasetInfo;
	rtfr::Dataset dataset;

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
	void AttachToContext(GellyObserverPtr<ISimContext> context) override;

	void Update(float deltaTime) override;
};

#endif	// CD3D11RTFRFLUIDSIMULATION_H
