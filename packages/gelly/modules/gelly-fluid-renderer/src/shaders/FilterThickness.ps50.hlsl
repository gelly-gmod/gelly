// Finally, no crazy logic in here, just a simple gaussian blur!!

#include "FluidRenderCBuffer.hlsli"
#include "ScreenQuadStructs.hlsli"

Texture2D InputThickness : register(t0);
SamplerState InputThicknessSampler : register(s0);

struct PS_OUTPUT {
    float4 Color : SV_Target0;
};

// We do 2x2 gaussian blur
// Every pixel is 1/4 of the total weight
static const float4 weights = float4(
    0.25f, 0.25f, 0.25f, 0.25f
);


float4 SampleThickness(float2 tex, float4 original) {
    float4 frag = InputThickness.Sample(InputThicknessSampler, tex);

    return lerp(frag, original, 1.f - frag.a);
}

float4 FilterThickness(float2 tex) {
    float4 filteredThickness = float4(0.f, 0.f, 0.f, 0.f);

    // To sample a 2x2 we can just perform a texture gather
    float4 redPixels = InputThickness.GatherRed(InputThicknessSampler, tex);
    float4 greenPixels = InputThickness.GatherGreen(InputThicknessSampler, tex);
    float4 bluePixels = InputThickness.GatherBlue(InputThicknessSampler, tex);

    filteredThickness = float4(
        dot(redPixels, weights),
        dot(greenPixels, weights),
        dot(bluePixels, weights),
        1.f
    );

    return filteredThickness;
}

PS_OUTPUT main(VS_OUTPUT input) {
    PS_OUTPUT output = (PS_OUTPUT)0;
    float4 original = InputThickness.Sample(InputThicknessSampler, input.Tex);
    if (original.a == 0.f) {
        discard;
    }

    output.Color = float4(FilterThickness(input.Tex).xyz, 1.f);
    return output;
}