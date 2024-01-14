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

static const float g_supportRadius = g_voxelSize * 2;
// P is a symmetric decaying spline kernel
static const float P = 315.f / (64.f * 3.14159265358979323846264338 * pow(g_supportRadius, 9));
static const float scalingFactor = 1.f / pow(g_supportRadius, 6);

float W(float3 xi, float3 xj) {
    return 1.f - pow((abs(length(xi - xj)) / g_supportRadius), 3);
}

// We'll end up having some trilinear filtering artifacts, so we can use Inigo Quilez's trick
/**
vec4 textureNice( sampler2D sam, vec2 uv )
{
    float textureResolution = float(textureSize(sam,0).x);
    uv = uv*textureResolution + 0.5;
    vec2 iuv = floor( uv );
    vec2 fuv = fract( uv );
    uv = iuv + fuv*fuv*(3.0-2.0*fuv);
    uv = (uv - 0.5)/textureResolution;
    return texture( sam, uv );
}
*/

float GrabTrilinearDensity(float3 uv) {
    float res = float(g_domainSize.x);
    uv = uv * res + 0.5f;
    float3 iuv = floor(uv);
    float3 fuv = frac(uv);
    uv = iuv + fuv * fuv * (3.f - 2.f * fuv);
    uv = (uv - 0.5f) / res;
    return g_bdg.SampleLevel(g_bdgSampler, uv, 0).x;
}

float CalculateDensity(float3 currentPosition, uint3 voxelPosition) {
    float density = 0.0f;

    // we can sample trilinearly at the voxel position to get the density
    float3 fractionalVoxelPosition = currentPosition / float3(g_voxelSize, g_voxelSize, g_voxelSize);
    // Then we need to convert to 0-1
    fractionalVoxelPosition /= g_domainSize;
    density += GrabTrilinearDensity(fractionalVoxelPosition);

    // for (uint i = 0; i < 27; i++) {`
    //     int3 neighborOffset = NEIGHBOR_OFFSETS[i];
    //     uint3 neighborVoxelPosition = (int3)voxelPosition + neighborOffset;

    //     uint neighborIndex = VoxelToIndex(neighborVoxelPosition);
    //     uint particleCount = g_particleCount[neighborVoxelPosition];
    //     for (uint j = 0; j < g_maxParticlesInVoxel; j++) {
    //         if (j >= particleCount) {
    //             break;
    //         }

    //         uint particleIndex = g_particlesInVoxel[neighborIndex * g_maxParticlesInVoxel + j];
    //         float3 particlePosition = g_positions[particleIndex].xyz;
    //         density += W(currentPosition, particlePosition) * 0.001f;
    //     }
    // }


    // uint voxelIndex = VoxelToIndex(voxelPosition);
    // uint particleCount = g_particleCount[voxelPosition];
    // for (uint j = 0; j < g_maxParticlesInVoxel; j++) {
    //     if (j >= particleCount) {
    //         break;
    //     }

    //     uint particleIndex = g_particlesInVoxel[voxelIndex * g_maxParticlesInVoxel + j];
    //     float3 particlePosition = g_positions[particleIndex].xyz;
    //     density += W(currentPosition, particlePosition);
    // }

    return density;
}