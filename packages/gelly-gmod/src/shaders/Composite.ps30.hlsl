#include "NDCQuadStages.hlsli"
#include "util/ReconstructHighBits.hlsli"

sampler2D depthTex : register(s0);
sampler2D normalTex : register(s1);

struct PS_OUTPUT {
    float4 Color : SV_TARGET0;
    float Depth : SV_DEPTH;
};

PS_OUTPUT main(VS_INPUT input) {
    PS_OUTPUT output = (PS_OUTPUT)0;
    output.Color = tex2D(normalTex, input.Tex);
    float4 depthFragment = tex2D(depthTex, input.Tex);
    output.Depth = ReconstructHighBits(depthFragment.y, depthFragment.z);
    return output;
}
