Texture3D<uint> g_particleCount : register(t0);
RWBuffer<uint> g_particlesInVoxel : register(u1);
Buffer<float4> g_positions : register(t2);
RWTexture3D<float2> g_bdg : register(u3);

#include "VoxelCB.hlsli"
#include "util/VoxelToIndex.hlsli"

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



static const float g_supportRadius = g_voxelSize * 2;
// P is a symmetric decaying spline kernel
static const float P = 315.f / (64.f * 3.14159265358979323846264338 * pow(g_supportRadius, 9));
static const float scalingFactor = 1.f / pow(g_supportRadius, 6);

float W(float3 xi, float3 xj) {
    return 1.f - pow((abs(length(xi - xj)) / g_supportRadius), 3);
}


float EstimateMaxDensity(uint3 voxelPosition) {
    // the authors proposed an estimation of the maximum density in a voxel by using the
    // particle count and weight instead of actually calculating the density
    // pMaxC = mNC * W(0, r)

    // float N = 0.f;

    // for (uint i = 0; i < 27; i++) {
    //     uint3 neighborPosition = voxelPosition + NEIGHBOR_OFFSETS[i];
    //     uint particleCount = g_particleCount[neighborPosition];
    //     N += particleCount;
    // }

    // // W in the paper already expects ||xi - xj||, so we can just pass 0 as the second parameter,
    // // also m is 1, so we can just ignore it
    // return N * W(float3(0.f, 0.f, 0.f), float3(0.f, 0.f, 0.f));

    float density = 0.0f;
    for (uint i = 0; i < 27; i++) {
        int3 neighborOffset = NEIGHBOR_OFFSETS[i];
        uint3 neighborVoxelPosition = (int3)voxelPosition + neighborOffset;

        uint neighborIndex = VoxelToIndex(neighborVoxelPosition);
        uint particleCount = g_particleCount[neighborVoxelPosition];
        for (uint j = 0; j < g_maxParticlesInVoxel; j++) {
            if (j >= particleCount) {
                break;
            }

            uint particleIndex = g_particlesInVoxel[neighborIndex * g_maxParticlesInVoxel + j];
            float3 particlePosition = g_positions[particleIndex].xyz;
            density += W(float3(voxelPosition) * g_voxelSize, particlePosition);
        }
    }
    return density;
}

[numthreads(4, 4, 4)]
void main(uint3 threadID : SV_DispatchThreadID) {
    uint3 voxelPosition = threadID.xyz;
    if (voxelPosition.x >= g_domainSize.x || voxelPosition.y >= g_domainSize.y || voxelPosition.z >= g_domainSize.z) {
        return;
    }

    uint voxelIndex = VoxelToIndex(voxelPosition);
    float maxDensity = EstimateMaxDensity(voxelPosition);

    float2 bd = float2(
        maxDensity,
        maxDensity > 5.f ? 1.0f : 0.0f
    );

    g_bdg[voxelPosition] = bd;
}