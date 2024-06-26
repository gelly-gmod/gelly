#include "FluidRenderCBuffer.hlsli"
#include "ScreenQuadStructs.hlsli"
#include "util/EyeToProjDepth.hlsli"
#include "util/WorldPosFromDepth.hlsli"

Texture2D InputBackDepth : register(t0);
Texture2D InputFrontDepth : register(t1);

struct PS_OUTPUT {
	float4 Thickness : SV_Target0;
};

// we want to clamp the thickness to a maximum distance
// so that when a fluid is viewed from another fluid at a distance,
// it doesnt appear to be infinitely thick
#define MAXIMUM_DISTANCE 30.f
float CalculateThickness(in float2 uv, in float frontDepth, in float backDepth) {
	float3 frontPos = WorldPosFromProjDepthF(uv, frontDepth);
	float3 backPos = WorldPosFromProjDepthF(uv, backDepth);
	return min(length(frontPos - backPos), MAXIMUM_DISTANCE);
}

PS_OUTPUT main(VS_OUTPUT input) {
	PS_OUTPUT output = (PS_OUTPUT)0;
	float2 viewport = float2(g_ViewportWidth, g_ViewportHeight);
	float frontDepth = InputFrontDepth.Load(int3(input.Tex.xy * viewport, 0)).r;

	if (frontDepth >= 1.f) {
		discard;
	}

	output.Thickness = CalculateThickness(input.Tex.xy, frontDepth, InputBackDepth.Load(int3(input.Tex.xy * viewport, 0)).r);
	return output;
}