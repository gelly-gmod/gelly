#include "NDCQuadStages.hlsli"
#include "PerFrameCB.hlsli"

struct PS_OUTPUT {
    float4 Normal : SV_TARGET0;
};

Texture2D depth : register(t0);
SamplerState depthSampler {
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

float3 WorldPosFromDepth(float2 uv, float depth) {
    uv.y = 1.0 - uv.y;
    float4 clipPos = float4(uv * 2 - 1, depth, 1);
    float4 viewPos = mul(clipPos, matInvProj);
    return viewPos.xyz / viewPos.w; 
}

float3 EstimateNormal(float2 texcoord) {
    float2 texelSize = 1.0 / res;
    /**
    float depth = texture_depth.SampleLevel(sampler_point_clamp, uv, 0).r;
float3 P = reconstructPosition(uv, depth, InverseViewProjection);
float3 normal = normalize(cross(ddx(P), ddy(P)));
*/  
    float depthCtr = depth.Sample(depthSampler, texcoord).r;
    float3 P = WorldPosFromDepth(texcoord, depthCtr);
    float3 Pdx = WorldPosFromDepth(texcoord + float2(texelSize.x, 0.f), depth.Sample(depthSampler, texcoord + float2(texelSize.x, 0.f)).r);
    float3 Pdy = WorldPosFromDepth(texcoord + float2(0.f, texelSize.y), depth.Sample(depthSampler, texcoord + float2(0.f, texelSize.y)).r);
    // Verify that these are correct, and that the normal is pointing in the right direction

    float3 normal = normalize(cross(Pdx - P, Pdy - P));

    // Check for invalid depth
    if (depthCtr == 0.f) {
        return float3(0.f, 0.f, 0.f);
    }
    
    float3 lightDir = normalize(float3(0.5, 0.5, 1.0));
    float NdotL = saturate(dot(normal, lightDir));
    float3 color = NdotL * float3(0.8, 0.8, 0.8);
    return color;
}

PS_OUTPUT main(VS_OUTPUT input) {
    PS_OUTPUT output;
    float depth = depth.Sample(depthSampler, input.Texcoord).r;
    if (depth == 0.f) {
        discard;
    }
    output.Normal = float4(EstimateNormal(input.Texcoord), 1.0);
    return output;
}