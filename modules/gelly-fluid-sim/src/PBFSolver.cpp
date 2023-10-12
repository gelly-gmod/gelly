#include "detail/PBFSolver.h"

PBFSolver::PBFSolver(SolverContext *context, PBFSolverSettings params)
	: context(context), settings(params), activeParticles(0) {
	positions = context->CreateBuffer<float4>(settings.maxParticles);
	velocities = context->CreateBuffer<float3>(settings.maxParticles);
}