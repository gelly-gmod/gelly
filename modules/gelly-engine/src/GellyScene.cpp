#include "MeshConvert.h"
#include "GellyScene.h"
#include <memory>

GellyScene::GellyScene(NvFlexLibrary *library, int maxParticles, int maxColliders) :
        maxParticles(maxParticles),
        currentParticleCount(0),
        solver(nullptr),
        library(library),
        params(nullptr),
        positions(library, maxParticles),
        velocities(library, maxParticles),
        phases(library, maxParticles),
        activeIndices(library, maxParticles),
        colliders(library, maxColliders) {
    gpuWork = false;

    NvFlexSolverDesc solverDesc{};
    NvFlexSetSolverDescDefaults(&solverDesc);
    solverDesc.maxParticles = maxParticles;
    solverDesc.maxNeighborsPerParticle = 128;

    solver = NvFlexCreateSolver(library, &solverDesc);

    auto defaultParams = new NvFlexParams();
    defaultParams->gravity[0] = 0.0f;
    defaultParams->gravity[1] = -1.8f;
    defaultParams->gravity[2] = 0.0f;
    defaultParams->radius = 0.2f;
    defaultParams->numIterations = 4;
    defaultParams->collisionDistance = 0.1f;
    defaultParams->particleCollisionMargin = 0.01f;
    defaultParams->shapeCollisionMargin = 0.01f;
    defaultParams->dynamicFriction = 0.25f;
    defaultParams->staticFriction = 0.25f;
    defaultParams->maxSpeed = FLT_MAX;
    defaultParams->maxAcceleration = 128.0f;
    defaultParams->surfaceTension = 2.f;
    defaultParams->viscosity = 10.f;
    defaultParams->adhesion = 0.01f;
    defaultParams->cohesion = 0.1f;
    defaultParams->fluidRestDistance = 0.1f;
    defaultParams->solidRestDistance = 0.1f;
    defaultParams->lift = 1.0f;
    defaultParams->drag = 1.0f;
    defaultParams->buoyancy = 1.0f;
    defaultParams->relaxationMode = eNvFlexRelaxationGlobal;
    defaultParams->relaxationFactor = 0.01f;
    defaultParams->numPlanes = 0;
    defaultParams->solidPressure = 1.f;
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
    colliders.EnterGPUWork();
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
    colliders.ExitGPUWork();
}

void GellyScene::Update(float deltaTime) {
    NvFlexCopyDesc copyDesc{};
    copyDesc.dstOffset = 0;
    copyDesc.srcOffset = 0;
    copyDesc.elementCount = currentParticleCount + 1;

    NvFlexSetParticles(solver, positions.buffer, &copyDesc);
    NvFlexSetVelocities(solver, velocities.buffer, &copyDesc);
    NvFlexSetPhases(solver, phases.buffer, &copyDesc);
    NvFlexSetActive(solver, activeIndices.buffer, &copyDesc);
    NvFlexSetActiveCount(solver, currentParticleCount);
    NvFlexSetShapes(
            solver,
            colliders.geometries.buffer,
            colliders.positions.buffer,
            colliders.rotations.buffer,
            colliders.prevPositions.buffer,
            colliders.prevRotations.buffer,
            colliders.flags.buffer,
            colliders.GetEntityCount());
    NvFlexSetParams(solver, params);
    NvFlexUpdateSolver(solver, deltaTime, 2, false);

    NvFlexGetParticles(solver, positions.buffer, &copyDesc);
    NvFlexGetVelocities(solver, velocities.buffer, &copyDesc);
    NvFlexGetPhases(solver, phases.buffer, &copyDesc);
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

Vec4 *GellyScene::GetPositions() const {
    if (!gpuWork) {
        return nullptr;
    }

    return positions.mappedPtr;
}

Vec3 *GellyScene::GetVelocities() const {
    if (!gpuWork) {
        return nullptr;
    }

    return velocities.mappedPtr;
}

int GellyScene::GetCurrentParticleCount() const {
    return currentParticleCount;
}

void GellyScene::AddBSP(const std::string &mapName, uint8_t *data, size_t dataSize) {
    if (!gpuWork) {
        return;
    }

    auto info = MeshConvert_LoadBSP(data, dataSize);
    colliders.AddTriangleMesh(mapName, info);
    MeshConvert_FreeBSP(info);

    colliders.AddEntity({
        .position = Vec3{0, 0, 0},
        .rotation = Quat{0, 0, 0, 1},
        .modelPath = mapName
    });
}
