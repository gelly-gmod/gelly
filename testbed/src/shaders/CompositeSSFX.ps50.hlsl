#include "NDCQuad.hlsli"
#include "math/BeersLaw.hlsli"

Texture2D GellyDepth : register(t0);
SamplerState GellyDepthSampler {
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

Texture2D GellyNormal : register(t1);
SamplerState GellyNormalSampler {
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

texture2D GellyAlbedo : register(t2);
SamplerState GellyAlbedoSampler {
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

texture2D GellyPositions : register(t3);
SamplerState GellyPositionsSampler {
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

texture2D GellyThickness : register(t4);
SamplerState GellyThicknessSampler {
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};

struct PS_OUTPUT {
    float4 Normal : SV_Target0;
    float4 Albedo : SV_Target1;
    float4 DepthOut : SV_Target2;
    float4 Positions : SV_Target3;
    float Depth : SV_Depth;
};

static const float MINIMUM_THICKNESS = 0.02f;
static const float3 ABSORPTION = float3(2.f, 2.f, 0.f);

PS_OUTPUT main(VS_INPUT input)
{
    float4 depth = GellyDepth.Sample(GellyDepthSampler, input.Tex);
    float4 normalFrag = GellyNormal.Sample(GellyNormalSampler, input.Tex);
    if (normalFrag.x == 0.f && normalFrag.y == 0.f && normalFrag.z == 0.f) {
        discard;
    }

    float3 normal = normalFrag.xyz;

    float thickness = GellyThickness.Sample(GellyThicknessSampler, input.Tex).x;
    if (thickness < MINIMUM_THICKNESS) {
        discard;
    }

    PS_OUTPUT output = (PS_OUTPUT)0;
    output.Normal = float4(normal * 2.f - 1.f, 1.0f);
    output.Albedo = float4(ComputeAbsorption(ABSORPTION, thickness), 0.5f);
    output.DepthOut = float4(depth.y, depth.y, depth.y, 1.0f);
    output.Positions = GellyPositions.Sample(GellyPositionsSampler, input.Tex);
    output.Depth = depth.y;
    return output;
}
