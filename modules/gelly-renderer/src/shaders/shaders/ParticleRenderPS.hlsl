cbuffer cbPerFrame : register(b0) {
	float2 res;
	float2 padding;
	float4x4 matProj;
	float4x4 matView;
};

struct GS_OUTPUT {
	float4 Position : SV_Position;
	float4 Center : CENTER;
	float2 Texcoord : TEXCOORD;
};

struct PS_OUTPUT {
	float4 DepthCol : SV_TARGET;
	float Depth : SV_Depth;
};

PS_OUTPUT main(GS_OUTPUT input) {
	// Project the center of the sphere to the screen
	float2 normal = input.Texcoord * float2(2.0, -2.0) + float2(-1.0, 1.0);
	float mag = dot(normal, normal);
	float3 fullNormal = float3(normal, sqrt(1.0 - mag));

	if (mag > 1.0) {
		discard;
	}

	float4 pointOnSphere = input.Center - float4(fullNormal, 0.0) * 0.1;
	pointOnSphere = mul(pointOnSphere, matProj);
	
	float depthOnSphere = pointOnSphere.z;
	PS_OUTPUT output;
	output.DepthCol = float4(depthOnSphere, depthOnSphere, depthOnSphere, 1.0);
	output.Depth = -depthOnSphere;

	return output;
}