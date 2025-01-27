#pragma once
#include <d3d11.h>

#include "NvFlex.h"
#include "solver.h"

namespace gelly::simulation {

class Simulation {
public:
	struct CreateInfo {
		ID3D11Device *device;
		ID3D11DeviceContext *context;

		int maxParticles;
		int maxDiffuseParticles;
	};

	Simulation(const CreateInfo &createInfo);
	~Simulation();

	Solver &GetSolver() { return solver; }
	Solver *GetUnownedSolver() { return &solver; }

	void AttachOutputBuffers(
		const std::array<OutputD3DBuffers, renderer::splatting::MAX_FRAMES>
			&buffers
	);
	const char *GetComputeDeviceName() const {
		return NvFlexGetDeviceName(library);
	}

private:
	CreateInfo info;
	NvFlexLibrary *library;
	Solver solver;

	NvFlexLibrary *CreateLibrary() const;
	static void ErrorHandler(
		NvFlexErrorSeverity severity,
		const char *msg,
		const char *file,
		int line
	);
};

}  // namespace gelly::simulation
