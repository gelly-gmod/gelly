#include "NDCQuadStages.hlsli"
#include "util/ReconstructHighBits.hlsli"

sampler2D depthTex : register(s0);
sampler2D normalTex : register(s1);
sampler2D positionTex : register(s2);
sampler2D backbufferTex : register(s3);
sampler2D thicknessTex : register(s4);

float3 eyePos : register(c0);
float4 absorptionCoeffs : register(c1);
float refractionStrength : register(c2);

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

float4 Shade(VS_INPUT input) {
    float3 sunDir = float3(-0.377821, 0.520026, 0.766044);
    float thickness = tex2D(thicknessTex, input.Tex).x;
    if (thickness < 0.02f) {
        discard;
    }

    float3 absorption = ComputeAbsorption(absorptionCoeffs.xyz, thickness);

    float3 position = tex2D(positionTex, input.Tex).xyz;
    float3 normal = tex2D(normalTex, input.Tex).xyz * 2.0f - 1.0f;

    float3 eyeDir = normalize(eyePos - position);
    float3 reflectionDir = reflect(-eyeDir, normal);
    // roughness is the inverse of the specular power
    float3 specular = pow(max(dot(reflectionDir, sunDir), 0.0), absorptionCoeffs.w) * 55.f;
    
    float fresnel = Schlicks(max(dot(normal, eyeDir), 0.0), 1.33);
    float2 transmissionUV = input.Tex + normal.xy * refractionStrength;
    float3 transmission = tex2D(backbufferTex, transmissionUV).xyz;
    // apply inverse gamma correction
    transmission = pow(transmission, 2.2);
    transmission *= absorption;

    float3 weight = (1.f - fresnel) * transmission + fresnel * specular;

    return float4(weight, 1.0);
}

PS_OUTPUT main(VS_INPUT input) {
    PS_OUTPUT output = (PS_OUTPUT)0;
    output.Color = Shade(input);
    float4 depthFragment = tex2D(depthTex, input.Tex);
    output.Depth = depthFragment.g;
    return output;
}
