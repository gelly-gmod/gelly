#pragma once
#include <DirectXMath.h>

#include <optional>
#include <vector>

#include "NvFlex.h"
#include "helpers/flex-buffer.h"
#include "helpers/flex-gpu-buffer.h"
#include "scene/scene.h"

#define OPTIONAL_PARAM(name, type) std::optional<type> name = std::nullopt;

namespace gelly::simulation {
using namespace helpers;

struct SolverBufferSet {
	FlexBuffer<DirectX::XMFLOAT4> positions;
	FlexBuffer<DirectX::XMFLOAT3> velocities;
	FlexBuffer<int> phases;
	FlexBuffer<int> actives;

	SolverBufferSet(NvFlexLibrary *library, int maxParticles) :
		positions({.library = library, .maxElements = maxParticles}),
		velocities({.library = library, .maxElements = maxParticles}),
		phases({.library = library, .maxElements = maxParticles}),
		actives({.library = library, .maxElements = maxParticles}) {}

	SolverBufferSet() = default;
};

struct OutputD3DBuffers {
	ID3D11Buffer *smoothedPositions;
	ID3D11Buffer *velocitiesPrevFrame;
	ID3D11Buffer *velocities;
	ID3D11Buffer *anisotropyQ1;
	ID3D11Buffer *anisotropyQ2;
	ID3D11Buffer *anisotropyQ3;
	ID3D11Buffer *foamPositions;
	ID3D11Buffer *foamVelocities;
};

struct OutputBuffers {
	FlexGpuBuffer<DirectX::XMFLOAT4> smoothedPositions;
	FlexGpuBuffer<DirectX::XMFLOAT3> velocitiesPrevFrame;
	FlexGpuBuffer<DirectX::XMFLOAT3> velocities;
	FlexGpuBuffer<DirectX::XMFLOAT4> anisotropyQ1;
	FlexGpuBuffer<DirectX::XMFLOAT4> anisotropyQ2;
	FlexGpuBuffer<DirectX::XMFLOAT4> anisotropyQ3;
	FlexGpuBuffer<DirectX::XMFLOAT4> foamPositions;
	FlexGpuBuffer<DirectX::XMFLOAT4> foamVelocities;

	struct CreateInfo {
		OutputD3DBuffers rendererBuffers;
		NvFlexLibrary *library;
		int maxParticles;
		int maxDiffuseParticles;
	};

	OutputBuffers(const CreateInfo &info) :
		smoothedPositions({
			.library = info.library,
			.buffer = info.rendererBuffers.smoothedPositions,
			.maxElements = info.maxParticles,
		}),
		velocitiesPrevFrame({
			.library = info.library,
			.buffer = info.rendererBuffers.velocitiesPrevFrame,
			.maxElements = info.maxParticles,
		}),
		velocities({
			.library = info.library,
			.buffer = info.rendererBuffers.velocities,
			.maxElements = info.maxParticles,
		}),
		anisotropyQ1({
			.library = info.library,
			.buffer = info.rendererBuffers.anisotropyQ1,
			.maxElements = info.maxParticles,
		}),
		anisotropyQ2({
			.library = info.library,
			.buffer = info.rendererBuffers.anisotropyQ2,
			.maxElements = info.maxParticles,
		}),
		anisotropyQ3({
			.library = info.library,
			.buffer = info.rendererBuffers.anisotropyQ3,
			.maxElements = info.maxParticles,
		}),
		foamPositions({
			.library = info.library,
			.buffer = info.rendererBuffers.foamPositions,
			.maxElements = info.maxDiffuseParticles,
		}),
		foamVelocities({
			.library = info.library,
			.buffer = info.rendererBuffers.foamVelocities,
			.maxElements = info.maxDiffuseParticles,
		}) {}

	OutputBuffers() :
		smoothedPositions(),
		velocitiesPrevFrame(),
		velocities(),
		anisotropyQ1(),
		anisotropyQ2(),
		anisotropyQ3(),
		foamPositions(),
		foamVelocities() {}
};

struct ParticleUploadInfo {
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 velocity;
};

using ParticleBatch = std::vector<ParticleUploadInfo>;

class Solver {
public:
	struct CreateInfo {
		NvFlexLibrary *library;

		int maxParticles;
		int maxDiffuseParticles;
	};

	struct UpdateSolverInfo {
		OPTIONAL_PARAM(radius, float);
		OPTIONAL_PARAM(viscosity, float);
		OPTIONAL_PARAM(cohesion, float);
		OPTIONAL_PARAM(surfaceTension, float);
		OPTIONAL_PARAM(vorticityConfinement, float);
		OPTIONAL_PARAM(adhesion, float);
		OPTIONAL_PARAM(dynamicFriction, float);
		OPTIONAL_PARAM(restDistanceRatio, float);
		OPTIONAL_PARAM(diffuseBallisticCount, float);
		OPTIONAL_PARAM(diffuseKineticThreshold, float);
		OPTIONAL_PARAM(diffuseBuoyancy, float);
		OPTIONAL_PARAM(diffuseDrag, float);
		OPTIONAL_PARAM(diffuseLifetime, float);
		OPTIONAL_PARAM(substeps, int);
		OPTIONAL_PARAM(iterations, int);
		OPTIONAL_PARAM(relaxationFactor, float);
		OPTIONAL_PARAM(collisionDistance, float);
		OPTIONAL_PARAM(gravity, float);
		OPTIONAL_PARAM(enableWhitewater, bool);
	};

	Solver(const CreateInfo &createInfo);
	~Solver();

	void Tick(float dt);
	void AddParticles(const ParticleBatch &particles);
	void Update(const UpdateSolverInfo &info);
	void Reset();

	int GetActiveParticleCount() const;
	int GetCurrentActiveParticleCount() const;
	int GetMaxParticles() const;
	int GetMaxDiffuseParticles() const;
	Scene &GetScene() { return scene; }
	Scene *GetUnownedScene() { return &scene; }

	void AttachOutputBuffers(const OutputD3DBuffers &buffers);
	void SetTimeStepMultiplier(float multiplier) {
		timeStepMultiplier = multiplier;
		params.diffuseLifetime = params.diffuseLifetime * timeStepMultiplier;
	}

private:
	CreateInfo info;
	NvFlexSolver *solver;
	NvFlexParams params;
	Scene scene;

	float timeStepMultiplier = 1.f;
	int activeParticleCount = 0;
	int newActiveParticleCount = 0;

	int substeps = 3;

	SolverBufferSet buffers;
	OutputBuffers outputBuffers;

	NvFlexParams CreateDefaultParams();
	NvFlexSolver *CreateSolver() const;
	Scene CreateScene() const;
};

}  // namespace gelly::simulation
