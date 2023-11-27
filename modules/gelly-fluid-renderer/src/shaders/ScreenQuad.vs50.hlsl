#include "ScreenQuadStructs.hlsli"

VS_OUTPUT main(VS_OUTPUT input) {
    VS_OUTPUT output;
    output.Pos = input.Pos;
    output.Tex = input.Tex;
    return output;
}