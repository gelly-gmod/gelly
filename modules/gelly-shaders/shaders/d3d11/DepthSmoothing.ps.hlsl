#include "NDCQuadStages.hlsli"
#include "PerFrameCB.hlsli"

struct PS_OUTPUT {
    float4 SmoothedDepth : SV_Target0;
};

static const float2 pixelScale = 1.f / res;
static const int maxFilterSize = 5;

Texture2D depth : register(t0);
SamplerState depthSampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};

static const int targetFilterSize = 5;
static const float thresholdRatio = 2;
// The amount of times that the filter is ran. This is a tuning parameter, and the bilateral gaussian filter can lose its ability to preserve edges if this is too high.
static const int iterations = 1;

float ComputeWeight(float2 r, float twoSigma2) {
    return exp(-dot(r, r) / twoSigma2);
}

float ComputeWeight(float r, float twoSigma2) {
    return exp(-r * r / twoSigma2);
}

// Adapted from: https://github.com/ttnghia/RealTimeFluidRendering/blob/master/Shaders/filter-bigaussian.fs.glsl
// This is a bilateral gaussian filter that preserves edges, and I've also modified it a tad bit to work with GMod's Z-buffer.
// The original code is heavily optimized for Z values that are close to the [0, -40], but GMod's is usually around [0, -4000].
float BiGaussFilter2D(VS_OUTPUT input, float pixelDepth) {
    float2 blurRadius = float2(1.0 / res.x, 1.0 / res.y);

    float ratio = res.y / 2.0 / tan(45.0 / 2.0);
    float K = -targetFilterSize * ratio * particleRadius * 0.1f;
    int filterSize = min(maxFilterSize, int(ceil(K / pixelDepth)));

    float sigma = filterSize / 3.0f;
    float twoSigma2 = 2.0f * sigma * sigma;

    float threshold = particleRadius * thresholdRatio;
    float sigmaDepth = threshold / 3.0f;
    float twoSigmaDepth2 = 2.0f * sigmaDepth * sigmaDepth;

    float4 f_tex = float4(input.Texcoord.xyxy);
    float2 r = float2(0, 0);
    float4 sum4 = float4(pixelDepth, 0, 0, 0);
    float4 wsum4 = float4(1, 0, 0, 0);
    float4 sampleDepth;
    float4 w4_r;
    float4 w4_depth;
    float4 rDepth;

    for (int x = 1; x <= filterSize; ++x) {
        r.x += blurRadius.x;
        f_tex.x += blurRadius.x;
        f_tex.z -= blurRadius.x;
        float4 f_tex1 = f_tex.xyxy;
        float4 f_tex2 = f_tex.zwzw;

        for (int y = 1; y <= filterSize; ++y) {
            r.y += blurRadius.y;

            f_tex1.y += blurRadius.y;
            f_tex1.w -= blurRadius.y;
            f_tex2.y += blurRadius.y;
            f_tex2.w -= blurRadius.y;

            sampleDepth.x = depth.SampleLevel(depthSampler, f_tex1.xy, 0).z;
            sampleDepth.y = depth.SampleLevel(depthSampler, f_tex1.zw, 0).z;
            sampleDepth.z = depth.SampleLevel(depthSampler, f_tex2.xy, 0).z;
            sampleDepth.w = depth.SampleLevel(depthSampler, f_tex2.zw, 0).z;

            if (sampleDepth.x == 0.f || sampleDepth.y == 0.f || sampleDepth.z == 0.f || sampleDepth.w == 0.f) {
                continue;
            }

            float deltaDepth = abs(sampleDepth.x - pixelDepth);
            if (deltaDepth > threshold) {
                continue;
            }

            rDepth = sampleDepth - float4(pixelDepth, pixelDepth, pixelDepth, pixelDepth);
            w4_r = float4(ComputeWeight(blurRadius * r, twoSigma2), ComputeWeight(blurRadius * r, twoSigma2), ComputeWeight(blurRadius * r, twoSigma2), ComputeWeight(blurRadius * r, twoSigma2));
            w4_depth.x = ComputeWeight(rDepth.x, twoSigmaDepth2);
            w4_depth.y = ComputeWeight(rDepth.y, twoSigmaDepth2);
            w4_depth.z = ComputeWeight(rDepth.z, twoSigmaDepth2);
            w4_depth.w = ComputeWeight(rDepth.w, twoSigmaDepth2);

            sum4 += sampleDepth * w4_r * w4_depth;
            wsum4 += w4_r * w4_depth;
        }
    }

    float2 filterVal;
    filterVal.x = dot(sum4, float4(1, 1, 1, 1));
    filterVal.y = dot(wsum4, float4(1, 1, 1, 1));

    return filterVal.x / filterVal.y;
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
    float filteredDepth = 0.f;
    for (int i = 0; i < iterations; i++) {
        filteredDepth += BiGaussFilter2D(input, depth.SampleLevel(depthSampler, input.Texcoord, 0).z);
    }
    filteredDepth /= iterations;

    // We have to convert back to clip-space depth.
    // Our depth texture stores the position of the pixel in view-space.
    float4 clipPoint = float4(depth.SampleLevel(depthSampler, input.Texcoord, 0).xyz, 1);
    clipPoint.z = filteredDepth;
    clipPoint = mul(clipPoint, matProj);

    filteredDepth = clipPoint.z / clipPoint.w;

    output.SmoothedDepth = float4(filteredDepth, 0, 0, 1);
    return output;
}