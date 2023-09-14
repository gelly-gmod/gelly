struct GS_OUTPUT {
	float4 Position : SV_Position;
	// View-space center position of the particle
	float4 Center : CENTER;
	float2 Texcoord : TEXCOORD;
};

struct VS_OUTPUT {
	float4 Pos : SV_Position;
	// This is what the GS consumes primarily, and it's just this vertex's position in view space to prevent things like the size being affected by the camera's position or distortion.
	float4 ViewPos : VIEWPOS;
};

struct PS_OUTPUT {
	float4 DepthLowCol : SV_TARGET0;
	float4 DepthHighCol : SV_TARGET1;
	float Depth : SV_DEPTH;
};