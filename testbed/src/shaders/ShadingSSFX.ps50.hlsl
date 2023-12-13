#include "NDCQuad.hlsli"
#include "WorldRenderCBuffer.hlsli"
#include "ShadingSSFXCBuffer.hlsli"
#include "lights/PointLight.hlsli"
#include "math/Schlicks.hlsli"

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
    float3 specular = float3(0.0f, 0.0f, 0.0f);
    float3 pos = Position.Sample(PositionSampler, input.Tex).xyz;
    float3 eyeDir = normalize(eyePos - pos);

    for (uint lightIndex = 0; lightIndex < lightCount; ++lightIndex)
    {
        Light light = lights[lightIndex];
        // while this is pretty crude, we apply cosine law to the light contribution
        // a correct implementation would to actually integrate over the hemisphere and
        // trace light rays. That would make Li naturally be a function of visibility and we could apply cosine law
        // to the result of the integration instead of the light contribution
        Li += CalculateLightContribution(light.color, light.power, light.radius, light.position, pos) * CalculateCosineLaw(normal, light.position, pos);

        // specular
        float3 lightDir = normalize(light.position - pos);
        float3 reflectionDir = reflect(-lightDir, normal);
        float specularStrength = pow(max(dot(reflectionDir, eyeDir), 0.0f), 32.0f);

        specular += light.color * specularStrength;
    }

    float3 diffuse = brdf * Li;
    float fresnel = SchlicksDielectric(dot(normal, eyeDir), 1.33f);
    float3 shading = fresnel * specular + (1.0f - fresnel) * diffuse;
    output.Color = float4(shading, 1.0f);
    return output;
}
