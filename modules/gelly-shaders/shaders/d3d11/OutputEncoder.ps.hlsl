#include "NDCQuadStages.hlsli"
#include "SplitFloat.hlsli"

struct PS_OUTPUT {
    float4 EncodedDepth : SV_TARGET0;
    float4 EncodedNormal : SV_TARGET1;
};

// The "encoding" is just converting the internal 32-bit float textures to the 16-bit float textures

Texture2D InternalDepth : register(t0);
Texture2D InternalNormal : register(t1);
SamplerState InternalDepthSampler : register(s0);
SamplerState InternalNormalSampler : register(s1);

PS_OUTPUT main(VS_OUTPUT input) {
    PS_OUTPUT output;

    float depth = InternalDepth.Sample(InternalDepthSampler, input.Texcoord).r;
    output.EncodedDepth = float4(SplitFloat(depth), 1.f, 1.f);
    output.EncodedNormal = float4(InternalNormal.Sample(InternalNormalSampler, input.Texcoord).rgba);

    return output;
}