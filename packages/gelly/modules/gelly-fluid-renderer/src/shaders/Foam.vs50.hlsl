#include "FoamStructs.hlsli"
#include "FluidRenderCBuffer.hlsli"

struct VS_INPUT {
    float4 Pos : SV_POSITION;
    float4 Velocity : VELOCITY;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    float4 ndcPos = mul(g_Projection, mul(g_View, float4(input.Pos.xyz, 1.f)));
    ndcPos /= ndcPos.w;

    output.FrustrumLifetime = float4(ndcPos.xy, 0.f, input.Pos.w);
    output.ViewVelocity = mul(g_View, float4(input.Velocity.xyz, 0.f));
    output.Pos = mul(g_View, float4(input.Pos.xyz, 1.f));
    output.WorldPos = float4(input.Pos.xyz, 1.f);

    return output;
}