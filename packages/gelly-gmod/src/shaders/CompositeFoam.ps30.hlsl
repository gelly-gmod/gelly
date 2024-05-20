#include "NDCQuadStages.hlsli"
#include "util/ReconstructHighBits.hlsli"

sampler2D foamTex : register(s0);

struct PS_OUTPUT {
    float4 Color : SV_TARGET0;
    float Depth : SV_DEPTH;
};

PS_OUTPUT main(VS_INPUT input) {
    float4 foam = tex2D(foamTex, input.Tex);
    if (foam.g >= 1.f) {
        discard;
    }
    
    PS_OUTPUT output = (PS_OUTPUT)0;
    output.Color = float4(foam.a, foam.a, foam.a, foam.a); // We do want it to blend with the background
    output.Depth = foam.g;
    return output;
}
