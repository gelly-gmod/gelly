#include "NDCQuadStages.hlsli"

float3 viewport : register(c8);

VS_INPUT main(VS_INPUT input) {
    VS_INPUT output = (VS_INPUT)0;
    output.Position = input.Position;
    output.Tex = input.Tex;
    return output;
}