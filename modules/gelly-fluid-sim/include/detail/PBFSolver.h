#ifndef GELLY_PBFSOLVER_H
#define GELLY_PBFSOLVER_H

#include <GellyD3D.h>

#include "SolverContext.h"

struct PBFSolverParams {
	float radius;
	int numIterations;
	float fluidRestDistance;
};

class PBFSolver {
private:
	PBFSolverParams settings;
	SolverContext *context;

public:
	PBFSolver(SolverContext *context, PBFSolverParams params);
	~PBFSolver();

	void Update(float dt);
};

#endif	// GELLY_PBFSOLVER_H
