#include "NDCQuad.hlsli"

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output;
    output.Pos = input.Pos;
    output.Tex = input.Tex;
    return output;
}