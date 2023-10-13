#ifndef GELLY_PBFSOLVER_H
#define GELLY_PBFSOLVER_H

#include <GellyD3D.h>

#include "MathPOD.h"
#include "SolverContext.h"

using namespace gelly_fluid_math;

struct PBFSolverSettings {
	float radius;
	int maxParticles;
	int numIterations;
	float fluidRestDistance;
};

class PBFSolver {
private:
	PBFSolverSettings settings;
	SolverContext *context;

	d3d11::Buffer<float4> positions;
	d3d11::Buffer<float3> velocities;
	d3d11::Buffer<float> densities;

	int activeParticles;

public:
	PBFSolver(SolverContext *context, PBFSolverSettings params);
	~PBFSolver();

	void Update(float dt);
};

#endif	// GELLY_PBFSOLVER_H
