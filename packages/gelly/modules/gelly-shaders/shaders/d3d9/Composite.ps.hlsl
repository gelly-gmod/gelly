struct VS_INPUT {
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};

struct PS_OUTPUT {
    float4 Col : SV_TARGET0;
    float Depth : SV_DEPTH;
};

float4x4 matView : register(c0);
float4x4 matProj : register(c4);
float4x4 matInvView : register(c8);
float4x4 matInvProj : register(c12);
float4 eyePos : register(c16);

sampler2D depthSampler : register(s0);
sampler2D normalSampler : register(s1);
sampler2D frameSampler : register(s2);
samplerCUBE cubeSampler : register(s3);

float LinearizeDepth(float z, float near, float far) {
    return (2.0f * near) / (far + near - z * (far - near));
}

float Remap(float value, float2 originalRange, float2 newRange) {
    return (value - originalRange.x) / (originalRange.y - originalRange.x) * (newRange.y - newRange.x) + newRange.x;
}

#define BREAKPOINT 0.98f
float ReconstructBrokenFloat(float low, float high) {
    return low * BREAKPOINT + high * (1.f - BREAKPOINT);
}

// Outside medium is always air, so that has an IOR of 1.0
// Inside medium is always water, so that has an IOR of 1.33
static const float IOR_AIR = 1.0;
static const float IOR_WATER = 1.33;
static const float R0 = pow((IOR_AIR - IOR_WATER) / (IOR_AIR + IOR_WATER), 2.0);

float ComputeFresnel(float cosTheta) {
    return R0 + (1.0f - R0) * pow(1.0f - cosTheta, 5.0);
}

PS_OUTPUT main(VS_INPUT input) {
    PS_OUTPUT output;

    float2 nudged = input.Tex;
    float4 depth = tex2D(depthSampler, nudged);
    float3 normal = tex2D(normalSampler, nudged).rgb;



    // float4 pos = float4(nudged.x * 2.f - 1.f, (1.f - nudged.y) * 2.f - 1.f, 1.f, 1);
    // pos = mul(transpose(matInvProj), pos);
    // pos.xyz /= pos.w;
    // pos = mul(transpose(matInvView), pos);

    // float3 viewDir = normalize(eyePos.xyz - pos.xyz);
    // // float3 refracted = refract(-viewDir, normal, 1.333f);

    // // float3 refractedPos = pos.xyz + refracted * 0.34f;
    // // float4 refractedPosView = mul(float4(refractedPos, 1), transpose(matView));
    // // float4 refractedPosProj = mul(refractedPosView, transpose(matProj));
    // // refractedPosProj.xyz /= refractedPosProj.w;
    // // float2 refractedPosUV = float2(refractedPosProj.x * 0.5f + 0.5f, -refractedPosProj.y * 0.5f + 0.5f);

    // // float4 refractedColor = tex2D(frameSampler, refractedPosUV);

    // float3 reflected = reflect(-viewDir, normal);
    // float cosTheta = dot(viewDir, normal);
    // float fresnel = ComputeFresnel(cosTheta);
    // fresnel = saturate(fresnel);

    // float3 reflectColor = texCUBE(cubeSampler, reflected).rgb;
    // // texCUBE can return an HDR value, so we need to tonemap it with a simple stupid clamp
    // reflectColor = saturate(reflectColor);
    // float3 diffuseColor = float3(1.0, 0.1, 0.1);

    // float3 finalColor = reflectColor * fresnel + diffuseColor * (1.f - fresnel);

    output.Col = float4(normal, 1.f);
    output.Depth = ReconstructBrokenFloat(depth.y, depth.x);
    return output;
}