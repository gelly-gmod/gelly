#include "detail/PBFSolver.h"

PBFSolver::PBFSolver(SolverContext *context, PBFSolverSettings params)
	: context(context), settings(params), activeParticles(0) {
	positions = context->CreateBuffer<float4>(settings.maxParticles);
	velocities = context->CreateBuffer<float3>(settings.maxParticles);
	densities = context->CreateBuffer<float>(settings.maxParticles);

	ID3D11Buffer *buffer = positions.Get();
	// Map and set to random values for testing

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	DX("Failed to map particle buffer",
	   context->GetDeviceContext()->Map(
		   buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource
	   ));

	auto *data = (float4 *)mappedResource.pData;
	for (int i = 0; i < settings.maxParticles; i++) {
		data[i] = float4(
			(float)rand() / RAND_MAX - 0.5f,
			(float)rand() / RAND_MAX - 0.5f,
			(float)rand() / RAND_MAX - 0.5f,
			1.0f
		);
	}

	context->GetDeviceContext()->Unmap(buffer, 0);
}

ID3D11Buffer *PBFSolver::GetGPUPositions() const { return positions.Get(); }
ID3D11Buffer *PBFSolver::GetGPUVelocities() const { return velocities.Get(); }
ID3D11Buffer *PBFSolver::GetGPUDensities() const { return densities.Get(); }
int PBFSolver::GetNumActiveParticles() const { return activeParticles; }
