// Isosurface extraction based on Fast, High-Quality Rendering of Liquids Generated Using Large-scale SPH Simulation by X. Xiao, et al.
// https://jcgt.org/published/0007/01/02/paper.pdf

#include "PerFrameCB.hlsli"

// Read-only buffers, uses SRVs. 
StructuredBuffer<uint> neighborIndices : register(t0);
StructuredBuffer<uint> neighborCounts : register(t1);
StructuredBuffer<uint> internalToAPI : register(t2);
StructuredBuffer<uint> apiToInternal : register(t3);
Buffer<float4> positions : register(t4);

Texture2D<float4> depth : register(t5);
RWTexture2D<float4> normal : register(u0);

uint GetNeighborCount(uint index) {
    uint internalIndex = apiToInternal[index];
    return neighborCounts[internalIndex];
}

float3 GetColorForIndex(uint index) {
    uint r = (index + 22) % 255;
    uint g = (index + 32) % 255;
    uint b = (index + 2) % 255;

    return float3(
        (float)r,
        (float)g,
        (float)b
    ) / float3(255, 255, 255);
}

// The isosurface reconstruction is done in 4x4 tiles.
[numthreads(4, 4, 1)]
void main(uint3 id : SV_DispatchThreadID) {
    // Just because of the nature of the tiling, we're unfortunately going to have to prune out-of-bounds threads.
    uint width = 0;
    uint height = 0;
    normal.GetDimensions(width, height);

    if (id.x >= width || id.y >= height) {
        return;
    }

    float2 depthIndex = depth[id.xy].xy;

    float depthValue = depthIndex.x;
    uint index = asuint(depthIndex.y);
    uint neighborCount = GetNeighborCount(index);
    float debugValue = (float)neighborCount / 64.f;
    normal[id.xy] = float4(debugValue, debugValue, debugValue, 1.f);
}