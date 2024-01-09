RWBuffer<uint> g_particleCount : register(u0);
RWBuffer<uint> g_particlesInVoxel : register(u1);

#include "VoxelCB.hlsli"
#include "util/VoxelToIndex.hlsli"

[numthreads(4, 4, 4)]
void main(uint3 threadID : SV_DispatchThreadID) {
    uint3 voxelPosition = threadID.xyz;
    if (voxelPosition.x >= g_domainSize.x || voxelPosition.y >= g_domainSize.y || voxelPosition.z >= g_domainSize.z) {
        return;
    }

    uint voxelIndex = VoxelToIndex(voxelPosition);

    for (uint i = 0; i < g_maxParticlesInVoxel; i++) {
        g_particlesInVoxel[voxelIndex * g_maxParticlesInVoxel + i] = 0;
    }

    g_particleCount[voxelIndex] = 0;
}