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

    // we can sample trilinearly at the voxel position to get the density
    currentPosition.xyz += 0.5f * g_voxelSize;
    float3 fractionalVoxelPosition = currentPosition / float3(g_voxelSize, g_voxelSize, g_voxelSize);
    // Then we need to convert to 0-1
    fractionalVoxelPosition /= g_domainSize;
    density += g_bdg.SampleLevel(g_bdgSampler, fractionalVoxelPosition, 0).x;

    return density;
}