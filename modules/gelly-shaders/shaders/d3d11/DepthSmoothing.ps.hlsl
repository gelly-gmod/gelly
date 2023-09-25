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

// float GetFilterSize(float z) {
//     // Authors of the paper use arbitrary scalars for the world filter size.
//     // Since in GMod, our particle scale has to be massive, I'm using a pretty small value compared to theirs.
//     float worldSize = 0.7 * particleRadius;

//     float numerator = res.y * worldSize;
//     float denominator = 2 * abs(z) * tan(fov / 2);

//     return 3 * (numerator / denominator);
// }

// float GaussianWeight(float2 i_coord, float2 j_coord, float standardDeviation) {
//     float power = -length(j_coord - i_coord);
//     power *= power;
//     power /= 2 * standardDeviation * standardDeviation;

//     return exp(power);
// }

// float ComputeWeight(float z_i, float z_j, float2 i_coord, float2 j_coord, float standardDeviation, float high) {
//     if (z_j > z_i + high) {
//         return 0.f;
//     }

//     return GaussianWeight(i_coord, j_coord, standardDeviation);
// }

// float Clamping(float z_i, float z_j, float low, float high) {
//     if (z_j >= z_i - high) {
//         return z_j;
//     }

//     return z_i - low;
// }

// float GetDepth(float2 coord) {
//     // View-space depth.
//     return -depth.SampleLevel(depthSampler, coord, 0).z;
// }

// float NarrowRangeFilter(float2 coord, float low, float high) {
//     float z_i = GetDepth(coord);
//     int filterSize = min(maxFilterSize, int(ceil(GetFilterSize(z_i))));

//     float sum = 0.0f;
//     float weightSum = 0.0f;

//     for (int x = -filterSize / 2; x <= filterSize / 2; x++) {
//         for (int y = -filterSize / 2; y <= filterSize / 2; y++) {
//             float2 offset = float2(x, y) * pixelScale;
//             float z_j = GetDepth(coord + offset);

//             float weight = ComputeWeight(z_i, z_j, coord, coord + offset, filterSize, high);
//             sum += weight * Clamping(z_i, z_j, low, high);
//             weightSum += weight;
//         }
//     }

//     weightSum = max(weightSum, 0.0001f);
//     return sum / weightSum;
// }

/*

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
const int   fixedFilterRadius = 4;
const float thresholdRatio    = 10.5;
const float clampRatio        = 1;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
float compute_weight1D(float r, float two_sigma2)
{
    return exp(-r * r / two_sigma2);
}

float compute_weight2D(vec2 r, float two_sigma2)
{
    return exp(-dot(r, r) / two_sigma2);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ModifiedGaussianFilter1D(inout float sampleDepth, inout float weight, inout float weight_other, inout float upper, inout float lower, float lower_clamp, float threshold)
{
    if(sampleDepth > upper) {
        weight = 0;
#ifdef FIX_OTHER_WEIGHT
        weight_other = 0;
#endif
    } else {
        if(sampleDepth < lower) {
            sampleDepth = lower_clamp;
        }
#ifdef RANGE_EXTENSION
        else {
            upper = max(upper, sampleDepth + threshold);
            lower = min(lower, sampleDepth - threshold);
        }
#endif
    }
}

void ModifiedGaussianFilter2D(inout float sampleDepth, inout float weight, inout float weight_other, inout float upper, inout float lower, float lower_clamp, float threshold)
{
    if(sampleDepth > upper) {
        weight = 0;
#ifdef FIX_OTHER_WEIGHT
        weight_other = 0;
#endif
    } else {
        if(sampleDepth < lower) {
            sampleDepth = lower_clamp;
        }
#ifdef RANGE_EXTENSION
        else {
            upper = max(upper, sampleDepth + threshold);
            lower = min(lower, sampleDepth - threshold);
        }
#endif
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
float filter1D(float pixelDepth)
{
    if(u_FilterSize == 0) {
        return pixelDepth;
    }

    vec2  blurRadius = vec2(1.0 / u_ScreenWidth, 1.0 / u_ScreenHeight);
    float threshold  = u_ParticleRadius * thresholdRatio;
    float ratio      = u_ScreenHeight / 2.0 / tan(PI_OVER_8);
    float K          = -u_FilterSize * ratio * u_ParticleRadius * 0.1f;
    int   filterSize = min(u_MaxFilterSize, int(ceil(K / pixelDepth)));

    float upper       = pixelDepth + threshold;
    float lower       = pixelDepth - threshold;
    float lower_clamp = pixelDepth - u_ParticleRadius * clampRatio;

    float sigma      = filterSize / 3.0f;
    float two_sigma2 = 2.0f * sigma * sigma;

    vec2 sum2  = vec2(pixelDepth, 0);
    vec2 wsum2 = vec2(1, 0);
    vec4 dtc   = (u_FilterDirection == 0) ? vec4(blurRadius.x, 0, -blurRadius.x, 0) : vec4(0, blurRadius.y, 0, -blurRadius.y);

    vec4  f_tex = f_TexCoord.xyxy;
    float r     = 0;
    float dr    = dtc.x + dtc.y;

    float upper1 = upper;
    float upper2 = upper;
    float lower1 = lower;
    float lower2 = lower;
    vec2  sampleDepth;
    vec2  w2;

    for(int x = 1; x <= filterSize; ++x) {
        f_tex += dtc;
        r     += dr;

        sampleDepth.x = texture(u_DepthTex, f_tex.xy).r;
        sampleDepth.y = texture(u_DepthTex, f_tex.zw).r;

        w2 = vec2(compute_weight1D(r, two_sigma2));
        ModifiedGaussianFilter1D(sampleDepth.x, w2.x, w2.y, upper1, lower1, lower_clamp, threshold);
        ModifiedGaussianFilter1D(sampleDepth.y, w2.y, w2.x, upper2, lower2, lower_clamp, threshold);

        sum2  += sampleDepth * w2;
        wsum2 += w2;
    }

    vec2 filterVal = vec2(sum2.x, wsum2.x) + vec2(sum2.y, wsum2.y);
    return filterVal.x / filterVal.y;
}

float filter2D(float pixelDepth)
{
    if(u_FilterSize == 0) {
        return pixelDepth;
    }

    vec2  blurRadius = vec2(1.0 / u_ScreenWidth, 1.0 / u_ScreenHeight);
    float threshold  = u_ParticleRadius * thresholdRatio;
    float ratio      = u_ScreenHeight / 2.0 / tan(PI_OVER_8);
    float K          = -u_FilterSize * ratio * u_ParticleRadius * 0.1f;
    int   filterSize = (u_DoFilter1D < 0) ? fixedFilterRadius : min(u_MaxFilterSize, int(ceil(K / pixelDepth)));

    float upper       = pixelDepth + threshold;
    float lower       = pixelDepth - threshold;
    float lower_clamp = pixelDepth - u_ParticleRadius * clampRatio;

    float sigma      = filterSize / 3.0f;
    float two_sigma2 = 2.0f * sigma * sigma;

    vec4 f_tex = f_TexCoord.xyxy;

    vec2 r     = vec2(0, 0);
    vec4 sum4  = vec4(pixelDepth, 0, 0, 0);
    vec4 wsum4 = vec4(1, 0, 0, 0);
    vec4 sampleDepth;
    vec4 w4;

    for(int x = 1; x <= filterSize; ++x) {
        r.x     += blurRadius.x;
        f_tex.x += blurRadius.x;
        f_tex.z -= blurRadius.x;
        vec4 f_tex1 = f_tex.xyxy;
        vec4 f_tex2 = f_tex.zwzw;

        for(int y = 1; y <= filterSize; ++y) {
            f_tex1.y += blurRadius.y;
            f_tex1.w -= blurRadius.y;
            f_tex2.y += blurRadius.y;
            f_tex2.w -= blurRadius.y;

            sampleDepth.x = texture(u_DepthTex, f_tex1.xy).r;
            sampleDepth.y = texture(u_DepthTex, f_tex1.zw).r;
            sampleDepth.z = texture(u_DepthTex, f_tex2.xy).r;
            sampleDepth.w = texture(u_DepthTex, f_tex2.zw).r;

            r.y += blurRadius.y;
            w4   = vec4(compute_weight2D(blurRadius * r, two_sigma2));

            ModifiedGaussianFilter2D(sampleDepth.x, w4.x, w4.w, upper, lower, lower_clamp, threshold);
            ModifiedGaussianFilter2D(sampleDepth.y, w4.y, w4.z, upper, lower, lower_clamp, threshold);
            ModifiedGaussianFilter2D(sampleDepth.z, w4.z, w4.y, upper, lower, lower_clamp, threshold);
            ModifiedGaussianFilter2D(sampleDepth.w, w4.w, w4.x, upper, lower, lower_clamp, threshold);

            sum4  += sampleDepth * w4;
            wsum4 += w4;
        }
    }

    vec2 filterVal;
    filterVal.x = dot(sum4, vec4(1, 1, 1, 1));
    filterVal.y = dot(wsum4, vec4(1, 1, 1, 1));
    return filterVal.x / filterVal.y;
}
*/

static const int fixedFilterRadius = 8;
static const int u_FilterSize = 32;
static const float thresholdRatio = 0.5;
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
    float ratio = res.y / 2.0 / tan(fov / 2);
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
        filteredDepth += Filter1D(input, depth.SampleLevel(depthSampler, input.Texcoord, 0).z, 0);
        filteredDepth += Filter1D(input, depth.SampleLevel(depthSampler, input.Texcoord, 0).z, 1);
    }
    filteredDepth /= 8.f;

    // We have to convert back to clip-space depth.
    // Our depth texture stores the position of the pixel in view-space.
    float4 clipPoint = float4(depth.SampleLevel(depthSampler, input.Texcoord, 0).xyz, 1);
    clipPoint.z = filteredDepth;
    clipPoint = mul(clipPoint, matProj);

    filteredDepth = clipPoint.z / clipPoint.w;
    output.SmoothedDepth = float4(filteredDepth, 0, 0, 1);
    return output;
}