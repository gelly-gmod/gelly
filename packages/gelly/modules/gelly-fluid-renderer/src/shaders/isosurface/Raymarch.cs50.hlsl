#include "VoxelCB.hlsli"
#include "util/VoxelizePosition.hlsli"
#include "util/VoxelToIndex.hlsli"

#include "FluidCB.hlsli"
#include "../util/EyeToProjDepth.hlsli"

Texture3D<float2> g_bdg : register(t0);
SamplerState g_bdgSampler : register(s0);
Texture3D<uint> g_particleCount : register(t1);
RWBuffer<uint> g_particlesInVoxel : register(u2);
Buffer<float4> g_positions : register(t3);

RWTexture2D<half2> g_depth : register(u4);
RWTexture2D<float4> g_normal : register(u5);

Texture2D<float2> g_entryDepth : register(t6);
SamplerState g_entryDepthSampler : register(s6);
Texture2D<float2> g_exitDepth : register(t7);

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
void main(uint3 threadID : SV_DispatchThreadID, uint3 groupID : SV_GroupThreadID) {
    float2 texcoord = threadID.xy / float2(g_ViewportWidth, g_ViewportHeight);

    float entryProjDepth = g_entryDepth.SampleLevel(g_entryDepthSampler, texcoord, 0).y;
    float exitProjDepth = g_exitDepth[threadID.xy].y;

    if (entryProjDepth == 1.f || exitProjDepth == 1.f) {
        g_depth[threadID.xy] = half2(0.0f, 1.0f);
        g_normal[threadID.xy] = float4(0.f, 0.f, 0.f, 1.0f);
        return;
    }

    float4 rayEntryPosition = float4(
        texcoord.x * 2.0f - 1.0f,
        (1.f - texcoord.y) * 2.0f - 1.0f,
        entryProjDepth,
        1.0f
    );

    rayEntryPosition = mul(g_InverseProjection, rayEntryPosition);
    rayEntryPosition = mul(g_InverseView, rayEntryPosition);
    rayEntryPosition /= rayEntryPosition.w;

    float4 rayExitPosition = float4(
        texcoord.x * 2.0f - 1.0f,
        (1.f - texcoord.y) * 2.0f - 1.0f,
        exitProjDepth,
        1.0f
    );

    rayExitPosition = mul(g_InverseProjection, rayExitPosition);
    rayExitPosition = mul(g_InverseView, rayExitPosition);
    rayExitPosition /= rayExitPosition.w;

    float3 rayDirection = normalize(rayExitPosition.xyz - rayEntryPosition.xyz);
    // using the parametric equation we can split up the ray into steps
    float t_step = length(rayExitPosition.xyz - rayEntryPosition.xyz) / g_maxSteps;

    for (uint i = 0; i < g_maxSteps; i++) {
        float3 position = rayEntryPosition.xyz + (rayDirection * (t_step * i));
        uint3 voxelPosition = VoxelizePosition(position);
        uint voxelIndex = VoxelToIndex(voxelPosition);

        // if out of bounds, stop marching
        if (voxelIndex >= g_domainSize.x * g_domainSize.y * g_domainSize.z) {
            break;
        }

        // evaluate BDG here, if we have no surface, we can skip this voxel
        // also important to not sample since that'll confuse the raymarcher

        
        // we've hit the surface, so color the voxel
        float density = CalculateDensity(position, voxelPosition);
        if (density > g_ParticleRadius) {
            // we can sample the density field to grab normals
            float3 normal = float3(
                CalculateDensity(position + float3(g_ThresholdRatio, 0.0f, 0.0f), voxelPosition) - CalculateDensity(position - float3(g_ThresholdRatio, 0.0f, 0.0f), voxelPosition),
                CalculateDensity(position + float3(0.0f, g_ThresholdRatio, 0.0f), voxelPosition) - CalculateDensity(position - float3(0.0f, g_ThresholdRatio, 0.0f), voxelPosition),
                CalculateDensity(position + float3(0.0f, 0.0f, g_ThresholdRatio), voxelPosition) - CalculateDensity(position - float3(0.0f, 0.0f, g_ThresholdRatio), voxelPosition)
            );
            normal = -normalize(normal);
            float3 sunDir = normalize(float3(0.5f, 0.5f, 0.5f));

            float3 diffuse = max(dot(normal, sunDir), 0.0f);
            float3 reflectionDir = reflect(rayDirection, normal);
            float3 specular = pow(max(dot(reflectionDir, sunDir), 0.0f), 64.0f);

            float3 color = colorHash(voxelPosition);
            float4 projectedPosition = mul(g_View, float4(position, 1.0f));
            projectedPosition = mul(g_Projection, projectedPosition);
            projectedPosition /= projectedPosition.w;

            g_depth[threadID.xy] = half2(projectedPosition.z, 1.0f);
            g_normal[threadID.xy] = float4(normal * 0.5f + 0.5f, 1.0f);
            return;
        }
    }

    g_depth[threadID.xy] = half2(0.0f, 1.0f);
    g_normal[threadID.xy] = float4(0.f, 0.f, 0.f, 1.0f);
}