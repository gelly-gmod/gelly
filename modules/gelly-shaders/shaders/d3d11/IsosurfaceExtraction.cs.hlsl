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

float3 GetWorldPos(uint x, uint y, uint w, uint h, float depth) {
    // We have to convert window space coords to clip space coords.
    float u = (float)x / (float)w;
    float v = (float)y / (float)h;

    float clipX = u * 2.f - 1.0;
    float clipY = (1.f - v) * 2.f - 1.0;
    float clipZ = depth;

    float4 clipCoord = float4(clipX, clipY, clipZ, 1.f);
    float4 viewPosition = mul(clipCoord, matInvProj);
    float4 worldPosition = mul(viewPosition / viewPosition.w, matInvView);

    return worldPosition.xyz;
}

float SmoothingKernel(float3 worldPos, float3 neighborPos, float radius) {
    return 1.f - pow(abs(length((worldPos - neighborPos))) / radius, 3);
}

float GetDensity(float3 worldPos, uint index) {
    float sum = 0.f;

    // Iterate over each neighbor and apply the smoothing function.
    /*
    // find offset in the neighbors buffer
		int offset = apiToInternal[i];
		int count = counts[offset];

		for (int c=0; c  count; ++c)
		{
			int neighbor = internalToApi[neighbors[c*stride + offset]];

			printf("Particle %d's neighbor %d is particle %d\n", i, c, neighbor);
		}
        */

    uint stride = 2100001;
    uint offset = apiToInternal[index];
    uint count = neighborCounts[offset];

    for (int c = 0; c < count; c++) {
        int neighbor = internalToAPI[neighborIndices[c*stride + offset]];
        float3 neighborPos = positions[neighbor].xyz;

        float m_j = 0.05f;
        float r = 255.f;
        sum += m_j * SmoothingKernel(worldPos, neighborPos, r);
    }

    return sum;
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
    if (depthValue == 1.f) {
        return;
    }

    uint index = asuint(depthIndex.y);
    
    float3 worldPos = GetWorldPos(id.x, id.y, width, height, depthValue);
    float isoValue = 5.f;
    float density = GetDensity(worldPos, index) + 0.001f;

    // Calculate the normal by taking the gradient of the density field.
    float3 gradient = float3(0, 0, 0);
    // TODO: THIS IS RETURNING THE SAME VALUE FOR EVERY PIXEL. WHY?
    float dist = 0.2f;
    float xDensity = GetDensity(worldPos + float3(dist, 0, 0), index);
    float yDensity = GetDensity(worldPos + float3(0, dist, 0), index);
    float zDensity = GetDensity(worldPos + float3(0, 0, dist), index);

    normal[id.xy] = float4(xDensity, yDensity, zDensity, 1.f);
}