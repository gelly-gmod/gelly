#include "FluidRenderCBuffer.hlsli"
#include "ScreenQuadStructs.hlsli"

Texture2D InputDepth : register(t0);
SamplerState InputDepthSampler : register(s0);

float3 WorldPosFromDepth(float2 tex) {
    float depth = InputDepth.Sample(InputDepthSampler, tex).g;
    float4 pos = float4(tex.x * 2.0f - 1.0f, (1.0f - tex.y) * 2.0f - 1.0f, depth, 1.0f);
    pos = mul(g_InverseProjection, pos);
    pos = mul(g_InverseView, pos);
    pos.xyz /= pos.w;
    return pos.xyz;
}

struct PS_OUTPUT {
    float4 PositiveNormal : SV_Target0;
    float4 WorldPosition : SV_Target1;
};

PS_OUTPUT main(VS_OUTPUT input) {
    PS_OUTPUT output = (PS_OUTPUT)0;
    float4 original = InputDepth.Sample(InputDepthSampler, input.Tex);
    if (original.a == 0.f) {
        discard;
    }

    // We perform our own taps
    float2 texelSize = 1.f / float2(g_ViewportWidth, g_ViewportHeight);

    float3 p0 = WorldPosFromDepth(input.Tex + float2(-texelSize.x, -texelSize.y));
    float3 p1 = WorldPosFromDepth(input.Tex + float2(0.f, -texelSize.y));
    float3 p2 = WorldPosFromDepth(input.Tex + float2(texelSize.x, -texelSize.y));
    float3 p3 = WorldPosFromDepth(input.Tex + float2(-texelSize.x, 0.f));
    
    // Check for a discontinuity

    float3 normal = -normalize(cross(p1 - p0, p3 - p0));

    output.PositiveNormal = float4(normal * 0.5f + 0.5f, 1.f);
    output.WorldPosition = float4(WorldPosFromDepth(input.Tex), 1.f);
    return output;
}