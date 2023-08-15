#ifndef GELLY_GELLYSCENE_H
#define GELLY_GELLYSCENE_H

#include <cstring>
#include <NvFlex.h>
#include <NvFlexExt.h>
#include <vector>
#include <map>
#include <string>
#include <unordered_map>

struct Vec4 {
    float x, y, z, w;
};

struct Vec3 {
    float x, y, z;
};

using Quat = Vec4;

class GPUCriticalObject {
protected:
    bool gpuWork;
public:
    virtual ~GPUCriticalObject() = default;

    virtual void EnterGPUWork() = 0;

    virtual void ExitGPUWork() = 0;
};

struct MeshUploadInfo {
    Vec3 *vertices;
    int vertexCount;
    int *indices;
    int indexCount;
    Vec3 upper;
    Vec3 lower;
};

using MeshID = NvFlexTriangleMeshId;

struct GellyEntity {
    Vec3 position;
    Quat rotation;
    std::string modelPath;
};

/**
 * @brief Internal mesh used to track the mesh ID and buffers for later deletion.
 */
struct ColliderMesh {
    MeshID id;
    NvFlexBuffer* vertices;
    NvFlexBuffer* indices;
};

using EntityHandle = unsigned int;
class Colliders : GPUCriticalObject {
    friend class GellyScene;
private:
    NvFlexLibrary *library;
    NvFlexVector<NvFlexCollisionGeometry> geometries;
    NvFlexVector<Vec4> positions;
    NvFlexVector<Quat> rotations;
    NvFlexVector<Vec4> prevPositions;
    NvFlexVector<Quat> prevRotations;
    NvFlexVector<int> flags;

    std::map<std::string, ColliderMesh> meshes;
    std::unordered_map<EntityHandle, GellyEntity> entities;
public:
    void EnterGPUWork() override;

    void ExitGPUWork() override;

    Colliders(NvFlexLibrary *library, int maxColliders);

    ~Colliders() override = default;

    void AddTriangleMesh(const std::string& modelPath, const MeshUploadInfo &info);

    /**
     * Adds an entity to the collider simulation. It's recommended to use this method during GPU work mode.
     * @note The entity structure passed is expected to not be used again as this method moves the entity into the internal map for efficient transfer.
     * @param entity
     * @return
     */
    EntityHandle AddEntity(GellyEntity entity);

    /**
     * Modifies an entity, this method requires that the class is in GPU work mode.
     * @param handle Entity handle received from AddEntity
     * @return Temporary pointer to the entity, this pointer is only valid while the class is in GPU work mode.
     */
    GellyEntity* ModifyEntity(EntityHandle handle);

    [[nodiscard]] int GetEntityCount() const;

    /**
     * Updates the colliders, this method requires that the class is in GPU work mode.
     */
    void Update();
};

class GellyScene : GPUCriticalObject {
private:
    int maxParticles;
    int currentParticleCount;

    NvFlexSolver *solver;
    NvFlexLibrary *library;
    NvFlexVector<Vec4> positions;
    NvFlexVector<Vec3> velocities;
    NvFlexVector<int> phases;
    NvFlexVector<int> activeIndices;
public:
    Colliders colliders;
    NvFlexParams *params;

    GellyScene(NvFlexLibrary *library, int maxParticles, int maxColliders);

    ~GellyScene() override;

    /**
     * This method ensures that the GPU is not calculating anything while we are. When this method is called, all buffers become available for reading/writing.
     * This method should be called before any read/write operations on the buffers.
     */
    void EnterGPUWork() override;

    /**
     * This method ensures that the GPU can start calculating again without us interfering. When this method is called, all buffers become closed for reading/writing.
     */
    void ExitGPUWork() override;

    void Update(float deltaTime);

    void AddParticle(Vec4 position, Vec3 velocity);

    void AddBSP(const std::string& mapName, uint8_t* data, size_t dataSize);

    [[nodiscard]] Vec4 *GetPositions() const;

    [[nodiscard]] Vec3 *GetVelocities() const;

    [[nodiscard]] int GetCurrentParticleCount() const;
};

#endif