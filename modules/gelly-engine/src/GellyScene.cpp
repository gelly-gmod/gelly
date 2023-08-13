#include "GellyScene.h"
#include <memory>

GellyScene::GellyScene(NvFlexLibrary *library, int maxParticles) :
    gpuWork(false),
    maxParticles(maxParticles),
    currentParticleCount(0),
    solver(nullptr),
    library(library),
    params(nullptr),
    positions(library, maxParticles),
    velocities(library, maxParticles),
    phases(library, maxParticles),
    activeIndices(library, maxParticles)
{
    NvFlexSolverDesc solverDesc{};
    solverDesc.maxParticles = maxParticles;
    solverDesc.maxNeighborsPerParticle = 128;

    solver = NvFlexCreateSolver(library, &solverDesc);

    auto defaultParams = new NvFlexParams();
    defaultParams->gravity[0] = 0.0f;
    defaultParams->gravity[1] = -1.8f;
    defaultParams->gravity[2] = 0.0f;
    defaultParams->radius = 0.2f;
    defaultParams->numIterations = 3;
    defaultParams->collisionDistance = defaultParams->radius;
    defaultParams->maxSpeed = FLT_MAX;
    defaultParams->maxAcceleration = 128.0f;
    defaultParams->surfaceTension = 2.f;
    defaultParams->viscosity = 10.f;
    defaultParams->adhesion = 0.01f;
    defaultParams->cohesion = 1.1f;
    defaultParams->fluidRestDistance = 0.1f;
    defaultParams->solidRestDistance = 0.1f;
    defaultParams->lift = 1.0f;
    defaultParams->drag = 10.0f;
    defaultParams->buoyancy = 1.0f;

    params = defaultParams;
    NvFlexSetParams(solver, params);
}

GellyScene::~GellyScene() {
    NvFlexDestroySolver(solver);
    delete params;
}

void GellyScene::EnterGPUWork() {
    if (gpuWork) {
        return;
    }

    gpuWork = true;

    positions.map();
    velocities.map();
    phases.map();
    activeIndices.map();
}

void GellyScene::ExitGPUWork() {
    if (!gpuWork) {
        return;
    }

    gpuWork = false;

    positions.unmap();
    velocities.unmap();
    phases.unmap();
    activeIndices.unmap();
}

void GellyScene::Update(float deltaTime) {
    NvFlexCopyDesc copyDesc{};
    copyDesc.dstOffset = 0;
    copyDesc.srcOffset = 0;
    copyDesc.elementCount = maxParticles;

    NvFlexSetParticles(solver, positions.buffer, &copyDesc);
    NvFlexSetVelocities(solver, velocities.buffer, &copyDesc);
    NvFlexSetPhases(solver, phases.buffer, &copyDesc);
    NvFlexSetActive(solver, activeIndices.buffer, &copyDesc);
    NvFlexSetActiveCount(solver, maxParticles);

    NvFlexUpdateSolver(solver, deltaTime, 2, false);

    NvFlexGetParticles(solver, positions.buffer, &copyDesc);
    NvFlexGetVelocities(solver, velocities.buffer, &copyDesc);
    NvFlexGetPhases(solver, phases.buffer, &copyDesc);

    NvFlexFlush(library);
}

void GellyScene::AddParticle(Vec4 position, Vec3 velocity) {
    if (!gpuWork) {
        return;
    }

    if (currentParticleCount >= maxParticles) {
        return;
    }

    positions[currentParticleCount] = position;
    velocities[currentParticleCount] = velocity;
    phases[currentParticleCount] = NvFlexMakePhase(0, eNvFlexPhaseSelfCollide | eNvFlexPhaseFluid);
    activeIndices[currentParticleCount] = currentParticleCount++;
}

Vec4* GellyScene::GetPositions() const {
    if (!gpuWork) {
        return nullptr;
    }

    return positions.mappedPtr;
}

Vec3* GellyScene::GetVelocities() const {
    if (!gpuWork) {
        return nullptr;
    }

    return velocities.mappedPtr;
}

int GellyScene::GetCurrentParticleCount() const {
    return currentParticleCount;
}
