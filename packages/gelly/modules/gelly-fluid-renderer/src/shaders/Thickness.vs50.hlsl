#include "ThicknessStructs.hlsli"

VS_INPUT main(VS_INPUT input) {
    VS_INPUT output = (VS_INPUT)0;
    // This is pretty much just a pass-through shader (hence returning an input), but we do want to make sure that the w-component of the position is 1.0
    output.Pos = float4(input.Pos.xyz, 1.0f);
    return output;
}