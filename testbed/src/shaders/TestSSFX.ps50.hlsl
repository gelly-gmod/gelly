#include "NDCQuad.hlsli"

Texture2D Albedo : register(t0);
SamplerState AlbedoSampler {
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

Texture2D Normal : register(t1);
SamplerState NormalSampler {
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

struct PS_OUTPUT {
    float4 Color : SV_Target;
};

PS_OUTPUT main(VS_OUTPUT input)
{
    PS_OUTPUT output;
    output.Color = float4(Normal.Sample(NormalSampler, input.Tex).xyz, 1.0f);
    return output;
}
