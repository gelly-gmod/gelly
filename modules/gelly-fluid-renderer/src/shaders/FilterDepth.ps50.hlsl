#include "FluidRenderCBuffer.hlsli"
#include "FilterDepthStructs.hlsli"

PS_OUTPUT main(VS_OUTPUT input) {
    PS_OUTPUT output = (PS_OUTPUT)0;
    output.Color = float4(1, 0, 0, 1);
    return output;
}