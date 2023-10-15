#include "DebugRenderStages.hlsli"
#include "DebugCB.hlsli"

PS_OUTPUT main(GS_OUTPUT input) {
    float3 normal;
    normal.xy = input.Texcoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
    float mag = dot(normal.xy, normal.xy);

    if (mag > 1.f) {
        discard;
    }

    normal.z = sqrt(1.0f - mag);

    PS_OUTPUT output = (PS_OUTPUT)0;
    output.Color = float4(input.Color.xyz, 1.f);

    return output;
}