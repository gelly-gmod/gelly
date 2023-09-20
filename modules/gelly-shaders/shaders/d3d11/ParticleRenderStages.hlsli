struct GS_OUTPUT {
	float4 Position : SV_Position;
	float2 Texcoord : TEXCOORD0;
	float4 CenterViewPos : TEXCOORD1;
};

struct VS_OUTPUT {
	float4 Pos : SV_Position;
	// This is what the GS consumes primarily, and it's just this vertex's position in view space to prevent things like the size being affected by the camera's position or distortion.
	float4 ViewPos : TEXCOORD0;
};

struct PS_OUTPUT {
	float4 DepthColor : SV_TARGET0;
};