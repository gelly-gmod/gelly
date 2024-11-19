#include "NDCQuadStages.hlsli"

sampler2D compositeTex : register(s0);

struct PS_OUTPUT {
	float4 Color : SV_TARGET0;
}

bool IsMasked(float2 tex) {
	float4 color = tex2D(compositeTex, tex);
	return color.a >= 1.f;
}

PS_OUTPUT main(VS_INPUT input) {
	PS_OUTPUT output = (PS_OUTPUT)0;
	float4 color = tex2D(compositeTex, input.Tex);

	output.Color = float4(color.rg, 0.f, 1.f);
	return output;
}
