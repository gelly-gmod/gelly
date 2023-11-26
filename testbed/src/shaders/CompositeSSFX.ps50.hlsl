#include "NDCQuad.hlsli"

Texture2D GellyDepth : register(t0);
SamplerState GellyDepthSampler {
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

float4 main(VS_INPUT input) : SV_Target
{
    float4 depth = GellyDepth.Sample(GellyDepthSampler, input.Tex);
    if (depth.a == 0.0f) {
        discard;
    }
    return depth;
}
