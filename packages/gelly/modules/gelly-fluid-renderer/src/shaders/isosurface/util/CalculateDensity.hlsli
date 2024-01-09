#include "../VoxelCB.hlsli"
#include "VoxelizePosition.hlsli"
#include "VoxelToIndex.hlsli"

static uint3 NEIGHBOR_OFFSETS[27] = {
    uint3(-1, -1, -1),
    uint3(-1, -1, 0),
    uint3(-1, -1, 1),
    uint3(-1, 0, -1),
    uint3(-1, 0, 0),
    uint3(-1, 0, 1),
    uint3(-1, 1, -1),
    uint3(-1, 1, 0),
    uint3(-1, 1, 1),
    uint3(0, -1, -1),
    uint3(0, -1, 0),
    uint3(0, -1, 1),
    uint3(0, 0, -1),
    uint3(0, 0, 0),
    uint3(0, 0, 1),
    uint3(0, 1, -1),
    uint3(0, 1, 0),
    uint3(0, 1, 1),
    uint3(1, -1, -1),
    uint3(1, -1, 0),
    uint3(1, -1, 1),
    uint3(1, 0, -1),
    uint3(1, 0, 0),
    uint3(1, 0, 1),
    uint3(1, 1, -1),
    uint3(1, 1, 0),
    uint3(1, 1, 1)
};

static const float g_supportRadius = g_voxelSize;

float W(float3 xi, float3 xj) {
    return 1.f - pow((abs(length(xi - xj)) / g_supportRadius), 3);
}

float CalculateDensity(float3 currentPosition, uint3 voxelPosition) {
    float density = 0.0f;

    for (uint i = 0; i < 27; i++) {
        uint3 neighborPosition = voxelPosition + NEIGHBOR_OFFSETS[i];
        uint neighborIndex = VoxelToIndex(neighborPosition);
        uint particleCount = g_particleCount[neighborIndex];
        
        for (uint j = 0; j < particleCount; j++) {
            uint particleIndex = g_particlesInVoxel[neighborIndex * g_maxParticlesInVoxel + j];
            float4 position = g_positions[particleIndex];
            float weight = W(currentPosition, position.xyz);
            density += weight;
        }
    }

    return density;
}