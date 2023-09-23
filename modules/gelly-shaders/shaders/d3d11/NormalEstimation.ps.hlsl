#include "NDCQuadStages.hlsli"
#include "PerFrameCB.hlsli"

struct PS_OUTPUT {
    float4 Normal : SV_TARGET0;
};

Texture2D depth : register(t0);
SamplerState depthSampler {
    Filter = MIN_MAG_LINEAR_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

float3 WorldPosFromDepth(float2 uv, float depth) {
    uv.y = 1.f - uv.y;
    float4 clipPos = float4(uv * 2.f - 1.f, depth, 1.0);
    float4 viewPos = mul(clipPos, matInvProj);
    viewPos /= viewPos.w;
    return viewPos.xyz;
}

float3 EstimateNormal(bool a, float2 texcoord) {
    float2 texelSize = 1.0 / res; 
    float depthCtr = depth.Sample(depthSampler, texcoord).r;
    float3 Pdx = ddx_fine(WorldPosFromDepth(texcoord, depthCtr));
    float3 Pdy = ddy_fine(WorldPosFromDepth(texcoord, depthCtr));
    float3 P = WorldPosFromDepth(texcoord, depthCtr);

    float3 normal = normalize(cross(Pdx, Pdy));

    float3 lightStart = mul(float4(eye, 1), matView);
    float3 lightEnd = mul(float4(eye + float3(0.3, 0.4, 0.1), 1), matView);
    float3 lightDir = normalize(lightEnd - lightStart);
    float NdotL = saturate(dot(normal, lightDir.xyz));
    float3 color = NdotL * float3(0.8, 0.8, 0.8);
    return color;
}

PS_OUTPUT main(VS_OUTPUT input) {
    PS_OUTPUT output;
    float4 posDepth = depth.Sample(depthSampler, input.Texcoord);

    output.Normal = float4(EstimateNormal(true, input.Texcoord), 1.0);
    return output;
}