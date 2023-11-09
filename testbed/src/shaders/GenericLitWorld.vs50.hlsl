#include "GenericLitWorld.hlsli"
#include "WorldRenderCBuffer.hlsli"

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output;
    output.Pos = mul(mvp, float4(input.Pos, 1.0f));
    output.Normal = input.Normal;
    output.Color = float4(0.8, 0.8, 0.8, 1);

    return output;
}