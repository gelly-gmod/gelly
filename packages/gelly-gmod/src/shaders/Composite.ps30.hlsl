#include "NDCQuadStages.hlsli"

sampler2D tex0 : register(s0);

struct PS_OUTPUT {
    float4 Color : SV_TARGET0;
};

PS_OUTPUT main(VS_INPUT input) {
    PS_OUTPUT output = (PS_OUTPUT)0;
    output.Color = tex2D(tex0, input.Tex);
    return output;
}
