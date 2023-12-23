#include "fluidsim/CD3D11FlexFluidSimulation.h"

#include <NvFlex.h>

#include <string>

// TODO: deduplicate this
struct FlexFloat4 {
	float x, y, z, w;
};

struct FlexFloat3 {
	float x, y, z;
};

// flex's design isn't exactly what i'd call flexible so
// we set up a global error callback to throw exceptions
// when flex errors occur

static void FlexErrorCallback(
	NvFlexErrorSeverity severity, const char *msg, const char *file, int line
) {
	throw std::runtime_error(
		"FlexErrorCallback: " + std::string(msg) + " at " + std::string(file) +
		":" + std::to_string(line)
	);
}

CD3D11FlexFluidSimulation::CD3D11FlexFluidSimulation()
	: simData(new CD3D11CPUSimData()),
	  maxParticles(0),
	  commandLists({}),
	  scene(nullptr) {}

CD3D11FlexFluidSimulation::~CD3D11FlexFluidSimulation() {
	delete simData;
	delete scene;

	if (buffers.positions != nullptr) {
		NvFlexFreeBuffer(buffers.positions);
	}

	if (buffers.velocities != nullptr) {
		NvFlexFreeBuffer(buffers.velocities);
	}

	if (buffers.phases != nullptr) {
		NvFlexFreeBuffer(buffers.phases);
	}

	if (buffers.actives != nullptr) {
		NvFlexFreeBuffer(buffers.actives);
	}
}

void CD3D11FlexFluidSimulation::SetMaxParticles(const int maxParticles) {
	this->maxParticles = maxParticles;
	simData->SetMaxParticles(maxParticles);
}

void CD3D11FlexFluidSimulation::Initialize() {
	if (!context) {
		throw std::runtime_error(
			"CD3D11FlexFluidSimulation::Initialize: context must be set before "
			"initializing the simulation."
		);
	}

	if (!simData->IsBufferLinked(SimBufferType::POSITION)) {
		// We need to register our flex buffers at this point.
		throw std::runtime_error(
			"CD3D11FlexFluidSimulation::Initialize: position buffer must be "
			"linked before initializing the simulation."
		);
	}

	NvFlexInitDesc initDesc = {};
	initDesc.computeType = eNvFlexD3D11;
	initDesc.renderDevice =
		context->GetAPIHandle(SimContextHandle::D3D11_DEVICE);
	initDesc.enableExtensions = true;
	initDesc.runOnRenderContext = false;

	library = NvFlexInit(NV_FLEX_VERSION, FlexErrorCallback, &initDesc);

	NvFlexSolverDesc solverDesc = {};
	NvFlexSetSolverDescDefaults(&solverDesc);

	solverDesc.maxParticles = maxParticles;
	// soon...
	solverDesc.maxDiffuseParticles = 0;
	solverDesc.maxNeighborsPerParticle = 96;
	solverDesc.maxContactsPerParticle = 6;

	solver = NvFlexCreateSolver(library, &solverDesc);

	// flex params are massive so we set them up in a separate function
	SetupParams();
	NvFlexSetParams(solver, &solverParams);

	buffers.positions = NvFlexAllocBuffer(
		library, maxParticles, sizeof(FlexFloat4), eNvFlexBufferHost
	);

	buffers.velocities = NvFlexAllocBuffer(
		library, maxParticles, sizeof(FlexFloat3), eNvFlexBufferHost
	);

	buffers.phases = NvFlexAllocBuffer(
		library, maxParticles, sizeof(uint), eNvFlexBufferHost
	);

	buffers.actives = NvFlexAllocBuffer(
		library, maxParticles, sizeof(uint), eNvFlexBufferHost
	);

	sharedBuffers.positions = NvFlexRegisterD3DBuffer(
		library,
		simData->GetLinkedBuffer(SimBufferType::POSITION),
		maxParticles,
		sizeof(FlexFloat4)
	);

	scene = new CFlexSimScene(library, solver);
}

ISimData *CD3D11FlexFluidSimulation::GetSimulationData() { return simData; }
ISimScene *CD3D11FlexFluidSimulation::GetScene() { return scene; }
SimContextAPI CD3D11FlexFluidSimulation::GetComputeAPI() {
	return SimContextAPI::D3D11;
}

void CD3D11FlexFluidSimulation::AttachToContext(
	GellyObserverPtr<ISimContext> context
) {
	this->context = context;
}

ISimCommandList *CD3D11FlexFluidSimulation::CreateCommandList() {
	auto *commandList = new CSimpleSimCommandList(supportedCommands);
	commandLists.push_back(commandList);
	return commandList;
}

void CD3D11FlexFluidSimulation::DestroyCommandList(ISimCommandList *commandList
) {
	delete commandList;
}

void CD3D11FlexFluidSimulation::ExecuteCommandList(ISimCommandList *commandList
) {
	if (commandList == nullptr) {
		throw std::invalid_argument(
			"CD3D11FlexFluidSimulation::ExecuteCommandList: commandList must "
			"not be null."
		);
	}

	const auto iterators = commandList->GetCommands();

	bool mappingRequired = false;
	std::vector<FlexFloat3> newParticles;
	for (auto it = iterators.first; it != iterators.second; ++it) {
		auto &command = *it;
		std::visit(
			[&](auto &&arg) {
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, Reset>) {
					simData->SetActiveParticles(0);
				} else if constexpr (std::is_same_v<T, AddParticle>) {
					mappingRequired = true;
					newParticles.push_back(FlexFloat3{arg.x, arg.y, arg.z});
				}
			},
			command.data
		);
	}

	// batches the particle updates
	if (mappingRequired) {
		uint currentActiveParticles = simData->GetActiveParticles();
		uint newActiveParticles = currentActiveParticles + newParticles.size();

		auto *positions = reinterpret_cast<FlexFloat4 *>(
			NvFlexMap(buffers.positions, eNvFlexMapWait)
		);

		auto *velocities = reinterpret_cast<FlexFloat3 *>(
			NvFlexMap(buffers.velocities, eNvFlexMapWait)
		);

		auto *phases =
			reinterpret_cast<int *>(NvFlexMap(buffers.phases, eNvFlexMapWait));

		auto *actives =
			reinterpret_cast<uint *>(NvFlexMap(buffers.actives, eNvFlexMapWait)
			);

		for (uint i = currentActiveParticles; i < newActiveParticles; i++) {
			const auto &position = newParticles[i - currentActiveParticles];
			positions[i] = FlexFloat4{
				position.x, position.y, position.z, particleInverseMass
			};

			velocities[i] = FlexFloat3{0.f, 0.f, 0.f};
			phases[i] =
				NvFlexMakePhase(0, eNvFlexPhaseSelfCollide | eNvFlexPhaseFluid);

			actives[i] = i;	 // Literally just the particle index as we dont do
							 // any special active stuff yet.
		}

		NvFlexUnmap(buffers.positions);
		NvFlexUnmap(buffers.velocities);
		NvFlexUnmap(buffers.phases);
		NvFlexUnmap(buffers.actives);

		simData->SetActiveParticles(newActiveParticles);
	}
}

void CD3D11FlexFluidSimulation::Update(float deltaTime) {
	NvFlexSetParticles(solver, buffers.positions, nullptr);
	NvFlexSetVelocities(solver, buffers.velocities, nullptr);
	NvFlexSetPhases(solver, buffers.phases, nullptr);
	NvFlexSetActiveCount(solver, simData->GetActiveParticles());
	NvFlexSetActive(solver, buffers.actives, nullptr);
	NvFlexSetParams(solver, &solverParams);
	scene->Update();

	NvFlexUpdateSolver(solver, deltaTime, substeps, false);

	NvFlexGetParticles(solver, buffers.positions, nullptr);
	NvFlexGetVelocities(solver, buffers.velocities, nullptr);
	NvFlexGetPhases(solver, buffers.phases, nullptr);

	NvFlexGetSmoothParticles(solver, sharedBuffers.positions, nullptr);
}

void CD3D11FlexFluidSimulation::SetupParams() {
	solverParams.radius = particleRadius;
	solverParams.gravity[0] = 0.f;
	solverParams.gravity[1] = -9.8f;
	solverParams.gravity[2] = 0.f;

	solverParams.viscosity = 100.0f;
	solverParams.dynamicFriction = 0.1f;
	solverParams.staticFriction = 0.1f;
	solverParams.particleFriction = 0.1f;
	solverParams.freeSurfaceDrag = 0.0f;
	solverParams.drag = 0.0f;
	solverParams.lift = 0.0f;
	solverParams.numIterations = 3;
	// According to the manual, the ratio of radius and rest distance should be
	// 2:1
	solverParams.fluidRestDistance = solverParams.radius * 0.6f;
	solverParams.solidRestDistance = solverParams.radius * 2.f;

	solverParams.anisotropyScale = 1.0f;
	solverParams.anisotropyMin = 0.1f;
	solverParams.anisotropyMax = 2.0f;
	solverParams.smoothing = 1.0f;

	solverParams.dissipation = 0.0f;
	solverParams.damping = 0.0f;
	solverParams.particleCollisionMargin = 0.0f;
	solverParams.shapeCollisionMargin = 0.01f;
	solverParams.collisionDistance = solverParams.radius * 0.75f;
	solverParams.sleepThreshold = 0.0f;
	solverParams.shockPropagation = 0.0f;
	solverParams.restitution = 1.0f;

	solverParams.maxSpeed = FLT_MAX;
	solverParams.maxAcceleration = 100.0f;	// approximately 10x gravity

	solverParams.relaxationMode = eNvFlexRelaxationLocal;
	solverParams.relaxationFactor = 1.0f;
	solverParams.solidPressure = 1.0f;
	solverParams.adhesion = 0.0f;
	solverParams.cohesion = 0.02f;
	solverParams.surfaceTension = 0.0f;
	solverParams.vorticityConfinement = 15.0f;
	solverParams.buoyancy = 1.0f;
}
