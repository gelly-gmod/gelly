#include "FluidRenderCBuffer.hlsli"
#include "ScreenQuadStructs.hlsli"
#include "util/LowBitEncoding.hlsli"

Texture2D InputDepth : register(t0);
SamplerState InputDepthSampler : register(s0);

struct PS_OUTPUT {
    float4 Color : SV_Target0;
};

PS_OUTPUT main(VS_OUTPUT input) {
    PS_OUTPUT output;
    float4 depth = InputDepth.Sample(InputDepthSampler, input.Tex);
    output.Color = float4(depth.r, EncodeProjectionDepth(depth.g), 1.f);
    return output;
}