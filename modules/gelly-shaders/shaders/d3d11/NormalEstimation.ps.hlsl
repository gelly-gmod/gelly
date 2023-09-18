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
    float4 clipPos = float4(uv * 2.f - 1.f, depth, 1.0);
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
    float depthCtr = depth.Sample(depthSampler, texcoord).r;
    float3 P = WorldPosFromDepth(texcoord, depthCtr);
    float Dxy = depth.Sample(depthSampler, texcoord + float2(texelSize.x, 0.f)).r;
    float Dyx = depth.Sample(depthSampler, texcoord + float2(0.f, texelSize.y)).r;
    if (depthCtr == 1 || Dxy == 1 || Dyx == 1) {
        discard;
    }

    float3 Pdx = WorldPosFromDepth(texcoord + float2(texelSize.x, 0.f), Dxy);
    float3 Pdy = WorldPosFromDepth(texcoord + float2(0.f, texelSize.y), Dyx);
    // Verify that these are correct, and that the normal is pointing in the right direction

    float3 normal = normalize(cross(Pdx - P, Pdy - P));

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

	// Do this outside of a shader in production...
    //fov = 2.0*atan( 1.0/prjMatrix[1][1] ) * 180.0 / PI;
	// float fov = 2.0 * atan(1.0 / matProj[1][1]) * 180.0 / 3.1415926535;
	// float4x4 modelViewMatrix = mul(matGeo, matView);
	// float screenAspect = res.x / res.y;
	// float2 aspect = float2(1.0f / res.x, screenAspect / res.y);
	// float2 invTexScale = aspect;
	// float2 clipToEye = float2(tan(fov * 0.5f) * screenAspect, tan(fov * 0.5f));

	// float2 uvCoord = float2(texcoord.x, texcoord.y);
	// float eyeDepth = depth.Sample(depthSampler, uvCoord).r;
	// if (eyeDepth >= 0.9999999f) { // Use a proper epsilon
	// 	discard;
	// }

    // float3 eyePos = viewportToEyeSpace(uvCoord, eyeDepth, clipToEye);

    // float3 zl = eyePos - viewportToEyeSpace(
	// 	uvCoord - float2(invTexScale.x, 0.f),
	// 	depth.Sample(depthSampler, uvCoord - float2(invTexScale.x, 0.f)).r,
	// 	clipToEye
	// );
    // float3 zr = viewportToEyeSpace(
	// 	uvCoord + float2(invTexScale.x, 0.f),
	// 	depth.Sample(depthSampler, uvCoord + float2(invTexScale.x, 0.f)).r,
	// 	clipToEye
	// ) - eyePos;
    // float3 zt = viewportToEyeSpace(
	// 	uvCoord + float2(0.f, invTexScale.y),
	// 	depth.Sample(depthSampler, uvCoord + float2(0.f, invTexScale.y)).r,
	// 	clipToEye
	// ) - eyePos;
    // float3 zb = eyePos - viewportToEyeSpace(
	// 	uvCoord - float2(0.f, invTexScale.y),
	// 	depth.Sample(depthSampler, uvCoord - float2(0.f, invTexScale.y)).r,
	// 	clipToEye
	// );

    // float3 dx = zl;
    // float3 dy = zt;

    // if (abs(zr.z) < abs(zl.z))
    //     dx = zr;

    // if (abs(zb.z) < abs(zt.z))
    //     dy = zb;

    // float3 normal = normalize(cross(dy, dx));

    // return normal * 0.5 + 0.5;
    float4 lightDir = mul(mul(float4(normalize(float3(0.5, 0.5, 1)), 0.f), matGeo), matView);
    float NdotL = saturate(dot(normal, -lightDir.xyz));
    float3 color = NdotL * float3(0.8, 0.8, 0.8);
    return color;
}

PS_OUTPUT main(VS_OUTPUT input) {
    PS_OUTPUT output;
    float4 posDepth = depth.Sample(depthSampler, input.Texcoord);

    output.Normal = float4(EstimateNormal(posDepth.xyz, input.Texcoord), 1.0);
    return output;
}