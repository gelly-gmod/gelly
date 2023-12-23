#include "FluidRenderCBuffer.hlsli"
#include "ThicknessStructs.hlsli"

static const float THICKNESS_ADJUSTMENT = 0.005f;

[earlydepthstencil]
PS_OUTPUT main(GS_OUTPUT input) {
    PS_OUTPUT output = (PS_OUTPUT)0;
    
    float3 normal;
    normal.xy = input.Tex * float2(2.0, -2.0) + float2(-1.0, 1.0);
    float magnitude = dot(normal.xy, normal.xy);

    if (magnitude > 1.0) {
        discard;
    }

    normal.z = sqrt(1.0 - magnitude);

    float thicknessAtFragment = THICKNESS_ADJUSTMENT;
    output.Thickness = float4(thicknessAtFragment, thicknessAtFragment, thicknessAtFragment, 1.0f);
    return output;
}