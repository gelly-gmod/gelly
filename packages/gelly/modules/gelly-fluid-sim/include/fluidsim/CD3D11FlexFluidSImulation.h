#ifndef CD3D11FLEXFLUIDSIMULATION_H
#define CD3D11FLEXFLUIDSIMULATION_H

#include "CD3D11CPUSimData.h"
#include "CFlexSimScene.h"
#include "CSimpleSimCommandList.h"
#include "IFluidSimulation.h"

class CD3D11FlexFluidSimulation : public IFluidSimulation {
private:
	static constexpr SimCommandType supportedCommands =
		static_cast<SimCommandType>(RESET | ADD_PARTICLE);

	CD3D11CPUSimData *simData;
	GellyObserverPtr<ISimContext> context{};

	int maxParticles;

	std::vector<CSimpleSimCommandList *> commandLists;
	CFlexSimScene *scene;

	NvFlexParams solverParams{};

	struct {
		NvFlexBuffer *positions;
		NvFlexBuffer *velocities;
		NvFlexBuffer *phases;
		NvFlexBuffer *actives;
	} buffers{};

	/**
	 * \brief These live entirely on the GPU and cannot be accessed at all
	 * by the host, unlike those contained in the buffers struct.
	 */
	struct {
		NvFlexBuffer *positions;
	} sharedBuffers{};

	NvFlexLibrary *library{};
	NvFlexSolver *solver{};

	// can be changed later via commands
	float particleRadius = 0.1f;
	float particleInverseMass = 1.0f;
	uint substeps = 2;

	void SetupParams();

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