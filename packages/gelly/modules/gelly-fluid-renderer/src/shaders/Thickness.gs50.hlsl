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
	float4 cullingNDCPos = mul(g_Projection, viewPosition);
	cullingNDCPos /= cullingNDCPos.w;

	// Note, it's actually important to cull the particles in the Z direction as well,
	// or else particles that are behind the camera will still be rendered.
	if (abs(cullingNDCPos.x) > 1.f || abs(cullingNDCPos.y) > 1.f || cullingNDCPos.z > 1.f) {
		return;
	}

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