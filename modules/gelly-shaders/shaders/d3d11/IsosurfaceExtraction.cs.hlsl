// Isosurface extraction based on Fast, High-Quality Rendering of Liquids Generated Using Large-scale SPH Simulation by X. Xiao, et al.
// https://jcgt.org/published/0007/01/02/paper.pdf

#include "PerFrameCB.hlsli"

// Read-only buffers, uses SRVs. 
StructuredBuffer<int> neighborIndices : register(t0);
StructuredBuffer<int> neighborCounts : register(t1);
StructuredBuffer<int> internalToAPI : register(t2);
StructuredBuffer<int> apiToInternal : register(t3);
Buffer<float4> positions : register(t4);

Texture2D<float4> depth : register(t5);
globallycoherent RWTexture2D<float4> normal : register(u0);

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

float3 GetDensity(float3 worldPos, uint index) {
    float3 sum = float3(0,0,0);

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

    // FIXME: THIS IS TEMPORALLY UNSTABLE??
    int stride = 2100001;
    int offset = apiToInternal[index];
    int count = neighborCounts[offset];

    for (int c = 0; c < count; c++) {
        int neighbor = internalToAPI[neighborIndices[c*stride + offset]];
        float3 neighborPos = positions[neighbor].xyz;

        float m_j = 0.05f;
        float r = 3.f;

        sum += neighborPos;
        //sum += m_j * SmoothingKernel(worldPos, neighborPos, r);
    }

    return normalize(positions[index].xyz);
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

    int index = asint(depthIndex.y);
    
    float3 worldPos = GetWorldPos(id.x, id.y, width, height, depthValue);
    float isoValue = 5.f;
    float3 density = GetDensity(worldPos, index);

    // // Calculate the normal by taking the gradient of the density field.
    // float3 gradient = float3(0, 0, 0);
    // // TODO: THIS IS RETURNING THE SAME VALUE FOR EVERY PIXEL. WHY?
    // float dist = 0.2f;
    // float dx1 = GetDensity(worldPos + float3(dist, 0, 0), index);
    // float dx2 = GetDensity(worldPos - float3(dist, 0, 0), index);
    // float dy1 = GetDensity(worldPos + float3(0, dist, 0), index);
    // float dy2 = GetDensity(worldPos - float3(0, dist, 0), index);
    // float dz1 = GetDensity(worldPos + float3(0, 0, dist), index);
    // float dz2 = GetDensity(worldPos - float3(0, 0, dist), index);

    // gradient.x = (dx1 - dx2) / (2.f * dist);
    // gradient.y = (dy1 - dy2) / (2.f * dist);
    // gradient.z = (dz1 - dz2) / (2.f * dist);
    
    // float3 normalValue = normalize(gradient);
    // normal[id.xy] = float4(normalValue * 0.5 + 0.5, 1.f);

    normal[id.xy] = float4(density, 1.f);
}