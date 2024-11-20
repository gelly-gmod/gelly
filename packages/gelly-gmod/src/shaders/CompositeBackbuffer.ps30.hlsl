#include "NDCQuadStages.hlsli"

sampler2D depthTex : register(s0);
sampler2D backbufferTex : register(s1);

float3 viewport : register(c8);
struct PS_OUTPUT {
	float4 Color : SV_TARGET0;
};

PS_OUTPUT main(VS_INPUT input) {
	input.Tex += float2(0.5f / viewport.y, 0.5f / viewport.z);
	float depth = tex2D(depthTex, input.Tex).r;
	if (depth != 1.f) {
		discard; // don't overwrite the fluid
	}

	PS_OUTPUT output = (PS_OUTPUT)0;
	output.Color = float4(tex2D(backbufferTex, input.Tex).xyz, 0.f); // mark as alpha 0 for further processing
	return output;
}