#include "NDCQuad.hlsli"

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output;
    output.Pos = input.Pos;
    output.Tex = float2(input.Tex.x, 1.0f - input.Tex.y);
    return output;
}