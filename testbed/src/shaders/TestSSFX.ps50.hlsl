#include "NDCQuad.hlsli"

struct PS_OUTPUT {
    float4 Color : SV_Target;
}

PS_OUTPUT main(VS_OUTPUT input)
{
    PS_OUTPUT output;
    output.Color = float4(1.0f, 0.0f, 0.0f, 1.0f);
    return output;
}
