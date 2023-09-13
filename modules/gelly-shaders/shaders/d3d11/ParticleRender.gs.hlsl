#include "ParticleRenderStages.hlsli"

// Basic particle splatting geometry shader
// References:
// https://github.com/NVIDIAGameWorks/FleX/blob/master/demo/d3d/shaders/pointPS.hlsl
// https://stackoverflow.com/questions/8608844/resizing-point-sprites-based-on-distance-from-the-camera
// Mainly adapted from this paper:
// https://graphics.cs.kuleuven.be/publications/PSIRPBSD/PSIRPBSD_paper.pdf

cbuffer cbPerFrame : register(b0) {
	float2 res;
	float2 padding;
	float4x4 matProj;
	float4x4 matView;
#ifdef SHADERED
	float4x4 matGeo;
#endif
	float4x4 matInvProj;
	float4x4 matInvView;
};

static const float2 corners[4] = {
	float2(0.0, 1.0), float2(0.0, 0.0), float2(1.0, 1.0), float2(1.0, 0.0)};

static const float4x4 matViewProj = mul(matView, matProj);

float4 ToClip(in float4 pos) { return mul(pos, matProj); }

[maxvertexcount(4)] void main(
	point VS_OUTPUT input[1], inout TriangleStream<GS_OUTPUT> stream
) {
// TODO: Make this a parameter in the per-frame constant buffer.
#ifdef SHADERED
	float particleScale = 0.01f;
#else
	float particleScale = 4.5f;
#endif

	GS_OUTPUT output = (GS_OUTPUT)0;

	for (int i = 0; i < 4; ++i) {
		float2 corner = corners[i];

		float4 pos = input[0].ViewPos;
		float2 cornerScaled = (corner - 0.5f) * particleScale;
		pos.xy += cornerScaled.xy;

		output.Position = ToClip(pos);
		output.Center = ToClip(pos);
		output.Texcoord = float2(corner.x, 1.0f - corner.y);

		stream.Append(output);
	}
}