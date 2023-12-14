#include "FluidRenderCBuffer.hlsli"
#include "ScreenQuadStructs.hlsli"

Texture2D InputDepth : register(t0);
SamplerState InputDepthSampler : register(s0);

struct PS_OUTPUT {
    float4 Color : SV_Target0;
};

static const float g_blurWeights[9] = {
    0.0625f, 0.125f, 0.0625f,
    0.125f, 0.25f, 0.125f,
    0.0625f, 0.125f, 0.0625f
};

float4 BlurDepth(float2 tex) {
    float4 color = 0.0f;
    float2 texelSize = 1.0f / float2(g_ViewportWidth, g_ViewportHeight);
    
    for (int i = 0; i < 9; ++i) {
        float2 offset = float2(i % 3 - 1, i / 3 - 1) * texelSize;
        float4 gausSample = InputDepth.Sample(InputDepthSampler, tex + offset);

        // Also, if the depth is too far away, we're going to assume that it's a discontinuity and discard it
        if (abs(gausSample.z - InputDepth.Sample(InputDepthSampler, tex).z) > 0.01f) {
            gausSample = InputDepth.Sample(InputDepthSampler, tex);
        }

        gausSample.xyz *= g_blurWeights[i];

        // if (gausSample.a == 0.f) {
        //     gausSample = InputDepth.Sample(InputDepthSampler, tex) * g_blurWeights[i];
        // }
        // branchless
        // we just take the original sample if the current sample is null
        gausSample = lerp(gausSample, InputDepth.Sample(InputDepthSampler, tex) * g_blurWeights[i], 1.f - gausSample.a);
        color += float4(gausSample.xyz, 1.0f);
    }

    return color;
}

PS_OUTPUT main(VS_OUTPUT input) {
    PS_OUTPUT output = (PS_OUTPUT)0;
    float4 original = InputDepth.Sample(InputDepthSampler, input.Tex);
    if (original.a == 0.f) {
        discard;
    }

    output.Color = float4(BlurDepth(input.Tex).xyz, 1.0f);
    return output;
}