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
constexpr auto VELOCITY_FRAMES = 5;

struct SolverBufferSet {
	FlexBuffer<DirectX::XMFLOAT4> positions;
	FlexBuffer<DirectX::XMFLOAT3> velocities;
	FlexBuffer<int> phases;
	FlexBuffer<int> actives;
	FlexBuffer<int> diffuseParticleCount;

	SolverBufferSet(NvFlexLibrary *library, int maxParticles) :
		positions({.library = library, .maxElements = maxParticles}),
		velocities({.library = library, .maxElements = maxParticles}),
		phases({.library = library, .maxElements = maxParticles}),
		actives({.library = library, .maxElements = maxParticles}),
		diffuseParticleCount({.library = library, .maxElements = 1}) {}

	SolverBufferSet() = default;
};

struct OutputD3DBuffers {
	ID3D11Buffer *smoothedPositions;
	ID3D11Buffer *velocities[VELOCITY_FRAMES];
	ID3D11Buffer *anisotropyQ1;
	ID3D11Buffer *anisotropyQ2;
	ID3D11Buffer *anisotropyQ3;
	ID3D11Buffer *foamPositions;
	ID3D11Buffer *foamVelocities;
};

struct OutputBuffers {
	FlexGpuBuffer<DirectX::XMFLOAT4> smoothedPositions;
	FlexGpuBuffer<DirectX::XMFLOAT3> velocities[VELOCITY_FRAMES];
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
		velocities({}),
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
		}) {
		for (int i = 0; i < VELOCITY_FRAMES; ++i) {
			velocities[i] = FlexGpuBuffer<DirectX::XMFLOAT3>({
				.library = info.library,
				.buffer = info.rendererBuffers.velocities[i],
				.maxElements = info.maxParticles,
			});
		}
	}

	OutputBuffers() :
		smoothedPositions(),
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
	float invMass;
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
		OPTIONAL_PARAM(anisotropyMin, float);
		OPTIONAL_PARAM(anisotropyMax, float);
	};

	Solver(const CreateInfo &createInfo);
	~Solver();

	void BeginTick(float dt);
	void EndTick();
	void AddParticles(const ParticleBatch &particles);
	void Update(const UpdateSolverInfo &info);
	void Reset();

	int GetActiveParticleCount() const;
	int GetCurrentActiveParticleCount() const;
	int GetActiveDiffuseParticleCount() const;
	int GetMaxParticles() const;
	int GetMaxDiffuseParticles() const;

	float GetLastDeltaTime() const { return lastDeltaTime; }

	bool IsWhitewaterEnabled() const { return info.maxDiffuseParticles > 0; }

	Scene &GetScene() { return scene; }
	Scene *GetUnownedScene() { return &scene; }

	void AttachOutputBuffers(const OutputD3DBuffers &buffers);
	void SetTimeStepMultiplier(float multiplier) {
		timeStepMultiplier = multiplier;
		params.diffuseLifetime = diffuseLifetime * timeStepMultiplier;
	}

private:
	CreateInfo info;
	NvFlexSolver *solver;
	NvFlexParams params;
	Scene scene;

	float timeStepMultiplier = 1.f;
	int activeParticleCount = 0;
	int activeDiffuseParticleCount = 0;
	int newActiveParticleCount = 0;
	int particleCountAtBeginTick = 0;
	int velocityFrameIndex = 0;

	int substeps = 3;
	// Tracked because it must scale with the time step multiplier
	float diffuseLifetime = 0.f;
	float lastDeltaTime = 0.f;

	SolverBufferSet buffers;
	OutputBuffers outputBuffers;

	NvFlexSolver *CreateSolver() const;
	Scene CreateScene() const;
	void SetupDefaultParams();
};

}  // namespace gelly::simulation
