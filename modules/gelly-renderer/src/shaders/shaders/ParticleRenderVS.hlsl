cbuffer cbPerFrame : register(b0) {
	float2 res;
	float2 padding;
	float4x4 matProj;
	float4x4 matView;
	#ifdef SHADERED
	float4x4 matGeo;
	#endif
};

struct VS_OUTPUT {
	float4 Pos : SV_Position;
	// This is what the GS consumes primarily, and it's just this vertex's position in view space to prevent things like the size being affected by the camera's position or distortion.
	float4 ViewPos : VIEWPOS;
};

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