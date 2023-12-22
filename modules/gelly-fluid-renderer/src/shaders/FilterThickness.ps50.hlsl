// Finally, no crazy logic in here, just a simple gaussian blur!!

#include "FluidRenderCBuffer.hlsli"
#include "ScreenQuadStructs.hlsli"

Texture2D InputThickness : register(t0);
SamplerState InputThicknessSampler : register(s0);

struct PS_OUTPUT {
    float4 Color : SV_Target0;
};

static const float g_blurWeights[9] = {
    0.0625f, 0.125f, 0.0625f,
    0.125f, 0.25f, 0.125f,
    0.0625f, 0.125f, 0.0625f
};

float SampleThickness(float2 tex, float original) {
    float4 frag = InputThickness.Sample(InputThicknessSampler, tex);

    return lerp(frag.r, original, 1.f - frag.a);
}

float FilterThickness(float2 tex) {
    float filteredThickness = 0.f;
    float2 texelSize = 1.0f / float2(g_ViewportWidth, g_ViewportHeight);
    
    float original = InputThickness.Sample(InputThicknessSampler, tex).r * g_blurWeights[4];
    filteredThickness += original;

    filteredThickness += SampleThickness(tex + float2(-1, 0) * texelSize, original) * g_blurWeights[3];
    filteredThickness += SampleThickness(tex + float2(1, 0) * texelSize, original) * g_blurWeights[5];
    filteredThickness += SampleThickness(tex + float2(0, -1) * texelSize, original) * g_blurWeights[7];
    filteredThickness += SampleThickness(tex + float2(0, 1) * texelSize, original) * g_blurWeights[1];
    filteredThickness += SampleThickness(tex + float2(-1, -1) * texelSize, original) * g_blurWeights[6];
    filteredThickness += SampleThickness(tex + float2(1, -1) * texelSize, original) * g_blurWeights[8];
    filteredThickness += SampleThickness(tex + float2(-1, 1) * texelSize, original) * g_blurWeights[0];
    filteredThickness += SampleThickness(tex + float2(1, 1) * texelSize, original) * g_blurWeights[2];

    return filteredThickness;
}

PS_OUTPUT main(VS_OUTPUT input) {
    PS_OUTPUT output = (PS_OUTPUT)0;
    float4 original = InputThickness.Sample(InputThicknessSampler, input.Tex);
    if (original.a == 0.f) {
        discard;
    }

    output.Color = float4(FilterThickness(input.Tex), 0.f, 0.f, 1.f);

    return output;
}