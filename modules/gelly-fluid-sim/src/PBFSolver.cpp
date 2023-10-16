#include "detail/PBFSolver.h"

static const char *TIME_STEP_SHADER_SOURCE =
#include "shaders/d3d11/TimeStepPBF.cs.embed.hlsl"
	;

PBFSolver::PBFSolver(SolverContext *context, PBFSolverSettings params)
	: context(context),
	  settings(params),
	  activeParticles(0),
	  timeStepProgram(context->CreateComputeProgram(
		  "TimeStepPBF", "main", TIME_STEP_SHADER_SOURCE
	  )),
	  timeStepLayout({}) {
	positions = context->CreateBuffer<float4>(settings.maxParticles);
	velocities = context->CreateBuffer<float3>(settings.maxParticles);
	densities = context->CreateBuffer<float>(settings.maxParticles);

	positionsUAV = d3d11::UAVBuffer<float4>(
		context->GetDevice(), DXGI_FORMAT_R32G32B32A32_FLOAT, positions
	);

	timeStepLayout.views[0] = positionsUAV.Get();
	timeStepLayout.numViews = 1;

	timeStepLayout.numResources = 0;
	timeStepLayout.constantBuffer = nullptr;
}

void PBFSolver::Update(float dt) {
	// The time step program runs in blocks of 3x3x3 threads.

	timeStepLayout.numThreadsX = activeParticles / 27 + 1;
	timeStepLayout.numThreadsY = 1;
	timeStepLayout.numThreadsZ = 1;
	timeStepProgram.Run(context->GetDeviceContext(), timeStepLayout);
}

ID3D11Buffer *PBFSolver::GetGPUPositions() const { return positions.Get(); }
ID3D11Buffer *PBFSolver::GetGPUVelocities() const { return velocities.Get(); }
ID3D11Buffer *PBFSolver::GetGPUDensities() const { return densities.Get(); }
int PBFSolver::GetNumActiveParticles() const { return activeParticles; }
