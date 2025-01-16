#pragma once
#include <d3d11.h>

#include "NvFlex.h"

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

private:
	CreateInfo info;
	NvFlexLibrary *library;
};

}  // namespace gelly::simulation
