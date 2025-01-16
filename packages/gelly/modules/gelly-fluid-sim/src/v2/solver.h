#pragma once
#include <DirectXMath.h>

#include <vector>

#include "NvFlex.h"
#include "helpers/flex-buffer.h"
#include "helpers/flex-gpu-buffer.h"

namespace gelly::simulation {
using namespace helpers;

struct SolverBufferSet {
	FlexBuffer<DirectX::XMFLOAT4> positions;
	FlexBuffer<DirectX::XMFLOAT4> velocities;
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
};

class Solver {
public:
	struct CreateInfo {
		NvFlexLibrary *library;

		int maxParticles;
		int maxDiffuseParticles;

		OutputD3DBuffers rendererBuffers;
	};

	Solver(const CreateInfo &createInfo);
	~Solver();

	void Tick(float dt);

private:
	CreateInfo info;
	NvFlexSolver *solver;
	NvFlexParams params;
	float timeStepMultiplier = 1.f;

	SolverBufferSet buffers;
	OutputBuffers outputBuffers;

	NvFlexParams CreateDefaultParams();
	NvFlexSolver *CreateSolver() const;
};

}  // namespace gelly::simulation
