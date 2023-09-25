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

static const int fixedFilterRadius = 8;
static const int u_FilterSize = 5;
static const float thresholdRatio = 10;
static const float clampRatio = 1;

float ComputeWeight(float2 r, float twoSigma2) {
    return exp(-dot(r, r) / twoSigma2);
}

float ComputeWeight(float r, float twoSigma2) {
    return exp(-r * r / twoSigma2);
}

void ModifiedGaussianFilter1D(inout float sampleDepth, inout float weight, inout float weightOther, inout float upper, inout float lower, float lowerClamp, float threshold) {
    if (sampleDepth > upper) {
        weight = 0;
        weightOther = 0;
    }
    else {
        if (sampleDepth < lower) {
            sampleDepth = lowerClamp;
        } else {
            upper = max(upper, sampleDepth + threshold);
            lower = min(lower, sampleDepth - threshold);
        }
    }
}

void ModifiedGaussianFilter2D(inout float sampleDepth, inout float weight, inout float weightOther, inout float upper, inout float lower, float lowerClamp, float threshold) {
    if (sampleDepth > upper) {
        weight = 0;
        weightOther = 0;
    }
    else {
        if (sampleDepth < lower) {
            sampleDepth = lowerClamp;
        } else {
            upper = max(upper, sampleDepth + threshold);
            lower = min(lower, sampleDepth - threshold);
        }
    }
}

float Filter1D(VS_OUTPUT input, float pixelDepth, int filterDirection) {
    if (u_FilterSize == 0) {
        return pixelDepth;
    }

    float threshold = particleRadius * thresholdRatio;
    float ratio = res.y / 2.0 / tan(0.392699082f); // pi / 8
    float K = -u_FilterSize * ratio * particleRadius * 0.1f;
    int filterSize = min(maxFilterSize, int(ceil(K / pixelDepth)));
    
    float upper = pixelDepth + threshold;
    float lower = pixelDepth - threshold;
    float lowerClamp = pixelDepth - particleRadius * clampRatio;

    float sigma = filterSize / 3.0f;
    float twoSigma2 = 2.0f * sigma * sigma;

    float2 sum2 = float2(pixelDepth, 0);
    float2 wsum2 = float2(1, 0);
    float4 dtc = (filterDirection == 0) ? float4(pixelScale.x, 0, -pixelScale.x, 0) : float4(0, pixelScale.y, 0, -pixelScale.y);

    float4 fTex = float4(input.Texcoord.xy, input.Texcoord.xy);

    float2 r = float2(0, 0);
    float dr = dtc.x + dtc.y;

    float upper1 = upper;
    float upper2 = upper;

    float lower1 = lower;
    float lower2 = lower;

    float2 sampleDepth;
    float2 w2;

    for (int x = 1; x <= filterSize; ++x) {
        fTex += dtc;
        r += dr;

        sampleDepth.x = depth.SampleLevel(depthSampler, fTex.xy, 0).z;
        sampleDepth.y = depth.SampleLevel(depthSampler, fTex.zw, 0).z;

        w2 = float2(ComputeWeight(r, twoSigma2), ComputeWeight(r, twoSigma2));
        ModifiedGaussianFilter1D(sampleDepth.x, w2.x, w2.y, upper1, lower1, lowerClamp, threshold);
        ModifiedGaussianFilter1D(sampleDepth.y, w2.y, w2.x, upper2, lower2, lowerClamp, threshold);

        sum2 += sampleDepth * w2;
        wsum2 += w2;
    }

    float2 filterVal = float2(sum2.x, wsum2.x) + float2(sum2.y, wsum2.y);
    return filterVal.x / filterVal.y;
}

float Filter2D(VS_OUTPUT input, float pixelDepth) {
    if (u_FilterSize == 0) {
        return pixelDepth;
    }

    float2 blurRadius = float2(1.0 / res.x, 1.0 / res.y);
    float threshold = particleRadius * thresholdRatio;
    float ratio = res.y / 2.0 / tan(0.392699082f); // pi / 8
    float K = -u_FilterSize * ratio * particleRadius * 0.1f;
    int filterSize = min(maxFilterSize, int(ceil(K / pixelDepth)));

    float upper = pixelDepth + threshold;
    float lower = pixelDepth - threshold;
    float lowerClamp = pixelDepth - particleRadius * clampRatio;

    float sigma = filterSize / 3.0f;
    float twoSigma2 = 2.0f * sigma * sigma;

    float4 fTex = float4(input.Texcoord.xy, input.Texcoord.xy);

    float2 r = float2(0, 0);
    float4 sum4 = float4(pixelDepth, 0, 0, 0);
    float4 wsum4 = float4(1, 0, 0, 0);
    float4 sampleDepth;
    float4 w4;

    for (int x = 1; x <= filterSize; ++x) {
        r.x += blurRadius.x;
        fTex.x += blurRadius.x;
        fTex.z -= blurRadius.x;
        float4 fTex1 = fTex.xyxy;
        float4 fTex2 = fTex.zwzw;

        for (int y = 1; y <= filterSize; ++y) {
            fTex1.y += blurRadius.y;
            fTex1.w -= blurRadius.y;
            fTex2.y += blurRadius.y;
            fTex2.w -= blurRadius.y;

            sampleDepth.x = depth.SampleLevel(depthSampler, fTex1.xy, 0).z;
            sampleDepth.y = depth.SampleLevel(depthSampler, fTex1.zw, 0).z;
            sampleDepth.z = depth.SampleLevel(depthSampler, fTex2.xy, 0).z;
            sampleDepth.w = depth.SampleLevel(depthSampler, fTex2.zw, 0).z;

            r.y += blurRadius.y;
            w4 = float4(ComputeWeight(blurRadius * r, twoSigma2), ComputeWeight(blurRadius * r, twoSigma2), ComputeWeight(blurRadius * r, twoSigma2), ComputeWeight(blurRadius * r, twoSigma2));

            ModifiedGaussianFilter2D(sampleDepth.x, w4.x, w4.w, upper, lower, lowerClamp, threshold);
            ModifiedGaussianFilter2D(sampleDepth.y, w4.y, w4.z, upper, lower, lowerClamp, threshold);
            ModifiedGaussianFilter2D(sampleDepth.z, w4.z, w4.y, upper, lower, lowerClamp, threshold);
            ModifiedGaussianFilter2D(sampleDepth.w, w4.w, w4.x, upper, lower, lowerClamp, threshold);

            sum4 += sampleDepth * w4;
            wsum4 += w4;
        }
    }

    float2 filterVal = float2(
        dot(sum4, float4(1, 1, 1, 1)),
        dot(wsum4, float4(1, 1, 1, 1))
    );

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
    float filteredDepth;
    for (int i = 0; i < 4; i++) {
        filteredDepth += Filter2D(input, depth.SampleLevel(depthSampler, input.Texcoord, 0).z);
    }
    filteredDepth /= 4;

    // We have to convert back to clip-space depth.
    // Our depth texture stores the position of the pixel in view-space.
    if (input.Texcoord.x > 0.5) {
            float4 clipPoint = float4(depth.SampleLevel(depthSampler, input.Texcoord, 0).xyz, 1);
    clipPoint.z = filteredDepth;
    clipPoint = mul(clipPoint, matProj);

    filteredDepth = clipPoint.z / clipPoint.w;
    } else {
            float4 clipPoint = float4(depth.SampleLevel(depthSampler, input.Texcoord, 0).xyz, 1);
    clipPoint = mul(clipPoint, matProj);

    filteredDepth = clipPoint.z / clipPoint.w;
    }

    output.SmoothedDepth = float4(filteredDepth, 0, 0, 1);
    return output;
}