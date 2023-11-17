#include "NDCQuad.hlsli"
#include "ShadingSSFXCBuffer.hlsli"
#include "lights/PointLight.hlsli"

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

Texture2D Position : register(t3);
SamplerState PositionSampler {
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
    float3 Li = float3(0.0f, 0.0f, 0.0f);
    float3 pos = Position.Sample(PositionSampler, input.Tex).xyz;


    Light light = lights[0];
    // pdf is one since we are using a point light
    Li += CalculateLightContribution(light.color, light.power, light.radius, light.position, pos);

    float3 shading = brdf * Li * CalculateCosineLaw(normal, light.position, pos);
    output.Color = float4(shading, 1.0f);
    return output;
}
