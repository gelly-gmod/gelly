#include "FluidRenderCBuffer.hlsli"
#include "ScreenQuadStructs.hlsli"

Texture2D InputAlbedo : register(t0);
SamplerState InputAlbedoSampler : register(s0);

Texture2D InputThickness : register(t1);
SamplerState InputThicknessSampler : register(s1);

struct PS_OUTPUT {
	float4 Albedo : SV_Target0;
	float Thickness : SV_Target1;
};

static float gaussianKernel_3x3[9] = {
	1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
	2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
	1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f
};

PS_OUTPUT main(VS_OUTPUT input) {
	PS_OUTPUT output = (PS_OUTPUT)0;
	float2 uv = input.Tex;
	float2 texelSize = float2(1.0 / g_ViewportWidth, 1.0 / g_ViewportHeight);

	float3 albedoTaps[9] = {
		InputAlbedo.Sample(InputAlbedoSampler, uv + float2(-1, -1) * texelSize).rgb,
		InputAlbedo.Sample(InputAlbedoSampler, uv + float2(0, -1) * texelSize).rgb,
		InputAlbedo.Sample(InputAlbedoSampler, uv + float2(1, -1) * texelSize).rgb,
		InputAlbedo.Sample(InputAlbedoSampler, uv + float2(-1, 0) * texelSize).rgb,
		InputAlbedo.Sample(InputAlbedoSampler, uv + float2(0, 0) * texelSize).rgb,
		InputAlbedo.Sample(InputAlbedoSampler, uv + float2(1, 0) * texelSize).rgb,
		InputAlbedo.Sample(InputAlbedoSampler, uv + float2(-1, 1) * texelSize).rgb,
		InputAlbedo.Sample(InputAlbedoSampler, uv + float2(0, 1) * texelSize).rgb,
		InputAlbedo.Sample(InputAlbedoSampler, uv + float2(1, 1) * texelSize).rgb
	};

	float3 albedo = 0;
	[unroll]
	for (int i = 0; i < 9; i++) {
		albedo += albedoTaps[i] * gaussianKernel_3x3[i];
	}

	// For thickness we use a less expensive cross pattern

	float thicknessTaps[5] = {
		InputThickness.Sample(InputThicknessSampler, uv + float2(0, -1) * texelSize).r,
		InputThickness.Sample(InputThicknessSampler, uv + float2(-1, 0) * texelSize).r,
		InputThickness.Sample(InputThicknessSampler, uv + float2(0, 0) * texelSize).r,
		InputThickness.Sample(InputThicknessSampler, uv + float2(1, 0) * texelSize).r,
		InputThickness.Sample(InputThicknessSampler, uv + float2(0, 1) * texelSize).r
	};

	float thickness = 0;
	[unroll]
	for (int i_t = 0; i_t < 5; i_t++) {
		thickness += thicknessTaps[i_t];
	}

	thickness /= 5.0f;
	
	output.Albedo = float4(albedo, 1.0f);
	output.Thickness = thickness;
	return output;
}