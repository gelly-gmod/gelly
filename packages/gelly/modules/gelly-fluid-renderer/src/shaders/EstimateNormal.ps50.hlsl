#include "FluidRenderCBuffer.hlsli"
#include "ScreenQuadStructs.hlsli"
#include "util/EyeToProjDepth.hlsli"

Texture2D InputDepth : register(t0);
SamplerState InputDepthSampler : register(s0);

float SampleNoDiscontinuity(float2 tex, float centerTapEye) {
    float2 depthTap = InputDepth.Sample(InputDepthSampler, tex);
	float depthTapProj = depthTap.r;
	float depthTapEye = -depthTap.g;

    if (depthTapProj >= 1.f) {
        return centerTapEye;
    }

    // We can use a gaussian to drive a blend between the two depths
    float depthDiff = (depthTapEye - centerTapEye) * g_ThresholdRatio;
    // We use a much smaller sigma to get a sharper blend,
    // if we don't then we still get depth discontinuities
    float weight = exp((-depthDiff * depthDiff) / 0.7f);

    return lerp(centerTapEye, depthTapEye, weight);
}

float3 WorldPosFromDepth(float2 tex, float centerTapEye) {
    float depth = SampleNoDiscontinuity(tex, centerTapEye);
    depth = EyeToProjDepth(depth);
    float4 pos = float4(tex.x * 2.0f - 1.0f, (1.0f - tex.y) * 2.0f - 1.0f, depth, 1.0f);
    pos = mul(g_InverseProjection, pos);
    pos = mul(g_InverseView, pos);
    pos.xyz /= pos.w;
    return pos.xyz;
}

float3 WorldPosFromDepthF(float2 tex, float depth) {
    float4 pos = float4(tex.x * 2.0f - 1.0f, (1.0f - tex.y) * 2.0f - 1.0f, depth, 1.0f);
    pos = mul(g_InverseProjection, pos);
    pos = mul(g_InverseView, pos);
    pos.xyz /= pos.w;
    return pos.xyz;
}

struct PS_OUTPUT {
    float4 PositiveNormal : SV_Target0;
};

PS_OUTPUT main(VS_OUTPUT input) {
    PS_OUTPUT output = (PS_OUTPUT)0;
    float2 centerTap = InputDepth.Sample(InputDepthSampler, input.Tex);
    if (centerTap.r >= 1.f) {
        discard;
    }

    // We perform our own taps
    float2 texelSize = 1.f / float2(g_ViewportWidth, g_ViewportHeight);
    float centerTapEye = -centerTap.g;

    /**    float c0 = texelFetch(depth,p           ,0).w;
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

    // float l2 = SampleNoDiscontinuity(input.Tex - float2(2, 0) * texelSize, zc);
    // float l1 = SampleNoDiscontinuity(input.Tex - float2(1, 0) * texelSize, zc);
    // float r1 = SampleNoDiscontinuity(input.Tex + float2(1, 0) * texelSize, zc);
    // float r2 = SampleNoDiscontinuity(input.Tex + float2(2, 0) * texelSize, zc);
    // float b2 = SampleNoDiscontinuity(input.Tex - float2(0, 2) * texelSize, zc);
    // float b1 = SampleNoDiscontinuity(input.Tex - float2(0, 1) * texelSize, zc);
    // float t1 = SampleNoDiscontinuity(input.Tex + float2(0, 1) * texelSize, zc);
    // float t2 = SampleNoDiscontinuity(input.Tex + float2(0, 2) * texelSize, zc);

    // float dl = abs(l1 * l2 / (2.0 * l2 - l1) - zc.r);
    // float dr = abs(r1 * r2 / (2.0 * r2 - r1) - zc.r);
    // float db = abs(b1 * b2 / (2.0 * b2 - b1) - zc.r);
    // float dt = abs(t1 * t2 / (2.0 * t2 - t1) - zc.r);

    // float3 ce = WorldPosFromDepthF(input.Tex, zc.r);

    // float3 dpdx = (dl < dr) ? ce - WorldPosFromDepthF(input.Tex - float2(1, 0) * texelSize, l1) :
    //     -ce + WorldPosFromDepthF(input.Tex + float2(1, 0) * texelSize, r1);
    // float3 dpdy = (db < dt) ? ce - WorldPosFromDepthF(input.Tex - float2(0, 1) * texelSize, b1) :
    //     -ce + WorldPosFromDepthF(input.Tex + float2(0, 1) * texelSize, t1);
    
    // float3 normal = -normalize(cross(dpdx, dpdy));

    float3 l1 = WorldPosFromDepth(input.Tex - float2(1, 0) * texelSize, centerTapEye);
    float3 r1 = WorldPosFromDepth(input.Tex + float2(1, 0) * texelSize, centerTapEye);
    float3 t1 = WorldPosFromDepth(input.Tex + float2(0, 1) * texelSize, centerTapEye);
    float3 b1 = WorldPosFromDepth(input.Tex - float2(0, 1) * texelSize, centerTapEye);

    float3 dpdx = r1 - l1;
    float3 dpdy = t1 - b1;

    float3 normal = -normalize(cross(dpdx, dpdy));

	if (isnan(normal.x) || isnan(normal.y) || isnan(normal.z)) {
		discard;
	}

    output.PositiveNormal = float4(normal, centerTapEye);
    return output;
}