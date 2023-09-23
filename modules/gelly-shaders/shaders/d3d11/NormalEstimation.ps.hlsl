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
    // float depthCtr = depth.Sample(depthSampler, texcoord).r;
    // float3 Pdx = ddx_fine(WorldPosFromDepth(texcoord, depthCtr));
    // float3 Pdy = ddy_fine(WorldPosFromDepth(texcoord, depthCtr));
    // float3 P = WorldPosFromDepth(texcoord, depthCtr);

    // float3 normal = normalize(cross(Pdx, Pdy));

    /*
        float c0 = texelFetch(depth,p           ,0).w;
    float l2 = texelFetch(depth,p-ivec2(2,0),0).w;
    float l1 = texelFetch(depth,p-ivec2(1,0),0).w;
    float r1 = texelFetch(depth,p+ivec2(1,0),0).w;
    float r2 = texelFetch(depth,p+ivec2(2,0),0).w;
    float b2 = texelFetch(depth,p-ivec2(0,2),0).w;
    float b1 = texelFetch(depth,p-ivec2(0,1),0).w;
    float t1 = texelFetch(depth,p+ivec2(0,1),0).w;
    float t2 = texelFetch(depth,p+ivec2(0,2),0).w;
    
    float dl = abs(l1*l2/(2.0*l2-l1)-c0);
    float dr = abs(r1*r2/(2.0*r2-r1)-c0);
    float db = abs(b1*b2/(2.0*b2-b1)-c0);
    float dt = abs(t1*t2/(2.0*t2-t1)-c0);
    
    vec3 ce = getPos(p,c0);

    vec3 dpdx = (dl<dr) ?  ce-getPos(p-ivec2(1,0),l1) : 
                          -ce+getPos(p+ivec2(1,0),r1) ;
    vec3 dpdy = (db<dt) ?  ce-getPos(p-ivec2(0,1),b1) : 
                          -ce+getPos(p+ivec2(0,1),t1) ;
                           return normalize(cross(dpdx,dpdy));
                          */

    float c0 = depth.Sample(depthSampler, texcoord).r;
    float l2 = depth.Sample(depthSampler, texcoord - texelSize * 2).r;
    float l1 = depth.Sample(depthSampler, texcoord - texelSize).r;
    float r1 = depth.Sample(depthSampler, texcoord + texelSize).r;
    float r2 = depth.Sample(depthSampler, texcoord + texelSize * 2).r;
    float b2 = depth.Sample(depthSampler, texcoord - texelSize * float2(0, 2)).r;
    float b1 = depth.Sample(depthSampler, texcoord - texelSize * float2(0, 1)).r;
    float t1 = depth.Sample(depthSampler, texcoord + texelSize * float2(0, 1)).r;
    float t2 = depth.Sample(depthSampler, texcoord + texelSize * float2(0, 2)).r;

    float dl = abs(l1 * l2 / (2.0 * l2 - l1) - c0);
    float dr = abs(r1 * r2 / (2.0 * r2 - r1) - c0);
    float db = abs(b1 * b2 / (2.0 * b2 - b1) - c0);
    float dt = abs(t1 * t2 / (2.0 * t2 - t1) - c0);

    float3 ce = WorldPosFromDepth(texcoord, c0);

    float3 dpdx = (dl < dr) ? ce - WorldPosFromDepth(texcoord - texelSize, l1) :
        -ce + WorldPosFromDepth(texcoord + texelSize, r1);
    float3 dpdy = (db < dt) ? ce - WorldPosFromDepth(texcoord - texelSize * float2(0, 1), b1) :
        -ce + WorldPosFromDepth(texcoord + texelSize * float2(0, 1), t1);
    
    // Checks if the derivative is valid
    if (length(dpdx) < 0.0001 || length(dpdy) < 0.0001) {
        return float3(0, 0, 0);
    }

    float3 normal = normalize(cross(dpdx, dpdy));
    // return normal * 0.5 + 0.5;
    float3 lightStart = mul(float4(eye, 1), matView);
    float3 lightEnd = mul(float4(eye + float3(0.3, 0.4, -0.9), 1), matView);
    float3 lightDir = normalize(lightEnd - lightStart);
    float NdotL = saturate(dot(normal, lightDir.xyz));
    float3 color = NdotL * float3(0.3, 0.3, 0.9);
    return color;
}

PS_OUTPUT main(VS_OUTPUT input) {
    PS_OUTPUT output;
    float4 posDepth = depth.Sample(depthSampler, input.Texcoord);

    output.Normal = float4(EstimateNormal(true, input.Texcoord), 1.0);
    return output;
}