// Isosurface extraction based on Fast, High-Quality Rendering of Liquids Generated Using Large-scale SPH Simulation by X. Xiao, et al.
// https://jcgt.org/published/0007/01/02/paper.pdf

RWStructuredBuffer<int> neighborIndices;
RWStructuredBuffer<int> neighborCounts;
RWStructuredBuffer<int> remap;
RWStructuredBuffer<float4> positions;

RWTexture2D<float4> normal;

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
}