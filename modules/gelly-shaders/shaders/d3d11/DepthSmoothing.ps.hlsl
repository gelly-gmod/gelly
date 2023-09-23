#include "NDCQuadStages.hlsli"
#include "PerFrameCB.hlsli"

struct PS_OUTPUT {
    float4 SmoothedDepth : SV_Target0;
};

static const float2 pixelScale = 1.f / res;
static const int maxFilterSize = 4;

Texture2D depth : register(t0);
SamplerState depthSampler : register(s0);

// Narrow-range filter implementation.

float GetFilterSize(float z) {
    // Authors of the paper use arbitrary scalars for the world filter size.
    // Since in GMod, our particle scale has to be massive, I'm using a pretty small value compared to theirs.
    float worldSize = 0.2 * particleRadius;

    float numerator = res.y * worldSize;
    float denominator = 2 * abs(z) * tan(fov / 2);

    return 3 * (numerator / denominator);
}

float GaussianWeight(float2 i_coord, float2 j_coord, float standardDeviation) {
    float power = -length(j_coord - i_coord);
    power *= power;
    power /= 2 * standardDeviation * standardDeviation;

    return exp(power);
}

float Clamping(float z_i, float z_j, float low, float high) {
    if (z_j >= z_i - high) {
        return z_j;
    }

    return z_i - low;
}

float GetDepth(float2 coord) {
    return depth.Sample(depthSampler, coord).r;
}

float NarrowRangeFilter(float2 coord, float low, float high) {
    float z_i = GetDepth(coord);
    int filterSize = GetFilterSize(z_i);

    float sum = 0.0f;
    float weightSum = 0.0f;

    for (int x = -maxFilterSize; x <= maxFilterSize; x++) {
        for (int y = -maxFilterSize; y <= maxFilterSize; y++) {
            float2 offset = float2(x, y) * pixelScale;
            float z_j = GetDepth(coord + offset);

            if (z_j == 1.0f) {
                continue;
            }

            float weight = GaussianWeight(coord, coord + offset, filterSize);
            sum += weight * Clamping(z_i, z_j, low, high);
            weightSum += weight;
        }
    }

    return sum / weightSum;
}

PS_OUTPUT main(VS_OUTPUT input) {
    PS_OUTPUT output;
    float filteredDepth = NarrowRangeFilter(input.Texcoord, particleRadius * 0.1, particleRadius * 0.04);
    output.SmoothedDepth = float4(filteredDepth, 0, 0, 1);
    return output;
}