#include "FluidRenderCBuffer.hlsli"
#include "ScreenQuadStructs.hlsli"
#include "util/EyeToProjDepth.hlsli"

Texture2D InputDepth : register(t0);
SamplerState InputDepthSampler : register(s0);

struct PS_OUTPUT {
    float4 Color : SV_Target0;
    #ifdef AOV_ENABLED
    float4 AOV : SV_Target1;
    #endif
};

float sqr(float x) {
    return x * x;
}

float ComputeBlurScale() {
    float aspect = g_ViewportWidth / g_ViewportHeight;
    return g_ViewportWidth / aspect * g_Projection._m11;
}

float FetchEyeDepth(float2 pixel) {
    return InputDepth.Load(int3(pixel, 0)).r;
}

float FetchProjDepth(float2 pixel) {
    return InputDepth.Load(int3(pixel, 0)).g;
}

float CreateIsosurfaceDepth(float2 tex
    #ifdef AOV_ENABLED
    , out float4 aov : SV_Target1
    #endif
) {
    float2 inPosition = tex * float2(g_ViewportWidth, g_ViewportHeight);
    const float blurRadiusWorld = g_ParticleRadius * 0.5f;
    const float blurScale = ComputeBlurScale();
    const float blurFalloff = g_ThresholdRatio;

    float depth = FetchEyeDepth(inPosition);

    float blurDepthFalloff = g_ThresholdRatio;
    float maxBlurRadius = 10.0;

    float radius = 10.f; //min(maxBlurRadius, blurScale * (blurRadiusWorld / -depth));
    float radiusInv = 1.0 / radius;
    float taps = ceil(radius);
    float frac = taps - radius;

    float sum = 0.0;
    float wsum = 0.0;
    float count = 0.0;

    for (float y = -10.f; y <= 10.f; y += 1.0) {
        for (float x = -10.f; x <= 10.f; x += 1.0) {
            float2 offset = float2(x, y);
            float sample = FetchEyeDepth(inPosition + offset);
            if (FetchProjDepth(inPosition + offset) >= 1.f) {
                continue;
            }

            float r1 = length(float2(x, y)) * radiusInv;
            float w = exp(-(r1 * r1));

            float r2 = (sample - depth) * blurDepthFalloff;
            // 'g' is a Gaussian, but it can have some artifacts.
            // To remedy it, we use a wider Gaussian, which is
            // derived from the parametric Gaussian, then
            // simplified to exp(-(r^2/4))
            float g = exp(-(r2 * r2) / 8);
            float wBoundary = step(radius, max(abs(x), abs(y)));
            float wFrac = 1.0 - wBoundary * frac;

            sum += sample * w * g * wFrac;
            wsum += w * g * wFrac;
            count += g * wFrac;
        }
    }

    if (wsum > 0.0) {
        sum /= wsum;
    }

#ifdef AOV_ENABLED
    aov = float4(sum, wsum, count, 1.0);
#endif
    float blend = count / sqr(2.0 * radius + 1.0);
    return lerp(depth, sum, blend);
}


PS_OUTPUT main(VS_OUTPUT input) {
    PS_OUTPUT output = (PS_OUTPUT)0;
    float4 original = InputDepth.SampleLevel(InputDepthSampler, input.Tex, 0);
    if (original.g >= 1.f) {
        discard;
    }

    float eyeDepth = CreateIsosurfaceDepth(input.Tex
        #ifdef AOV_ENABLED
        , output.AOV
        #endif
    );

    float projDepth = EyeToProjDepth(eyeDepth);
    
    output.Color = float4(eyeDepth, projDepth, 0.f, 1.0f);
    return output;
}