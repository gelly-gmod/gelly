#include "VoxelCB.hlsli"
#include "util/VoxelizePosition.hlsli"
#include "util/VoxelToIndex.hlsli"

#include "FluidCB.hlsli"

RWBuffer<half2> g_bdg : register(u0);
RWBuffer<uint> g_particleCount : register(u1);
RWBuffer<uint> g_particlesInVoxel : register(u2);
Buffer<float4> g_positions : register(t3);

RWTexture2D<half2> g_depth : register(u4);
RWTexture2D<float4> g_normal : register(u5);

static const float g_stepLength = 0.5f;
static const uint g_maxSteps = 16;

[numthreads(8, 8, 1)]
void main(uint3 threadID : SV_DispatchThreadID) {
    float2 texcoord = threadID.xy / float2(g_ViewportWidth, g_ViewportHeight);
    float4 rayEnd = float4(
        texcoord.x * 2.0f - 1.0f,
        (1.0f - texcoord.y) * 2.0f - 1.0f,
        1.0f,
        1.0f
    );

    rayEnd = mul(rayEnd, g_InverseProjection);
    rayEnd = mul(rayEnd, g_InverseView);
    rayEnd /= rayEnd.w;

    float3 rayDirection = normalize(rayEnd.xyz - g_CameraPosition);

    for (uint i = 0; i < g_maxSteps; i++) {
        float3 position = g_CameraPosition + (rayDirection * (g_stepLength * (float)i));
        uint3 voxelPosition = VoxelizePosition(position);
        uint voxelIndex = VoxelToIndex(voxelPosition);

        uint currentCount = g_particleCount[voxelIndex];
        if (currentCount > 0) {
            g_depth[threadID.xy] = half2(position.z, 1.0f);
            // color code voxel index
            float3 color = float3(voxelIndex, voxelIndex, voxelIndex);
            g_normal[threadID.xy] = float4(color, 1.0f);
            return;
        }
    }

    g_depth[threadID.xy] = half2(0.0f, 1.0f);
    g_normal[threadID.xy] = float4(rayDirection, 1.0f);
}