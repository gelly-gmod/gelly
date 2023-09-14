#include "ParticleRenderStages.hlsli"
#include "PerFrameCB.hlsli"

// Basic particle splatting geometry shader
// References:
// https://github.com/NVIDIAGameWorks/FleX/blob/master/demo/d3d/shaders/pointPS.hlsl
// https://stackoverflow.com/questions/8608844/resizing-point-sprites-based-on-distance-from-the-camera
// Mainly adapted from this paper:
// https://graphics.cs.kuleuven.be/publications/PSIRPBSD/PSIRPBSD_paper.pdf

static const float2 corners[4] = {
	float2(0.0, 1.0), float2(0.0, 0.0), float2(1.0, 1.0), float2(1.0, 0.0)};

[maxvertexcount(4)] void main(
	point VS_OUTPUT input[1], inout TriangleStream<GS_OUTPUT> stream
) {
	GS_OUTPUT output = (GS_OUTPUT)0;

	for (int i = 0; i < 4; ++i) {
		float2 corner = corners[i];

		float4 pos = input[0].ViewPos;
		float2 cornerScaled = (corner - 0.5f) * particleRadius;
		pos.xy += cornerScaled.xy;

		output.Position = mul(pos, matProj);
		output.Center = input[0].ViewPos; // we re-output this only because the pipeline will automatically do perspective divide and viewport transform which we dont want
		output.Texcoord = float2(corner.x, 1.0f - corner.y);

		stream.Append(output);
	}
}