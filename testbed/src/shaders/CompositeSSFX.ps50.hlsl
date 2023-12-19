#include "NDCQuad.hlsli"

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


struct PS_OUTPUT {
    float4 Normal : SV_Target0;
    float4 Albedo : SV_Target1;
    float4 DepthOut : SV_Target2;
    float4 Positions : SV_Target3;
    float Depth : SV_Depth;
};

PS_OUTPUT main(VS_INPUT input)
{
    float4 depth = GellyDepth.Sample(GellyDepthSampler, input.Tex);
    if (depth.a == 0.0f) {
        discard;
    }

    PS_OUTPUT output = (PS_OUTPUT)0;
    output.Normal = float4(GellyNormal.Sample(GellyNormalSampler, input.Tex).xyz * 2.f - 1.f, 1.0f);
    output.Albedo = GellyAlbedo.Sample(GellyAlbedoSampler, input.Tex);
    output.DepthOut = float4(depth.y, depth.y, depth.y, 1.0f);
    output.Positions = GellyPositions.Sample(GellyPositionsSampler, input.Tex);
    output.Depth = depth.y;
    return output;
}
