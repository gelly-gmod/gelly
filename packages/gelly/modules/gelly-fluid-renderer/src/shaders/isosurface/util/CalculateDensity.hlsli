#include "../VoxelCB.hlsli"
#include "VoxelizePosition.hlsli"
#include "VoxelToIndex.hlsli"

static int3 NEIGHBOR_OFFSETS[27] = {
    int3(-1, -1, -1),
    int3(-1, -1, 0),
    int3(-1, -1, 1),
    int3(-1, 0, -1),
    int3(-1, 0, 0),
    int3(-1, 0, 1),
    int3(-1, 1, -1),
    int3(-1, 1, 0),
    int3(-1, 1, 1),
    int3(0, -1, -1),
    int3(0, -1, 0),
    int3(0, -1, 1),
    int3(0, 0, -1),
    int3(0, 0, 0),
    int3(0, 0, 1),
    int3(0, 1, -1),
    int3(0, 1, 0),
    int3(0, 1, 1),
    int3(1, -1, -1),
    int3(1, -1, 0),
    int3(1, -1, 1),
    int3(1, 0, -1),
    int3(1, 0, 0),
    int3(1, 0, 1),
    int3(1, 1, -1),
    int3(1, 1, 0),
    int3(1, 1, 1)
};

static const float g_supportRadius = g_voxelSize;

float W(float3 xi, float3 xj) {
    return pow((abs(length(xi - xj)) / g_supportRadius), 3);
}

float CalculateDensity(float3 currentPosition, uint3 voxelPosition) {
    float density = 0.0f;

    // // we can sample trilinearly at the voxel position to get the density
    // currentPosition.xyz += 0.5f * g_voxelSize;
    // float3 fractionalVoxelPosition = currentPosition / float3(g_voxelSize, g_voxelSize, g_voxelSize);
    // // Then we need to convert to 0-1
    // fractionalVoxelPosition /= g_domainSize;
    // density += g_bdg.SampleLevel(g_bdgSampler, fractionalVoxelPosition, 0).x;

    for (uint i = 0; i < 27; i++) {
        int3 neighborOffset = NEIGHBOR_OFFSETS[i];
        float3 newVoxelPosition = float3(voxelPosition) + float3(neighborOffset);
        uint3 neighborVoxelPosition = VoxelizePosition(newVoxelPosition);

        uint neighborIndex = VoxelToIndex(neighborVoxelPosition);
        uint particleCount = g_particleCount[neighborVoxelPosition];
        for (uint j = 0; j < g_maxParticlesInVoxel; j++) {
            if (j >= particleCount) {
                break;
            }

            uint particleIndex = g_particlesInVoxel[neighborIndex * g_maxParticlesInVoxel + j];
            float3 particlePosition = g_positions[particleIndex].xyz;
            density += 0.1f;
        }
    }

    return density;
}