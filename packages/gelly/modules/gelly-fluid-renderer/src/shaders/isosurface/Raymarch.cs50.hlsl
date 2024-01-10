#include "VoxelCB.hlsli"
#include "util/VoxelizePosition.hlsli"
#include "util/VoxelToIndex.hlsli"

#include "FluidCB.hlsli"
#include "../util/EyeToProjDepth.hlsli"

Texture3D<half2> g_bdg : register(t0);
SamplerState g_bdgSampler : register(s0);
Texture3D<uint> g_particleCount : register(t1);
RWBuffer<uint> g_particlesInVoxel : register(u2);
Buffer<float4> g_positions : register(t3);

RWTexture2D<half2> g_depth : register(u4);
RWTexture2D<float4> g_normal : register(u5);

#include "util/CalculateDensity.hlsli"

static const float g_stepLength = g_voxelSize * 0.3f;
static const uint g_maxSteps = 128;

float3 colorHash(uint3 voxel) {
    uint hash = voxel.x * 73856093 + voxel.y * 19349663 + voxel.z * 83492791;
    float3 color = float3(
        (hash & 0xFF) / 255.0f,
        ((hash >> 8) & 0xFF) / 255.0f,
        ((hash >> 16) & 0xFF) / 255.0f
    );

    return color;
}

// iso-value for the surface
static const float g_isoValue = 150.f;

[numthreads(8, 8, 1)]
void main(uint3 threadID : SV_DispatchThreadID) {
    float2 texcoord = threadID.xy / float2(g_ViewportWidth, g_ViewportHeight);

    float4 rayEnd = float4(
        texcoord.x * 2.0f - 1.0f,
        (1.f - texcoord.y) * 2.0f - 1.0f,
        1.f,
        1.0f
    );

    rayEnd = mul(g_InverseProjection, rayEnd);
    rayEnd = mul(g_InverseView, rayEnd);
    rayEnd /= rayEnd.w;

    float4 rayStart = float4(
        texcoord.x * 2.0f - 1.0f,
        (1.f - texcoord.y) * 2.0f - 1.0f,
        0.f,
        1.0f
    );

    rayStart = mul(g_InverseProjection, rayStart);
    rayStart = mul(g_InverseView, rayStart);
    rayStart /= rayStart.w;

    float3 rayDirection = normalize(rayEnd.xyz - rayStart.xyz);

    for (uint i = 0; i < g_maxSteps; i++) {
        float3 position = rayStart.xyz + (rayDirection * (g_stepLength * (float)i));
        uint3 voxelPosition = VoxelizePosition(position);
        uint voxelIndex = VoxelToIndex(voxelPosition);

        // if out of bounds, stop marching
        if (voxelIndex >= g_domainSize.x * g_domainSize.y * g_domainSize.z) {
            break;
        }

        uint currentCount = g_particleCount[voxelPosition];
        
        // we've hit the surface, so color the voxel
        if (CalculateDensity(position, voxelPosition) > g_isoValue) {
            // we can sample the density field to grab normals
            float3 normal = float3(
                CalculateDensity(position + float3(g_voxelSize, 0.f, 0.f), voxelPosition) - CalculateDensity(position - float3(g_voxelSize, 0.f, 0.f), voxelPosition),
                CalculateDensity(position + float3(0.f, g_voxelSize, 0.f), voxelPosition) - CalculateDensity(position - float3(0.f, g_voxelSize, 0.f), voxelPosition),
                CalculateDensity(position + float3(0.f, 0.f, g_voxelSize), voxelPosition) - CalculateDensity(position - float3(0.f, 0.f, g_voxelSize), voxelPosition)
            );

            normal = -normalize(normal);
            float3 sunDir = normalize(float3(0.5f, 0.5f, 0.5f));

            float3 diffuse = max(dot(normal, sunDir), 0.0f);
            float3 reflectionDir = reflect(rayDirection, normal);
            float3 specular = pow(max(dot(reflectionDir, sunDir), 0.0f), 64.0f);

            float3 color = colorHash(voxelPosition);
            g_depth[threadID.xy] = half2(position.z, 1.0f);
            g_normal[threadID.xy] = float4(diffuse + specular, 1.0f);
            return;
        }
    }

    g_depth[threadID.xy] = half2(0.0f, 1.0f);
    g_normal[threadID.xy] = float4(rayDirection, 1.0f);
}