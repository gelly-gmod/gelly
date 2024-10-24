#include "FluidRenderCBuffer.hlsli"
#include "ScreenQuadStructs.hlsli"

Texture2D InputAlbedo : register(t0);
SamplerState InputAlbedoSampler : register(s0);

Texture2D InputThickness : register(t1);
SamplerState InputThicknessSampler : register(s1);

struct PS_OUTPUT {
	float4 Albedo : SV_Target0;
	float2 Thickness : SV_Target1;
};

static float gaussianKernel_3x3[9] = {
	1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
	2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
	1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f
};

// we extend out a max of 24 pixels, but to save on time we'll bake a jittered kernel since
// it's *really* hard to tell the pattern and it's not worth the time to compute it like the
// depth filter, where we have pass-correlated random noise such that we can't see the pattern
static float albedo_jitter[9] = {
	13.73, 21.31, 13.73,
	17.61, 8.32f, 11.61,
	3.73, 16.31, 9.73
};

PS_OUTPUT main(VS_OUTPUT input) {
	PS_OUTPUT output = (PS_OUTPUT)0;
	float2 uv = input.Tex;
	float2 texelSize = float2(1.0 / g_ViewportWidth, 1.0 / g_ViewportHeight);

	float3 albedoTaps[9] = {
		InputAlbedo.Sample(InputAlbedoSampler, uv + (float2(-1, -1) * albedo_jitter[0]) * texelSize).rgb,
	InputAlbedo.Sample(InputAlbedoSampler, uv + (float2(0, -1) * albedo_jitter[1]) * texelSize).rgb,
		InputAlbedo.Sample(InputAlbedoSampler, uv + (float2(1, -1) * albedo_jitter[2]) * texelSize).rgb,
		InputAlbedo.Sample(InputAlbedoSampler, uv + (float2(-1, 0) * albedo_jitter[3]) * texelSize).rgb,
		InputAlbedo.Sample(InputAlbedoSampler, uv + (float2(0, 0) * albedo_jitter[4]) * texelSize).rgb,
		InputAlbedo.Sample(InputAlbedoSampler, uv + (float2(1, 0) * albedo_jitter[5]) * texelSize).rgb,
		InputAlbedo.Sample(InputAlbedoSampler, uv + (float2(-1, 1) * albedo_jitter[6]) * texelSize).rgb,
		InputAlbedo.Sample(InputAlbedoSampler, uv + (float2(0, 1) * albedo_jitter[7]) * texelSize).rgb,
		InputAlbedo.Sample(InputAlbedoSampler, uv + (float2(1, 1) * albedo_jitter[8]) * texelSize).rgb
	};

	float3 albedo = 0;
	[unroll]
	for (int i = 0; i < 9; i++) {
		albedo += albedoTaps[i] * gaussianKernel_3x3[i];
	};

	float2 thicknessTaps[9] = {
		InputThickness.Sample(InputThicknessSampler, uv + float2(-1, -1) * texelSize).rg,
		InputThickness.Sample(InputThicknessSampler, uv + float2(0, -1) * texelSize).rg,
		InputThickness.Sample(InputThicknessSampler, uv + float2(1, -1) * texelSize).rg,
		InputThickness.Sample(InputThicknessSampler, uv + float2(-1, 0) * texelSize).rg,
		InputThickness.Sample(InputThicknessSampler, uv + float2(0, 0) * texelSize).rg,
		InputThickness.Sample(InputThicknessSampler, uv + float2(1, 0) * texelSize).rg,
		InputThickness.Sample(InputThicknessSampler, uv + float2(-1, 1) * texelSize).rg,
		InputThickness.Sample(InputThicknessSampler, uv + float2(0, 1) * texelSize).rg,
		InputThickness.Sample(InputThicknessSampler, uv + float2(1, 1) * texelSize).rg
	};

	float2 thickness = float2(0, 0);
	float centerAcceleration = thicknessTaps[4].g;
	float threshold = 400.f;
	[unroll]
	for (int i_t = 0; i_t < 9; i_t++) {
		float2 tap = thicknessTaps[i_t] * gaussianKernel_3x3[i_t];
		float acceleration = abs(centerAcceleration - tap.g);
		if (acceleration > threshold || tap.g <= 0) {
			tap.g = centerAcceleration * gaussianKernel_3x3[i_t];
		}

		thickness += tap;
	}
	
	output.Albedo = float4(albedo, 1.0f);
	output.Thickness = thickness;
	return output;
}