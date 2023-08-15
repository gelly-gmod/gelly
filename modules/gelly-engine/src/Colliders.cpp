#include <utility>

#include "GellyScene.h"

/**
 * @brief This is a monotonic increasing counter that is used to assign unique IDs to entities.
 */
static EntityHandle nextEntityHandle = 0;

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

void Colliders::AddTriangleMesh(const std::string& modelPath, const MeshUploadInfo &info) {
    NvFlexBuffer* verticesBuffer = NvFlexAllocBuffer(library, info.vertexCount, sizeof(Vec4), eNvFlexBufferHost);
    NvFlexBuffer* indicesBuffer = NvFlexAllocBuffer(library, info.indexCount, sizeof(int), eNvFlexBufferHost);

    Vec4* vertices = static_cast<Vec4 *>(NvFlexMap(verticesBuffer, eNvFlexMapWait));
    int* indices = static_cast<int *>(NvFlexMap(indicesBuffer, eNvFlexMapWait));

    for (int i = 0; i < info.vertexCount; i++) {
        vertices[i] = Vec4{info.vertices[i].x, info.vertices[i].y, info.vertices[i].z, 0};
    }

    memcpy(indices, info.indices, sizeof(int) * info.indexCount);

    NvFlexUnmap(verticesBuffer);
    NvFlexUnmap(indicesBuffer);

    auto meshId = NvFlexCreateTriangleMesh(library);

    NvFlexUpdateTriangleMesh(library, meshId, verticesBuffer, indicesBuffer, info.vertexCount, info.vertexCount / 3,
                             reinterpret_cast<const float *>(&info.lower), reinterpret_cast<const float *>(&info.upper));

    meshes[modelPath] = {
            .id = meshId,
            .vertices = verticesBuffer,
            .indices = indicesBuffer
    };
}

EntityHandle Colliders::AddEntity(GellyEntity entity) {
    EntityHandle handle = nextEntityHandle++;
    entities[handle] = std::move(entity);

    return handle;
}

GellyEntity* Colliders::ModifyEntity(EntityHandle handle) {
    if (entities.find(handle) == entities.end()) {
        return nullptr;
    }

    return &entities[handle];
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
        if (meshes.find(entity.modelPath) == meshes.end()) {
            printf("Mesh not found: %s\n", entity.modelPath.c_str());
            continue;
        }

        const auto& mesh = meshes[entity.modelPath];
        geometries[entityIndex].triMesh.mesh = mesh.id;
        geometries[entityIndex].triMesh.scale[0] = 1.f;
        geometries[entityIndex].triMesh.scale[1] = 1.f;
        geometries[entityIndex].triMesh.scale[2] = 1.f;

        prevPositions[entityIndex] = positions[entityIndex];
        prevRotations[entityIndex] = rotations[entityIndex];

        positions[entityIndex] = Vec4{entity.position.x, entity.position.y, entity.position.z, 0};
        rotations[entityIndex] = entity.rotation;

        flags[entityIndex] = NvFlexMakeShapeFlags(eNvFlexShapeTriangleMesh, true);
    }
}
