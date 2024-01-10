Texture3D<uint> g_particleCount : register(t0);
RWBuffer<uint> g_particlesInVoxel : register(u1);
Buffer<float4> g_positions : register(t2);
RWTexture3D<half2> g_bdg : register(u3);

#include "VoxelCB.hlsli"
#include "util/VoxelToIndex.hlsli"

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
static const float g_isoValue = 15.f;

float W(float3 xi, float3 xj) {
    return 1.f - pow((abs(length(xi - xj)) / g_supportRadius), 3);
}

float EstimateMaxDensity(uint3 voxelPosition) {
    // the authors proposed an estimation of the maximum density in a voxel by using the
    // particle count and weight instead of actually calculating the density
    // pMaxC = mNC * W(0, r)

    float N = 0.f;

    for (uint i = 0; i < 27; i++) {
        uint3 neighborPosition = voxelPosition + NEIGHBOR_OFFSETS[i];
        uint particleCount = g_particleCount[neighborPosition];
        N += particleCount;
    }

    // W in the paper already expects ||xi - xj||, so we can just pass 0 as the second parameter,
    // also m is 1, so we can just ignore it
    return N * W(float3(0.f, 0.f, 0.f), float3(0.f, 0.f, 0.f));
}

[numthreads(4, 4, 4)]
void main(uint3 threadID : SV_DispatchThreadID) {
    uint3 voxelPosition = threadID.xyz;
    if (voxelPosition.x >= g_domainSize.x || voxelPosition.y >= g_domainSize.y || voxelPosition.z >= g_domainSize.z) {
        return;
    }

    uint voxelIndex = VoxelToIndex(voxelPosition);
    float maxDensity = EstimateMaxDensity(voxelPosition);

    half2 bd = half2(
        maxDensity,
        maxDensity > g_isoValue ? 1.0f : 0.0f
    );

    g_bdg[voxelPosition] = bd;
}