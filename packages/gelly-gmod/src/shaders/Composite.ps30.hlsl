#include "NDCQuadStages.hlsli"
#include "util/ReconstructHighBits.hlsli"
#include "material/FluidMaterial.hlsli"
#include "material/Absorption.hlsli"
#include "material/Schlicks.hlsli"
#include "source-engine/AmbientCube.hlsli"
#include "material/Diffuse.hlsli"

sampler2D depthTex : register(s0);
sampler2D normalTex : register(s1);
sampler2D positionTex : register(s2);
sampler2D backbufferTex : register(s3);
sampler2D thicknessTex : register(s4);
samplerCUBE cubemapTex : register(s5);
sampler2D absorptionTex : register(s6);
sampler2D backNormalTex : register(s7);

float4 eyePos : register(c0);
float4 refractAndCubemapStrength : register(c1);

struct CompositeLight {
    float4 LightInfo;
    float4 Position;
    float4 Enabled;
};

CompositeLight lights[2] : register(c2); // next reg that can be used is c8 (2 + 6)
float4 aspectRatio : register(c8);
float4 ambientCube[6] : register(c9);
FluidMaterial material : register(c15);

struct PS_OUTPUT {
    float4 Color : SV_TARGET0;
    float Depth : SV_DEPTH;
};

float3 ComputeSpecularRadianceFromLights(float3 position, float3 normal, float3 eyePos) {
    float3 radiance = float3(0.0, 0.0, 0.0);

    [unroll]
    for (int i = 0; i < 2; i++) {
        float3 lightDir = normalize(lights[i].Position.xyz - position);
        float3 reflectionDir = reflect(-lightDir, normal);
        float3 eyeDir = normalize(eyePos - position);
        float specularRadiance = pow(max(dot(reflectionDir, eyeDir), 0.0), 64.0) * 4.f; // Source-engine-like specular

        radiance += lights[i].LightInfo.xyz * specularRadiance * lights[i].Enabled.x;
    }

    return radiance;
}

float2 ApplyRefractionToUV(in float2 tex, in float thickness, in float3 normal) {
    return tex + normal.yx * TexRefractFromMaterial(material);
}

float3 SampleTransmission(in float2 tex, in float thickness, in float3 pos, in float3 eyeDir, in float3 normal, in float3 absorption) {
    float2 uv = ApplyRefractionToUV(tex, thickness, normal);
    float3 transmission = tex2D(backbufferTex, uv).xyz;
    transmission *= absorption;
    return transmission;
}

#define UNDERWATER_DEPTH_MINIMUM 0.7f
float4 Shade(VS_INPUT input, float projectedDepth) {
    if (projectedDepth <= UNDERWATER_DEPTH_MINIMUM) {
        float thickness = tex2D(thicknessTex, input.Tex).x;
        float3 absorption = ComputeAbsorption(NormalizeAbsorption(tex2D(absorptionTex, input.Tex).xyz, thickness), thickness);
        float3 transmission = tex2D(backbufferTex, input.Tex).xyz;

        return float4(transmission * absorption, 1.f); // simple underwater effect
    }

    float thickness = tex2D(thicknessTex, input.Tex).x;
    float3 absorption = ComputeAbsorption(NormalizeAbsorption(tex2D(absorptionTex, input.Tex).xyz, thickness), thickness);
    float3 position = tex2D(positionTex, input.Tex).xyz;
    float3 normal = tex2D(normalTex, input.Tex).xyz;
    
    float3 eyeDir = normalize(eyePos.xyz - position);
    float3 reflectionDir = reflect(-eyeDir, normal);

    float fresnel = Schlicks(max(dot(normal, eyeDir), 0.0), material.r_st_ior.z);

    float3 specular = texCUBE(cubemapTex, reflectionDir).xyz * refractAndCubemapStrength.y + ComputeSpecularRadianceFromLights(position, normal, eyePos.xyz);
    float3 diffuseIrradiance = SampleAmbientCube(ambientCube, normal);
    float3 diffuse = Fr_DisneyDiffuse(
        GetNdotV(normal, eyeDir),
        GetNdotL(normal, normal), // ambient light is coming from everywhere
        GetLdotH(normal, GetHalfwayDir(eyeDir, normal)),
        material.r_st_ior.x * material.r_st_ior.x
    ) * diffuseIrradiance * material.diffuseAlbedo;

    float3 specularTransmissionLobe = (1.f - fresnel) * SampleTransmission(input.Tex, thickness, position, eyeDir, normal, absorption) + fresnel * specular;
    // inverse fresnel is already applied to the diffuse lobe
    float3 diffuseSpecularLobe = diffuse + fresnel * specular;
        float3 roughLobe = (1.f - material.r_st_ior.x) * diffuseSpecularLobe + material.r_st_ior.x * diffuse;

    specularTransmissionLobe *= material.r_st_ior.y;
    roughLobe *= (1.f - material.r_st_ior.y);

    float3 weight = specularTransmissionLobe + roughLobe;
	return float4(weight, 1.f);
}

PS_OUTPUT main(VS_INPUT input) {
    float4 depthFragment = tex2D(depthTex, input.Tex);
    if (depthFragment.r >= 1.f) {
        discard;
    }
    
    PS_OUTPUT output = (PS_OUTPUT)0;
    output.Color = Shade(input, depthFragment.r);
    output.Depth = depthFragment.r;
    return output;
}
