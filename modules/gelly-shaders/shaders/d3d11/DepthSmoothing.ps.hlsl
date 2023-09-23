#include "NDCQuadStages.hlsli"
#include "PerFrameCB.hlsli"

struct PS_OUTPUT {
    float4 SmoothedDepth : SV_Target0;
};

static const float2 pixelScale = 1.f / res;
static const int maxFilterSize = 10;

Texture2D depth : register(t0);
SamplerState depthSampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};

// Narrow-range filter implementation.

float GetFilterSize(float z) {
    // Authors of the paper use arbitrary scalars for the world filter size.
    // Since in GMod, our particle scale has to be massive, I'm using a pretty small value compared to theirs.
    float worldSize = 0.6 * particleRadius;

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
    // View-space depth.
    if (depth.SampleLevel(depthSampler, coord, 0).r == 1.0f) {
        // Our filter has hit a pixel that was cleared to 1.f, so we're rendering nothing.
        return 0.0f;
    }

    return -depth.SampleLevel(depthSampler, coord, 0).z;
}

float NarrowRangeFilter(float2 coord, float low, float high) {
    float z_i = GetDepth(coord);
    int filterSize = min(maxFilterSize, int(ceil(GetFilterSize(z_i))));

    float sum = 0.0f;
    float weightSum = 0.0f;

    for (int x = -filterSize; x <= filterSize; x++) {
        for (int y = -filterSize; y <= filterSize; y++) {
            float2 offset = float2(x, y) * pixelScale;
            float z_j0 = GetDepth(coord + offset);
            float z_j1 = GetDepth(coord - offset);

            float z_j = max(z_j0, z_j1);

            if (z_j == 1.0f || z_j <= 0.0f) {
                continue;
            }

            float weight = GaussianWeight(coord, coord + offset, filterSize);
            sum += weight * Clamping(z_i, z_j, low, high);
            weightSum += weight;
        }
    }

    weightSum = max(weightSum, 0.0001f);
    return sum / weightSum;
}

PS_OUTPUT main(VS_OUTPUT input) {
    float clearValue = depth.SampleLevel(depthSampler, input.Texcoord, 0).r;
    if (clearValue == 1.0f) {
        // r is cleared to 1.f when we render the particles, so if it's 1.f, we're rendering nothing.
        discard;
    }

    if (-depth.SampleLevel(depthSampler, input.Texcoord, 0).z <= 0.f) {
        // Don't smooth negative depth values. (can happen when our camera is inside a particle and the height goes into the near plane
        discard;
    }

    PS_OUTPUT output;
    float filteredDepth = NarrowRangeFilter(input.Texcoord, particleRadius * 0.1, particleRadius * 0.04);
    // We have to convert back to clip-space depth.
    // Our depth texture stores the position of the pixel in view-space.
    float4 clipPoint = float4(depth.SampleLevel(depthSampler, input.Texcoord, 0).xyz, 1);
    clipPoint.z = filteredDepth;
    clipPoint = mul(clipPoint, matProj);

    filteredDepth = clipPoint.z / clipPoint.w;
    output.SmoothedDepth = float4(filteredDepth, 0, 0, 1);
    return output;
}