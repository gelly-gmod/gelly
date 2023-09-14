#include "ParticleRenderStages.hlsli"
#include "PerFrameCB.hlsli"

VS_OUTPUT main(float4 pos : SV_Position) {
	VS_OUTPUT output;
	output.Pos = float4(pos.xyz, 1.f); // This is straight from FleX which uses the w component for other things, so we discard it here.
	output.ViewPos = mul(float4(pos.xyz, 1.f), matView);
#ifdef SHADERED
	// Multiply by both geometry transform and view
	output.ViewPos = mul(mul(float4(pos.xyz, 1.f), matGeo), matView);
#endif
	return output;
}