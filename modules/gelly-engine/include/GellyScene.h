#ifndef GELLY_GELLYSCENE_H
#define GELLY_GELLYSCENE_H

#include <NvFlex.h>
#include <NvFlexExt.h>
#include <vector>

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
    MeshID mesh;
};

class Colliders : GPUCriticalObject {
    friend class GellyScene;
private:
    NvFlexLibrary *library;
    std::vector<GellyEntity> entities;
    NvFlexVector<NvFlexCollisionGeometry> geometries;
    NvFlexVector<Vec4> positions;
    NvFlexVector<Quat> rotations;
    NvFlexVector<Vec4> prevPositions;
    NvFlexVector<Quat> prevRotations;
    NvFlexVector<int> flags;
public:
    void EnterGPUWork() override;

    void ExitGPUWork() override;

    Colliders(NvFlexLibrary *library, int maxColliders);

    ~Colliders() override = default;

    MeshID AddTriangleMesh(const MeshUploadInfo &info);

    void AddEntity(GellyEntity entity);

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

    [[nodiscard]] Vec4 *GetPositions() const;

    [[nodiscard]] Vec3 *GetVelocities() const;

    [[nodiscard]] int GetCurrentParticleCount() const;
};

#endif