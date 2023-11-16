#include "GenericLitWorld.hlsli"
#include "WorldRenderCBuffer.hlsli"

PS_OUTPUT main(VS_OUTPUT input) {
    PS_OUTPUT output;
    output.Albedo = input.Color.xyz;
    output.Normal = input.Normal.xyz;
    output.Depth = input.Pos.z;
    return output;
}