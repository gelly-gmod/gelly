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
    solverDesc.maxParticles = maxParticles;
    solverDesc.maxNeighborsPerParticle = 128;

    solver = NvFlexCreateSolver(library, &solverDesc);

    auto defaultParams = new NvFlexParams();
    defaultParams->gravity[0] = 0.0f;
    defaultParams->gravity[1] = -1.8f;
    defaultParams->gravity[2] = 0.0f;
    defaultParams->radius = 0.2f;
    defaultParams->numIterations = 4;
    defaultParams->collisionDistance = 21.f;
    defaultParams->particleCollisionMargin = 1.f;
    defaultParams->shapeCollisionMargin = 1.f;
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

void Colliders::EnterGPUWork() {
    if (gpuWork) {
        return;
    }

    gpuWork = true;
    geometries.map();
    positions.map();
    rotations.map();
    prevPositions.map();
    prevRotations.map();
    flags.map();
}

void Colliders::ExitGPUWork() {
    if (!gpuWork) {
        return;
    }

    gpuWork = false;
    geometries.unmap();
    positions.unmap();
    rotations.unmap();
    prevPositions.unmap();
    prevRotations.unmap();
    flags.unmap();
}

Colliders::Colliders(NvFlexLibrary *library, int maxColliders) :
        library(library),
        geometries(library, maxColliders),
        positions(library, maxColliders),
        rotations(library, maxColliders),
        prevPositions(library, maxColliders),
        prevRotations(library, maxColliders),
        flags(library, maxColliders) {
    gpuWork = false;
}

MeshID Colliders::AddTriangleMesh(const MeshUploadInfo &info) {
    auto meshId = NvFlexCreateTriangleMesh(library);

    NvFlexBuffer* verticesBuffer = NvFlexAllocBuffer(library, info.vertexCount, sizeof(Vec4), eNvFlexBufferHost);
    NvFlexBuffer* indicesBuffer = NvFlexAllocBuffer(library, info.indexCount, sizeof(int), eNvFlexBufferHost);

    Vec4* vertices = static_cast<Vec4 *>(NvFlexMap(verticesBuffer, eNvFlexMapWait));
    int* indices = static_cast<int *>(NvFlexMap(indicesBuffer, eNvFlexMapWait));

    for (int i = 0; i < info.vertexCount; i++) {
        vertices[i] = Vec4{info.vertices[i].x, info.vertices[i].y, info.vertices[i].z, 0.5};
    }

    memcpy(indices, info.indices, sizeof(int) * info.indexCount);

    NvFlexUnmap(verticesBuffer);
    NvFlexUnmap(indicesBuffer);

    NvFlexUpdateTriangleMesh(library, meshId, verticesBuffer, indicesBuffer, info.vertexCount, info.vertexCount / 3,
                             reinterpret_cast<const float *>(&info.lower), reinterpret_cast<const float *>(&info.upper));

    return meshId;
}

void Colliders::AddEntity(GellyEntity entity) {
    entities.push_back(entity);
}

int Colliders::GetEntityCount() const {
    return static_cast<int>(entities.size());
}

void Colliders::Update() {
    if (!gpuWork) {
        return;
    }

    for (int entityIndex = 0; entityIndex < entities.size(); entityIndex++) {
        const auto& entity = entities[entityIndex];

        geometries[entityIndex].triMesh.mesh = entity.mesh;
        geometries[entityIndex].triMesh.scale[0] = 1.0f;
        geometries[entityIndex].triMesh.scale[1] = 1.0f;
        geometries[entityIndex].triMesh.scale[2] = 1.0f;

        prevPositions[entityIndex] = positions[entityIndex];
        prevRotations[entityIndex] = rotations[entityIndex];

        positions[entityIndex] = Vec4{entity.position.x, entity.position.y, entity.position.z, 0};
        rotations[entityIndex] = entity.rotation;

        flags[entityIndex] = NvFlexMakeShapeFlags(eNvFlexShapeTriangleMesh, false);
    }
}
