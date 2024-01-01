#include "FluidRenderCBuffer.hlsli"
#include "SplattingStructs.hlsli"

PS_OUTPUT main(GS_OUTPUT input) {
    PS_OUTPUT output = (PS_OUTPUT)0;
    
    float3 normal;
    normal.xy = input.Tex * float2(2.0, -2.0) + float2(-1.0, 1.0);
    float magnitude = dot(normal.xy, normal.xy);

    if (magnitude > 1.0) {
        discard;
    }

    normal.z = sqrt(1.0 - magnitude);

    // We can calculate the depth from the normal by using the z component of the normal

    float4 ndcPosition = input.Pos;
    ndcPosition.xy /= float2(g_ViewportWidth, g_ViewportHeight);
    ndcPosition.xy = ndcPosition.xy * 2.0 - 1.0;
    ndcPosition.w = 1.0;
    float4 viewPosition = mul(g_InverseProjection, ndcPosition);
    viewPosition /= viewPosition.w;

    float4 nudgedPosition = viewPosition;
    nudgedPosition.z += normal.z * (g_ParticleRadius);
    float4 viewNudgedPosition = nudgedPosition;
    nudgedPosition = mul(g_Projection, nudgedPosition);

    float depth = nudgedPosition.z / nudgedPosition.w;

    output.ShaderDepth = float4(viewNudgedPosition.z, depth, 0.f, 1.f);
    output.Albedo = input.Absorption;
    output.Depth = depth;
    return output;
}