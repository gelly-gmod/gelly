#ifndef CD3D11FLEXFLUIDSIMULATION_H
#define CD3D11FLEXFLUIDSIMULATION_H

#include <functional>

#include "CD3D11CPUSimData.h"
#include "CFlexSimScene.h"
#include "CSimpleSimCommandList.h"
#include "IFluidSimulation.h"

class CD3D11FlexFluidSimulation : public IFluidSimulation {
private:
	static constexpr SimCommandType supportedCommands =
		static_cast<SimCommandType>(
			RESET | ADD_PARTICLE | CHANGE_RADIUS | SET_FLUID_PROPERTIES |
			CONFIGURE
		);

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
		NvFlexBuffer *contactVelocities;
		NvFlexBuffer *contactCounts;
		NvFlexBuffer *diffuseParticleCount;
	} buffers{};

	/**
	 * \brief These live entirely on the GPU and cannot be accessed at all
	 * by the host, unlike those contained in the buffers struct.
	 */
	struct {
		NvFlexBuffer *positions;
		NvFlexBuffer *velocities0;
		NvFlexBuffer *velocities1;
		NvFlexBuffer *foamPositions;
		NvFlexBuffer *foamVelocities;
		NvFlexBuffer *anisotropyQ1Buffer;
		NvFlexBuffer *anisotropyQ2Buffer;
		NvFlexBuffer *anisotropyQ3Buffer;

		bool velocityBufferSwapped = false;
	} sharedBuffers{};

	NvFlexLibrary *library{};
	NvFlexSolver *solver{};

	// can be changed later via commands
	float particleRadius = 0.1f;
	float particleInverseMass = 1.f;
	uint maxContactsPerParticle = 6;
	int substeps = 3;
	float timeStepMultiplier = 1.f;

	struct {
		bool deferFlag : 1;
		uint newActiveCount : 31;
	} particleCountUpdateFlags{};

	void SetupParams();
	void DebugDumpParams();
	void SetDeferredActiveParticleCount(uint newActiveCount);

public:
	CD3D11FlexFluidSimulation();
	CD3D11FlexFluidSimulation(const CD3D11FlexFluidSimulation &) = delete;
	CD3D11FlexFluidSimulation &operator=(const CD3D11FlexFluidSimulation &) =
		delete;
	CD3D11FlexFluidSimulation(CD3D11FlexFluidSimulation &&) = delete;
	CD3D11FlexFluidSimulation &operator=(CD3D11FlexFluidSimulation &&) = delete;

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
	void SetTimeStepMultiplier(float timeStepMultiplier) override;

	const char *GetComputeDeviceName() override;
	bool CheckFeatureSupport(GELLY_FEATURE feature) override;
	unsigned int GetRealActiveParticleCount() override;

	void VisitLatestContactPlanes(ContactPlaneVisitor visitor) override;
};

#endif	// CD3D11FLEXFLUIDSIMULATION_H
