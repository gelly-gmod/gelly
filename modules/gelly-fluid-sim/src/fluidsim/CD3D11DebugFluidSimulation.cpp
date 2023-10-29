#include "fluidsim/CD3D11DebugFluidSimulation.h"

CD3D11DebugFluidSimulation::CD3D11DebugFluidSimulation(int maxParticles)
	: maxParticles(maxParticles), simData(nullptr) {}

void CD3D11DebugFluidSimulation::GenerateRandomParticles() {
	if (!simData) {
		return;
	}

	SimFloat4 *positions = simData->MapBuffer(SimBuffer::Position);

	for (int i = 0; i < maxParticles; i++) {
		positions[i].x = static_cast<float>(rand()) / RAND_MAX;
		positions[i].y = static_cast<float>(rand()) / RAND_MAX;
		positions[i].z = static_cast<float>(rand()) / RAND_MAX;
		positions[i].w = 1.0f;
	}

	simData->UnmapBuffer(SimBuffer::Position);
}

CD3D11DebugFluidSimulation::~CD3D11DebugFluidSimulation() { delete simData; }

ISimData *CD3D11DebugFluidSimulation::GetSimulationData() { return simData; }

void CD3D11DebugFluidSimulation::AttachToContext(ISimContext *context) {
	delete simData;

	simData = new CD3D11CPUSimData(context);
	simData->Initialize(maxParticles);

	GenerateRandomParticles();
}

FluidSimCompute CD3D11DebugFluidSimulation::GetComputeType() {
	return FluidSimCompute::D3D11;
}

void CD3D11DebugFluidSimulation::Update(float deltaTime) {
	// Nothing!
}