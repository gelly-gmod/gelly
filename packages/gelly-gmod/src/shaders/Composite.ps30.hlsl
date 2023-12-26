#include "NDCQuadStages.hlsli"
#include "util/ReconstructHighBits.hlsli"

sampler2D depthTex : register(s0);
sampler2D normalTex : register(s1);
sampler2D positionTex : register(s2);
sampler2D backbufferTex : register(s3);
sampler2D thicknessTex : register(s4);

float3 eyePos : register(c0);
float pad0 : register(c1);

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
    float3 absorptionCoefficients = float3(0.3, 0.05, 0.3);
    float3 absorption = ComputeAbsorption(absorptionCoefficients, tex2D(thicknessTex, input.Tex).x);

    float3 position = tex2D(positionTex, input.Tex).xyz;
    float3 normal = tex2D(normalTex, input.Tex).xyz * 2.0f - 1.0f;

    float3 eyeDir = normalize(eyePos - position);
    float3 reflection = reflect(-eyeDir, normal);
    float3 specular = pow(max(dot(reflection, sunDir), 0.0), 32.0f);

    float fresnel = Schlicks(max(dot(normal, eyeDir), 0.0), 1.33);
    float2 transmissionUV = input.Tex + normal.xy * 0.03f;
    float3 transmission = tex2D(backbufferTex, transmissionUV).xyz;
    // apply inverse gamma correction
    transmission = pow(transmission, 2.2);

    float3 weight = (1.f - fresnel) * transmission + fresnel * specular;

    return float4(weight * absorption, 1.0);
}

PS_OUTPUT main(VS_INPUT input) {
    PS_OUTPUT output = (PS_OUTPUT)0;
    output.Color = Shade(input);
    float4 depthFragment = tex2D(depthTex, input.Tex);
    output.Depth = depthFragment.g;
    return output;
}
