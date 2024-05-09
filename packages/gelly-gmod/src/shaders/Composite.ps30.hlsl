#include "NDCQuadStages.hlsli"
#include "util/ReconstructHighBits.hlsli"

sampler2D depthTex : register(s0);
sampler2D normalTex : register(s1);
sampler2D positionTex : register(s2);
sampler2D backbufferTex : register(s3);
sampler2D thicknessTex : register(s4);
samplerCUBE cubemapTex : register(s5);
sampler2D absorptionTex : register(s6);
sampler2D depthBufferTex : register(s7);

float3 eyePos : register(c0);
float4 absorptionCoeffs : register(c1);
float refractionStrength : register(c2);
float4x4 invMVP : register(c3);

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

    float2 transmissionUV = input.Tex + normal.xy * refractionStrength;
    float3 transmission = tex2D(backbufferTex, transmissionUV).xyz;
    // apply inverse gamma correction
    transmission = pow(transmission, 2.2);
    transmission *= absorption;

    float3 weight = (1.f - fresnel) * transmission + fresnel * specular;
    return float4(weight, 1.0);
}

float3 cmrMap(float t) {
    const float3 c0 = float3(-0.046981,0.001239,0.005501);
    const float3 c1 = float3(4.080583,1.192717,3.049337);
    const float3 c2 = float3(-38.877409,1.524425,20.200215);
    const float3 c3 = float3(189.038452,-32.746447,-140.774611);
    const float3 c4 = float3(-382.197327,95.587531,270.024592);
    const float3 c5 = float3(339.891791,-100.379096,-212.471161);
    const float3 c6 = float3(-110.928480,35.828481,60.985694);
    return c0+t*(c1+t*(c2+t*(c3+t*(c4+t*(c5+t*c6)))));
}

float3 GrabWorldPosition(float2 tex) {
    float depth = tex2D(depthBufferTex, tex).x;
    float4 ndc = float4(tex.x * 2.f - 1.f, (1.f - tex.y) * 2.f - 1.f, depth, 1.f);
    float4 worldPos = mul(ndc, invMVP);
    worldPos /= worldPos.w;
    return worldPos.xyz;
}

PS_OUTPUT main(VS_INPUT input) {
    float4 depthFragment = tex2D(depthTex, input.Tex);
    if (depthFragment.g < 1.f) {
        PS_OUTPUT output = (PS_OUTPUT)0;
        output.Color = float4(tex2D(normalTex, input.Tex).xyz * 0.5f + 0.5f, 1.f);
        output.Depth = 0.f;
        return output;
    }
    
    PS_OUTPUT output = (PS_OUTPUT)0;
    float near = 3.f;
    float far = 23000.f;
    float testDepth = tex2D(depthBufferTex, input.Tex).x;
    float3 linearDepth = cmrMap(2.0 * near / (far + near - testDepth * (far - near)));

    // normal estimation
    float3 ddxWorldPos = GrabWorldPosition(input.Tex + float2(0.001, 0));
    float3 ddyWorldPos = GrabWorldPosition(input.Tex + float2(0, 0.001));
    float3 worldPos = GrabWorldPosition(input.Tex);
    float3 dx = ddxWorldPos - worldPos;
    float3 dy = ddyWorldPos - worldPos;
    float3 normal = -normalize(cross(dx, dy));

    float split = floor(input.Tex.x / 0.25f);
    if (split == 0.f) {
        output.Color = float4(tex2D(backbufferTex, input.Tex).xyz, 1.f);
    } else if (split == 1.f) {
        output.Color = float4(normal, 1.f);
    } else if (split == 2.f) {
        output.Color = float4(linearDepth, 1.f);
    } else {
        output.Color = float4(worldPos.xyz, 1.f);
    }

    output.Depth = 0.f;
    return output;
}
