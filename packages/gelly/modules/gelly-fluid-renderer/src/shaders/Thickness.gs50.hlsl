#include "ThicknessStructs.hlsli"
#include "FluidRenderCBuffer.hlsli"

static float2 corners[4] = {
	float2(-1.f, 1.f),
	float2(-1.f, -1.f),
	float2(1.f, 1.f),
	float2(1.f, -1.f)
};

[maxvertexcount(4)]
void main(point VS_OUTPUT input[1], inout TriangleStream<GS_OUTPUT> stream) {
	float4 viewPosition = float4(input[0].Pos.xyz, 1.f);
	viewPosition = mul(g_View, viewPosition);

	[unroll]
	for (int i = 0; i < 4; i++) {
		float4 cornerPosition = viewPosition;
		cornerPosition.xy += corners[i] * g_ParticleRadius;
		cornerPosition = mul(g_Projection, cornerPosition);
		cornerPosition /= cornerPosition.w;

		GS_OUTPUT output = (GS_OUTPUT)0;
		output.Pos = cornerPosition;
		output.Tex = corners[i] * 0.5f + 0.5f;
		stream.Append(output);
	}
}