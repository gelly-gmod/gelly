// Basic particle splatting geometry shader
// References:
// https://github.com/NVIDIAGameWorks/FleX/blob/master/demo/d3d/shaders/pointPS.hlsl
// https://stackoverflow.com/questions/8608844/resizing-point-sprites-based-on-distance-from-the-camera

cbuffer cbPerFrame : register(b0) {
	float2 res;
	float2 padding;
	float4x4 matProj;
	float4x4 matView;
#ifdef SHADERED
	float4x4 matGeo;
#endif
	float4x4 matInvProj;
};

struct GS_OUTPUT {
	float4 Position : SV_Position;
	float4 Center : CENTER;
	float2 Texcoord : TEXCOORD;
};

struct VS_OUTPUT {
	// We don't require a position output, but most renderers will shriek if
	// it's missing
	float4 Pos : SV_Position;
	// This is what the GS consumes primarily, and it's just this vertex's
	// position in view space to prevent things like the size being affected by
	// the camera's position or distortion.
	float4 ViewPos : VIEWPOS;
};

static const float2 corners[4] = {
	float2(0.0, 1.0), float2(0.0, 0.0), float2(1.0, 1.0), float2(1.0, 0.0)};

float4 ToClip(in float4 pos) { return mul(float4(pos.xyz, 1), matProj); }

[maxvertexcount(4)] void main(
	point VS_OUTPUT input[1], inout TriangleStream<GS_OUTPUT> stream
) {
// TODO: Make this a parameter in the per-frame constant buffer.
#ifdef SHADERED
	float particleScale = 0.3f;
#else
	float particleScale = 6.f;
#endif

	for (int i = 0; i < 4; i++) {
		float2 corner = corners[i];

		GS_OUTPUT output = (GS_OUTPUT)0;
		float4 cornerViewPos = input[0].ViewPos;
		cornerViewPos.xy += particleScale * (corner - float2(0.5, 0.5));
		output.Position = ToClip(cornerViewPos);
		output.Center = input[0].ViewPos;
		output.Texcoord = corner;

		stream.Append(output);
	}
}