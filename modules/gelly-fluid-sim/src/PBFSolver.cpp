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
}

ID3D11Buffer *PBFSolver::GetGPUPositions() const { return positions.Get(); }
ID3D11Buffer *PBFSolver::GetGPUVelocities() const { return velocities.Get(); }
ID3D11Buffer *PBFSolver::GetGPUDensities() const { return densities.Get(); }
int PBFSolver::GetNumActiveParticles() const { return activeParticles; }
