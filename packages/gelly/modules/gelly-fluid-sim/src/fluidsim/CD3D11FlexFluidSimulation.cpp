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
	printf("FlexErrorCallback: %s - %s:%d\n", msg, file, line);
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

	if (buffers.contactCounts != nullptr) {
		NvFlexFreeBuffer(buffers.contactCounts);
	}

	if (buffers.contactVelocities != nullptr) {
		NvFlexFreeBuffer(buffers.contactVelocities);
	}

	NvFlexDestroySolver(solver);
	NvFlexShutdown(library);
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
#ifdef _DEBUG
	// FleX will try to use GPU extensions and compute queues which destroy most
	// rendering debuggers
	initDesc.enableExtensions = false;
	initDesc.runOnRenderContext = true;
#else
	initDesc.enableExtensions = false;
	initDesc.runOnRenderContext = true;
#endif

	library = NvFlexInit(NV_FLEX_VERSION, FlexErrorCallback, &initDesc);

	NvFlexSolverDesc solverDesc = {};
	NvFlexSetSolverDescDefaults(&solverDesc);

	solverDesc.maxParticles = maxParticles;
	// soon...
	solverDesc.maxDiffuseParticles = 0;
	solverDesc.maxNeighborsPerParticle = 64;
	solverDesc.maxContactsPerParticle = maxContactsPerParticle;

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

	buffers.contactVelocities = NvFlexAllocBuffer(
		library,
		maxParticles * maxContactsPerParticle,
		sizeof(FlexFloat4),
		eNvFlexBufferHost
	);

	buffers.contactCounts = NvFlexAllocBuffer(
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
	std::vector<AddParticle> newParticles;

	for (auto it = iterators.first; it != iterators.second; ++it) {
		auto &command = *it;
		std::visit(
			[&](auto &&arg) {
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, Reset>) {
					simData->SetActiveParticles(0);
				} else if constexpr (std::is_same_v<T, AddParticle>) {
					mappingRequired = true;
					newParticles.push_back(arg);
				} else if constexpr (std::is_same_v<T, SetFluidProperties>) {
					solverParams.adhesion = arg.adhesion;
					solverParams.cohesion = arg.cohesion;
					solverParams.surfaceTension = arg.surfaceTension;
					solverParams.vorticityConfinement =
						arg.vorticityConfinement;
					solverParams.viscosity = arg.viscosity;
					DebugDumpParams();
				} else if constexpr (std::is_same_v<T, ChangeRadius>) {
					particleRadius = arg.radius;
					SetupParams();
				}
			},
			command.data
		);
	}

	// batches the particle updates
	if (mappingRequired) {
		uint currentActiveParticles = simData->GetActiveParticles();
		uint newActiveParticles = currentActiveParticles + newParticles.size();

		// Update the positions and velocities of the particles
		NvFlexGetParticles(solver, buffers.positions, nullptr);
		NvFlexGetVelocities(solver, buffers.velocities, nullptr);
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
			_mm_prefetch(
				reinterpret_cast<const char *>(
					&newParticles[i - currentActiveParticles] + 1
				),
				_MM_HINT_T0
			);

			const auto &position = newParticles[i - currentActiveParticles];
			positions[i] = FlexFloat4{
				position.x, position.y, position.z, particleInverseMass
			};

			velocities[i] = FlexFloat3{position.vx, position.vy, position.vz};
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

		NvFlexCopyDesc copyDesc = {};
		copyDesc.dstOffset = 0;
		copyDesc.srcOffset = 0;
		copyDesc.elementCount = simData->GetActiveParticles();

		NvFlexSetParticles(solver, buffers.positions, &copyDesc);
		NvFlexSetVelocities(solver, buffers.velocities, &copyDesc);
		NvFlexSetPhases(solver, buffers.phases, &copyDesc);
		NvFlexSetActive(solver, buffers.actives, &copyDesc);
	}
}

void CD3D11FlexFluidSimulation::Update(float deltaTime) {
	NvFlexCopyDesc copyDesc = {};
	copyDesc.dstOffset = 0;
	copyDesc.srcOffset = 0;
	copyDesc.elementCount = simData->GetActiveParticles();

	NvFlexSetParams(solver, &solverParams);
	NvFlexSetActiveCount(solver, simData->GetActiveParticles());

	NvFlexUpdateSolver(solver, deltaTime, substeps, false);
	NvFlexGetSmoothParticles(solver, sharedBuffers.positions, &copyDesc);
}

void CD3D11FlexFluidSimulation::SetupParams() {
	DebugDumpParams();
	// Rule of thumb is to use proportional values to the particle radius, as
	// the radius is really what determines the properties of the fluid.
	solverParams.radius = particleRadius;
	solverParams.gravity[0] = 0.f;
	solverParams.gravity[1] = 0.f;
	solverParams.gravity[2] = -4.f;

	solverParams.viscosity = 0.0f;
	solverParams.dynamicFriction = 0.1f;
	solverParams.staticFriction = 0.1f;
	solverParams.particleFriction = 0.1f;
	solverParams.freeSurfaceDrag = 0.0f;
	solverParams.drag = 0.0f;
	solverParams.lift = 0.0f;
	solverParams.numIterations = 3;
	// According to the manual, the ratio of radius and rest distance should be
	// 2:1
	solverParams.fluidRestDistance = solverParams.radius * 0.73f;
	solverParams.solidRestDistance = solverParams.radius * 2.13f;

	solverParams.anisotropyScale = 1.0f;
	solverParams.anisotropyMin = 0.1f;
	solverParams.anisotropyMax = 2.0f;
	solverParams.smoothing = 2.2f;

	solverParams.dissipation = 0.0f;
	solverParams.damping = 0.0f;
	solverParams.particleCollisionMargin = 1.f;
	solverParams.shapeCollisionMargin = 1.f;
	solverParams.collisionDistance = solverParams.fluidRestDistance * 0.5f;
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
	solverParams.surfaceTension = 1.0f;
	solverParams.vorticityConfinement = 1.0f;
	solverParams.buoyancy = 1.0f;
	printf("== NEW PARAMS ==\n");
	DebugDumpParams();
}

void CD3D11FlexFluidSimulation::DebugDumpParams() {
	printf("== DEBUG PARAM DUMP ==\n");
	printf("\tradius: %f\n", solverParams.radius);
	printf(
		"\tgravity: %f, %f, %f\n",
		solverParams.gravity[0],
		solverParams.gravity[1],
		solverParams.gravity[2]
	);
	printf("\tviscosity: %f\n", solverParams.viscosity);
	printf("\tdynamicFriction: %f\n", solverParams.dynamicFriction);
	printf("\tstaticFriction: %f\n", solverParams.staticFriction);
	printf("\tparticleFriction: %f\n", solverParams.particleFriction);
	printf("\tfreeSurfaceDrag: %f\n", solverParams.freeSurfaceDrag);
	printf("\tdrag: %f\n", solverParams.drag);
	printf("\tlift: %f\n", solverParams.lift);
	printf("\tnumIterations: %d\n", solverParams.numIterations);
	printf("\tfluidRestDistance: %f\n", solverParams.fluidRestDistance);
	printf("\tsolidRestDistance: %f\n", solverParams.solidRestDistance);
	printf("\tanisotropyScale: %f\n", solverParams.anisotropyScale);
	printf("\tanisotropyMin: %f\n", solverParams.anisotropyMin);
	printf("\tanisotropyMax: %f\n", solverParams.anisotropyMax);
	printf("\tsmoothing: %f\n", solverParams.smoothing);
	printf("\tdissipation: %f\n", solverParams.dissipation);
	printf("\tdamping: %f\n", solverParams.damping);
	printf(
		"\tparticleCollisionMargin: %f\n", solverParams.particleCollisionMargin
	);
	printf("\tshapeCollisionMargin: %f\n", solverParams.shapeCollisionMargin);
	printf("\tcollisionDistance: %f\n", solverParams.collisionDistance);
	printf("\tsleepThreshold: %f\n", solverParams.sleepThreshold);
	printf("\tshockPropagation: %f\n", solverParams.shockPropagation);
	printf("\trestitution: %f\n", solverParams.restitution);
	printf("\tmaxSpeed: %f\n", solverParams.maxSpeed);
	printf("\tmaxAcceleration: %f\n", solverParams.maxAcceleration);
	printf("\trelaxationMode: %d\n", solverParams.relaxationMode);
	printf("\trelaxationFactor: %f\n", solverParams.relaxationFactor);
	printf("\tsolidPressure: %f\n", solverParams.solidPressure);
	printf("\tadhesion: %f\n", solverParams.adhesion);
	printf("\tcohesion: %f\n", solverParams.cohesion);
	printf("\tsurfaceTension: %f\n", solverParams.surfaceTension);
	printf("\tvorticityConfinement: %f\n", solverParams.vorticityConfinement);
	printf("\tbuoyancy: %f\n", solverParams.buoyancy);
	printf("== END DEBUG PARAM DUMP ==\n");
}

const char *CD3D11FlexFluidSimulation::GetComputeDeviceName() {
	return NvFlexGetDeviceName(library);
}

bool CD3D11FlexFluidSimulation::CheckFeatureSupport(GELLY_FEATURE feature) {
	switch (feature) {
		case GELLY_FEATURE::FLUIDSIM_CONTACTPLANES:
			return true;
		default:
			return false;
	}
}

void CD3D11FlexFluidSimulation::VisitLatestContactPlanes(
	ContactPlaneVisitor visitor
) {
	NvFlexGetContacts(
		solver,
		nullptr,
		buffers.contactVelocities,
		nullptr,
		buffers.contactCounts
	);
	NvFlexGetParticles(solver, buffers.positions, nullptr);

	const auto *velocities = static_cast<FlexFloat4 *>(
		NvFlexMap(buffers.contactVelocities, eNvFlexMapWait)
	);

	const auto *counts =
		static_cast<int *>(NvFlexMap(buffers.contactCounts, eNvFlexMapWait));

	for (uint i = 0; i < simData->GetActiveParticles(); i++) {
		const int contactCount = counts[i];

		for (int contactIndex = 0; contactIndex < contactCount;
			 contactIndex++) {
			const FlexFloat4 velocity =
				velocities[i * maxContactsPerParticle + contactIndex];
			uint shapeIndex = static_cast<uint>(velocity.w);

			ObjectHandle handle = scene->GetHandleFromShapeIndex(shapeIndex);
			XMFLOAT3 velocityVector =
				XMFLOAT3(velocity.x, velocity.y, velocity.z);

			XMStoreFloat3(
				&velocityVector,
				XMVectorScale(XMLoadFloat3(&velocityVector), 25.f)
			);

			if (visitor(velocityVector, handle)) {
				break;
			}
		}
	}

	NvFlexUnmap(buffers.contactVelocities);
	NvFlexUnmap(buffers.contactCounts);
}