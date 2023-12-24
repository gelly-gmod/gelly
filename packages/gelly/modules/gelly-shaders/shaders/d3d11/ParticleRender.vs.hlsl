#include "ParticleRenderStages.hlsli"
#include "PerFrameCB.hlsli"

struct VS_INPUT {
	float4 Pos : SV_Position;
	uint Index : SV_VertexID;
};

VS_OUTPUT main(VS_INPUT input) {
	VS_OUTPUT output;
	output.Pos = float4(input.Pos.xyz, 1.f); // This is straight from FleX which uses the w component for other things, so we discard it here.
	output.Index = (int)input.Index;
	output.ViewPos = mul(float4(input.Pos.xyz, 1.f), matView);
#ifdef SHADERED
	// Multiply by both geometry transform and view
	output.ViewPos = mul(mul(float4(input.Pos.xyz, 1.f), matGeo), matView);
#endif
	return output;
}