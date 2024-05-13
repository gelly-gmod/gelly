#include "NDCQuadStages.hlsli"
#include "util/ReconstructHighBits.hlsli"

sampler2D depthTex : register(s0);
sampler2D normalTex : register(s1);
sampler2D positionTex : register(s2);
sampler2D backbufferTex : register(s3);
sampler2D thicknessTex : register(s4);
samplerCUBE cubemapTex : register(s5);
sampler2D absorptionTex : register(s6);

float3 eyePos : register(c0);
float refractionStrength : register(c1);

struct PS_OUTPUT {
    float4 Color : SV_TARGET0;
    float Depth : SV_DEPTH;
};

float3 ComputeAbsorption(float3 absorptionCoefficients, float distance) {
    return exp(-absorptionCoefficients * distance);
}

float Schlicks(float cosTheta, float refractionIndex) {
    float r0 = (1.0 - refractionIndex) / (1.0 + refractionIndex);
    r0 = r0 * r0;
    return r0 + (1.0 - r0) * pow(1.0 - cosTheta, 5.0);
}

float3 NormalizeAbsorption(float3 absorption, float thickness) {
    // cool technique here: since absorption will get darker as thickness increases, we can
    // normalize it so that from the top it looks the same as from the side

    return absorption / thickness;
}

float4 Shade(VS_INPUT input) {
    float thickness = tex2D(thicknessTex, input.Tex).x;

    float3 absorption = ComputeAbsorption(NormalizeAbsorption(tex2D(absorptionTex, input.Tex).xyz, thickness), thickness);

    float3 position = tex2D(positionTex, input.Tex).xyz;
    float3 normal = tex2D(normalTex, input.Tex).xyz;
    
    float3 eyeDir = normalize(eyePos - position);
    float3 reflectionDir = reflect(-eyeDir, normal);
    float3 specular = texCUBE(cubemapTex, reflectionDir).xyz;
    
    float fresnel = Schlicks(max(dot(normal, eyeDir), 0.0), 1.33);
    if (fresnel > 0.89f) {
        discard;
    }

    float2 transmissionUV = input.Tex + normal.zx * refractionStrength;
    float3 transmission = tex2D(backbufferTex, transmissionUV).xyz;
    // apply inverse gamma correction
    transmission = pow(transmission, 2.2);
    transmission *= absorption;

    float3 weight = (1.f - fresnel) * transmission + fresnel * specular;
    return float4(weight, 1.0);
}

PS_OUTPUT main(VS_INPUT input) {
    float4 depthFragment = tex2D(depthTex, input.Tex);
    if (depthFragment.g >= 1.f) {
        discard;
    }
    
    PS_OUTPUT output = (PS_OUTPUT)0;
    output.Color = Shade(input);
    output.Depth = depthFragment.g;
    return output;
}
