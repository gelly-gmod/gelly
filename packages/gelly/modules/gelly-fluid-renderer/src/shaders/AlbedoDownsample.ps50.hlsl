#include "FluidRenderCBuffer.hlsli"
#include "ScreenQuadStructs.hlsli"

Texture2D InputAlbedo : register(t0);
SamplerState InputAlbedoSampler : register(s0);

struct PS_OUTPUT {
	float4 Albedo : SV_Target0;
};

PS_OUTPUT main(VS_OUTPUT input) {
	PS_OUTPUT output = (PS_OUTPUT)0;
	// Quick cross filter, its a little easier on the eyes
	float2 uv = input.Tex;
	float2 texelSize = 1.f / float2(g_ViewportWidth, g_ViewportHeight);

	float4 nx = InputAlbedo.Sample(InputAlbedoSampler, uv + float2(-texelSize.x, 0));
	float4 px = InputAlbedo.Sample(InputAlbedoSampler, uv + float2(texelSize.x, 0));
	float4 ny = InputAlbedo.Sample(InputAlbedoSampler, uv + float2(0, -texelSize.y));
	float4 py = InputAlbedo.Sample(InputAlbedoSampler, uv + float2(0, texelSize.y));
	float4 center = InputAlbedo.Sample(InputAlbedoSampler, uv);

	float4 outputColor = (nx + px + ny + py + center) / 5.f;
	output.Albedo = outputColor;
	return output;
}