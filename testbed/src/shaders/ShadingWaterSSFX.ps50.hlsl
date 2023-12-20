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

Texture2D OpaqueBackBuffer : register(t4);
SamplerState OpaqueBackBufferSampler {
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
    float4 albedo = Albedo.Sample(AlbedoSampler, input.Tex);
    bool isWater = albedo.a < 0.7f;
    if (!isWater) {
        discard;
    }


    float3 normal = Normal.Sample(NormalSampler, input.Tex).xyz;
    float3 specular = float3(0.0f, 0.0f, 0.0f);
    float3 pos = Position.Sample(PositionSampler, input.Tex).xyz;
    float3 eyeDir = normalize(eyePos - pos);

    for (uint lightIndex = 0; lightIndex < lightCount; ++lightIndex)
    {
        Light light = lights[lightIndex];

        // specular
        float3 lightDir = normalize(light.position - pos);
        float3 reflectionDir = reflect(-lightDir, normal);
        float specularStrength = pow(max(dot(reflectionDir, eyeDir), 0.0f), 32.0f);

        specular += light.color * light.power * specularStrength;
    }

    float3 transmission = float3(0.0f, 0.0f, 0.0f);

    // Try to find suitable refraction ray
    // note: we use a crude approximation, in fact you could call it entirely wrong
    // this is because we can't use the improved SS-refraction by Chris Wyman since
    // it's extremely expensive for us to render BOTH backface depth and backface normal, along with thickness and along with the frontface depth and frontface normal
    float2 transmittedUV = input.Tex + normal.xy * 0.03f;
    float3 transmittedRadiance = OpaqueBackBuffer.Sample(OpaqueBackBufferSampler, transmittedUV).xyz;
    transmittedRadiance *= albedo.xyz; // For water, aka fluids, the albedo is the absorption coefficients with beer's law

    float fresnel = min(1.f, SchlicksDielectric(dot(normal, eyeDir), 1.33f));

    if (fresnel >= 1.f)
    {
        fresnel = 0.f;
    }

    // later on specular will also consider environment (and MAYBE screen-space reflections)
    output.Color = float4((1.f - fresnel) * transmittedRadiance + fresnel * specular, 1.0f);
    return output;
}
