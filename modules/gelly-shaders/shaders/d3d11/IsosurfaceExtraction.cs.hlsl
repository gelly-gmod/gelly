// Isosurface extraction based on Fast, High-Quality Rendering of Liquids Generated Using Large-scale SPH Simulation by X. Xiao, et al.
// https://jcgt.org/published/0007/01/02/paper.pdf

#include "PerFrameCB.hlsli"

// Read-only buffers, uses SRVs. 
Buffer<int> neighborIndices : register(t0);
Buffer<int> neighborCounts : register(t1);
Buffer<int> internalToAPI : register(t2);
Buffer<int> apiToInternal : register(t3);
Buffer<float4> positions : register(t4);

Texture2D<float4> depth : register(t5);
RWTexture2D<float4> normal : register(u0);

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

    normal[id.xy] = float4(depth[id.xy].xyz, 1.0f);
}