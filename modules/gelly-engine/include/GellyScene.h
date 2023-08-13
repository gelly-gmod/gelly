#ifndef GELLY_GELLYSCENE_H
#define GELLY_GELLYSCENE_H

#include <NvFlex.h>
#include <NvFlexExt.h>

struct Vec4{
    float x, y, z, w;
};

struct Vec3{
    float x, y, z;
};

using Quat = Vec4;

//class Colliders {
//private:
//    NvFlexVector<NvFlexCollisionGeometry> geometries;
//    NvFlexVector<Vec4> positions;
//    NvFlexVector<Quat> rotations;
//    NvFlexVector<Vec4> prevPositions;
//    NvFlexVector<Quat> prevRotations;
//    NvFlexVector<int> flags;
//};

class GellyScene {
private:
    bool gpuWork;
    int maxParticles;
    int currentParticleCount;

    NvFlexSolver* solver;
    NvFlexLibrary* library;
    NvFlexParams* params;

    NvFlexVector<Vec4> positions;
    NvFlexVector<Vec3> velocities;
    NvFlexVector<int> phases;
    NvFlexVector<int> activeIndices;

//    Colliders colliders;
public:
    GellyScene(NvFlexLibrary* library, int maxParticles);
    ~GellyScene();

    /**
     * This method ensures that the GPU is not calculating anything while we are. When this method is called, all buffers become available for reading/writing.
     * This method should be called before any read/write operations on the buffers.
     */
    void EnterGPUWork();
    /**
     * This method ensures that the GPU can start calculating again without us interfering. When this method is called, all buffers become closed for reading/writing.
     */
    void ExitGPUWork();

    void Update(float deltaTime);
    void AddParticle(Vec4 position, Vec3 velocity);
    [[nodiscard]] Vec4* GetPositions() const;
    [[nodiscard]] Vec3* GetVelocities() const;

    [[nodiscard]] int GetCurrentParticleCount() const;
};

#endif