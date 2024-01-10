#include "VoxelCB.hlsli"
#include "util/VoxelizePosition.hlsli"
#include "util/VoxelToIndex.hlsli"

static uint3 DISPATCH_SIZE = uint3(64, 128, 1);

Buffer<float4> g_positions : register(t0);
RWTexture3D<uint> g_particleCount : register(u1);
RWBuffer<uint> g_particlesInVoxel : register(u2);

// The threads are completely arbitrary, but a workgroup size of 64 is favored by some hardware.
[numthreads(64, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID) {
    uint particleIndex = threadID.y * DISPATCH_SIZE.x + threadID.x;
    if (particleIndex >= g_activeParticles) {
        return;
    }

    float4 position = g_positions[particleIndex];
    uint3 voxelPosition = VoxelizePosition(position.xyz);

    uint voxelIndex = VoxelToIndex(voxelPosition);
    uint currentCount = g_particleCount[voxelPosition];

    // If the voxel is full, we can't add any more particles to it- so early out.
    if (currentCount >= g_maxParticlesInVoxel) {
        return;
    }

    // must do this atomically since some other thread might be trying to write to the same voxel
    InterlockedAdd(g_particleCount[voxelPosition], 1);

    uint newCount = currentCount + 1;
    if (newCount < g_maxParticlesInVoxel) {
        g_particlesInVoxel[voxelIndex * g_maxParticlesInVoxel + newCount] = particleIndex;
    }
}