#include "NDCQuadStages.hlsli"
#include "PerFrameCB.hlsli"

struct PS_OUTPUT {
    float4 Normal : SV_TARGET0;
};

Texture2D depth : register(t0);
SamplerState depthSampler {
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

float3 WorldPosFromDepth(float3 clipOrigin, float2 uv, float depth) {
    float4 clipPos = float4(clipOrigin.xy + (uv * 2.0 - 1.0), depth, 1.0);
    float4 viewPos = mul(clipPos, matInvProj);
    return viewPos.xyz / viewPos.w;
}

float3 EstimateNormal(float3 clipOrigin, float2 texcoord) {
    float2 texelSize = 1.0 / res;
    /**
    float depth = texture_depth.SampleLevel(sampler_point_clamp, uv, 0).r;
float3 P = reconstructPosition(uv, depth, InverseViewProjection);
float3 normal = normalize(cross(ddx(P), ddy(P)));
*/  
    // float depthCtr = depth.Sample(depthSampler, texcoord).r;
    // float3 P = WorldPosFromDepth(texcoord, depthCtr);
    // float3 Pdx = WorldPosFromDepth(texcoord + float2(texelSize.x, 0.f), depth.Sample(depthSampler, texcoord + float2(texelSize.x, 0.f)).r);
    // float3 Pdy = WorldPosFromDepth(texcoord + float2(0.f, texelSize.y), depth.Sample(depthSampler, texcoord + float2(0.f, texelSize.y)).r);
    // // Verify that these are correct, and that the normal is pointing in the right direction

    // float3 normal = normalize(cross(Pdx - P, Pdy - P));

    // // Check for invalid depth
    // if (depthCtr == 0.f) {
    //     return float3(0.f, 0.f, 0.f);
    // }

    /**
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

    // float c0 = depth.Sample(depthSampler, texcoord).r;

    // float l2 = depth.Sample(depthSampler, texcoord - float2(texelSize.x * 2, 0.f)).r;
    // float l1 = depth.Sample(depthSampler, texcoord - float2(texelSize.x, 0.f)).r;
    // float r1 = depth.Sample(depthSampler, texcoord + float2(texelSize.x, 0.f)).r;
    // float r2 = depth.Sample(depthSampler, texcoord + float2(texelSize.x * 2, 0.f)).r;
    // float b2 = depth.Sample(depthSampler, texcoord - float2(0.f, texelSize.y * 2)).r;
    // float b1 = depth.Sample(depthSampler, texcoord - float2(0.f, texelSize.y)).r;
    // float t1 = depth.Sample(depthSampler, texcoord + float2(0.f, texelSize.y)).r;
    // float t2 = depth.Sample(depthSampler, texcoord + float2(0.f, texelSize.y * 2)).r;

    // float dl = abs(l1 * l2 / (2.0 * l2 - l1) - c0);
    // float dr = abs(r1 * r2 / (2.0 * r2 - r1) - c0);
    // float db = abs(b1 * b2 / (2.0 * b2 - b1) - c0);
    // float dt = abs(t1 * t2 / (2.0 * t2 - t1) - c0);

    // float3 ce = WorldPosFromDepth(texcoord, c0);

    // float3 dpdx = (dl < dr) ? ce - WorldPosFromDepth(texcoord - float2(texelSize.x, 0.f), l1) :
    //     -ce + WorldPosFromDepth(texcoord + float2(texelSize.x, 0.f), r1);
    // float3 dpdy = (db < dt) ? ce - WorldPosFromDepth(texcoord - float2(0.f, texelSize.y), b1) :
    //     -ce + WorldPosFromDepth(texcoord + float2(0.f, texelSize.y), t1);

    // float3 normal = normalize(cross(dpdx, dpdy));
    // // Consider the aspect ratio of the screen

    /**
    	float3 eyePos = viewportToEyeSpace(uvCoord, eyeZ);

	// finite difference approx for normals, can't take dFdx because
	// the one-sided difference is incorrect at shape boundaries
	float3 zl = eyePos - viewportToEyeSpace(uvCoord - float2(invTexScale.x, 0.0), depthTex.Sample(texSampler, uvCoord - float2(invTexScale.x, 0.0)).x);
	float3 zr = viewportToEyeSpace(uvCoord + float2(invTexScale.x, 0.0), depthTex.Sample(texSampler, uvCoord + float2(invTexScale.x, 0.0)).x) - eyePos;
	float3 zt = viewportToEyeSpace(uvCoord + float2(0.0, invTexScale.y), depthTex.Sample(texSampler, uvCoord + float2(0.0, invTexScale.y)).x) - eyePos;
	float3 zb = eyePos - viewportToEyeSpace(uvCoord - float2(0.0, invTexScale.y), depthTex.Sample(texSampler, uvCoord - float2(0.0, invTexScale.y)).x);

	float3 dx = zl;
	float3 dy = zt;

	if (abs(zr.z) < abs(zl.z))
		dx = zr;

	if (abs(zb.z) < abs(zt.z))
		dy = zb;
	
    */

    float3 eyePos = WorldPosFromDepth(clipOrigin, texcoord, depth.Sample(depthSampler, texcoord).a);

    float3 zl = eyePos - WorldPosFromDepth(clipOrigin, texcoord - float2(texelSize.x, 0.f), depth.Sample(depthSampler, texcoord - float2(texelSize.x, 0.f)).a);
    float3 zr = WorldPosFromDepth(clipOrigin, texcoord + float2(texelSize.x, 0.f), depth.Sample(depthSampler, texcoord + float2(texelSize.x, 0.f)).a) - eyePos;
    float3 zt = WorldPosFromDepth(clipOrigin, texcoord + float2(0.f, texelSize.y), depth.Sample(depthSampler, texcoord + float2(0.f, texelSize.y)).a) - eyePos;
    float3 zb = eyePos - WorldPosFromDepth(clipOrigin, texcoord - float2(0.f, texelSize.y), depth.Sample(depthSampler, texcoord - float2(0.f, texelSize.y)).a);

    float3 dx = zl;
    float3 dy = zt;

    if (abs(zr.z) < abs(zl.z))
        dx = zr;

    if (abs(zb.z) < abs(zt.z))
        dy = zb;
    
    float3 normal = normalize(cross(dx, dy));


    return normal * 0.5 + 0.5;    
    float3 lightDir = normalize(float3(0.5, 0.5, 1.0));
    float NdotL = saturate(dot(normal, lightDir));
    float3 color = NdotL * float3(0.8, 0.8, 0.8);
    return color;
}

PS_OUTPUT main(VS_OUTPUT input) {
    PS_OUTPUT output;
    float4 posDepth = depth.Sample(depthSampler, input.Texcoord);

    output.Normal = float4(EstimateNormal(posDepth.xyz, input.Texcoord), 1.0);
    return output;
}