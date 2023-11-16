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

Texture2D Depth : register(t2);
SamplerState DepthSampler {
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
    float3 normal = Normal.Sample(NormalSampler, input.Tex).xyz;
    float3 sunDir = normalize(float3(0.0f, 1.0f, 0.0f));
    float3 brdf = Albedo.Sample(AlbedoSampler, input.Tex).xyz / 3.14159265359f;
    float3 diffuse = brdf * max(0.0f, dot(normal, sunDir));
    float3 bounceLight = brdf * abs(dot(normal, sunDir));

    output.Color = float4(diffuse + bounceLight, 1.0f);
    
    return output;
}
