#include "NDCQuadStages.hlsli"

#define FXAA_PC 1
#define FXAA_HLSL_3 1
#define FXAA_QUALITY__PRESET 39
#define FXAA_GREEN_AS_LUMA 1
#define FXAA_EARLY_EXIT 1

#include "fxaa/fxaa.hlsli"

sampler2D compositeTex : register(s0);
sampler2D backbufferTex : register(s1);
float3 viewport : register(c8);

struct PS_OUTPUT {
	float4 Color : SV_TARGET0;
};

bool IsMasked(float4 color) {
	return color.a >= 1.f;
}

bool IsMaskedPixelOffset(float2 uv, float2 offset) {
	float viewportWidth = viewport.y;
	float viewportHeight = viewport.z;
	float2 invViewportSize = float2(1.f / viewportWidth, 1.f / viewportHeight);
	return IsMasked(tex2D(compositeTex, uv + offset * invViewportSize));
}

PS_OUTPUT main(VS_INPUT input) {
	PS_OUTPUT output = (PS_OUTPUT)0;
	// fix half-pixel offset
	input.Tex += float2(0.5f / viewport.y, 0.5f / viewport.z);
	float4 color = tex2D(compositeTex, input.Tex);

	// dilate mask by 3x3 kernel
	bool masks[9] = {
		IsMaskedPixelOffset(input.Tex, float2(-1, -1)),
		IsMaskedPixelOffset(input.Tex, float2(0, -1)),
		IsMaskedPixelOffset(input.Tex, float2(1, -1)),
		IsMaskedPixelOffset(input.Tex, float2(-1, 0)),
		IsMaskedPixelOffset(input.Tex, float2(0, 0)),
		IsMaskedPixelOffset(input.Tex, float2(1, 0)),
		IsMaskedPixelOffset(input.Tex, float2(-1, 1)),
		IsMaskedPixelOffset(input.Tex, float2(0, 1)),
		IsMaskedPixelOffset(input.Tex, float2(1, 1))
	};

	bool masked = false;

	[unroll]
	for (int i = 0; i < 9; i++) {
		masked = masked || masks[i];
	}

	if (!masked) {
		output.Color = tex2D(backbufferTex, input.Tex);
	} else {
		output.Color = FxaaPixelShader(
			input.Tex,
			float4(0, 0, 0, 0), // console
			compositeTex,
			compositeTex,
			compositeTex,
			float2(1.f / viewport.y, 1.f / viewport.z),
			float4(0, 0, 0, 0), // console
			float4(0, 0, 0, 0), // console
			float4(0, 0, 0, 0), // console
			1,
			0.063,
			0.0,
			8.0, // console
			0.125, // console
			0.05, // console
			float4(0, 0, 0, 0) // console
		);
	}

	return output;
}
