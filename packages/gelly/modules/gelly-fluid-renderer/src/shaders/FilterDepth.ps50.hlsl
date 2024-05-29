#include "FluidRenderCBuffer.hlsli"
#include "ScreenQuadStructs.hlsli"
#include "util/EyeToProjDepth.hlsli"

Texture2D InputDepth : register(t0);
SamplerState InputDepthSampler : register(s0);

struct PS_OUTPUT {
    float4 Color : SV_Target0;
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

float CreateIsosurfaceDepth(float2 tex) {
    float2 inPosition = tex * float2(g_ViewportWidth, g_ViewportHeight);
    const float blurRadiusWorld = g_ParticleRadius * 0.5f;
    const float blurScale = ComputeBlurScale();
    const float blurFalloff = g_ThresholdRatio;

    float depth = FetchEyeDepth(inPosition);

    float blurDepthFalloff = g_ThresholdRatio;
    float maxBlurRadius = 5.0;

    float radius = 5.f; //min(maxBlurRadius, blurScale * (blurRadiusWorld / -depth));
    float radiusInv = 1.0 / radius;
    float taps = ceil(radius);
    float frac = taps - radius;

    float sum = 0.0;
    float wsum = 0.0;
    float count = 0.0;

    for (float y = -5.f; y <= 5.f; y += 1.0) {
        for (float x = -5.f; x <= 5.f; x += 1.0) {
            float2 offset = float2(x, y);
            float sample = FetchEyeDepth(inPosition + offset);

            float r1 = length(float2(x, y)) * radiusInv;
            float w = exp(-(r1 * r1));

            float r2 = (sample - depth) * blurDepthFalloff;
            float g = exp(-(r2 * r2));

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

    float blend = count / sqr(2.0 * radius + 1.0);
    return lerp(depth, sum, blend);
}


PS_OUTPUT main(VS_OUTPUT input) {
    PS_OUTPUT output = (PS_OUTPUT)0;
    float4 original = InputDepth.SampleLevel(InputDepthSampler, input.Tex, 0);
    if (original.g >= 1.f) {
        discard;
    }

    float eyeDepth = CreateIsosurfaceDepth(input.Tex);
    float projDepth = EyeToProjDepth(eyeDepth);
    
    output.Color = float4(eyeDepth, projDepth, 0.f, 1.0f);
    return output;
}