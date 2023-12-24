#include "GenericLitWorld.hlsli"
#include "WorldRenderCBuffer.hlsli"

PS_OUTPUT main(VS_OUTPUT input) {
    PS_OUTPUT output;
    output.Albedo = float4(input.Color.rgb, 1.f);
    output.Normal = input.Normal.xyz;
    output.Depth = input.Pos.z;
    float4 clipPos = float4(input.Pos.x / windowSize.x, 1.f - input.Pos.y / windowSize.y, input.Pos.z, 1.f);
    clipPos.xy = clipPos.xy * 2.f - 1.f;
    output.Position = mul(invMvp, clipPos);
    output.Position /= output.Position.w;
    return output;
}