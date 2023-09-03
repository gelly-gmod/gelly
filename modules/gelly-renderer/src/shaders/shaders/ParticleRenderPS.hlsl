cbuffer cbPerFrame : register(b0) {
	float2 res;
	float2 padding;
	float4x4 matProj;
	float4x4 matView;
	float4x4 matInvProj;
	float4x4 matInvView;
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

	if (mag > 1.0) {
		discard;
	}

	// TODO: Make this a parameter in the per-frame constant buffer.
	#ifdef SHADERED
	float particleScale = 0.3f;
	#else
	float particleScale = 8.f;
	#endif

	float3 fullNormal = float3(normal, sqrt(1.0f - mag));
	fullNormal.y *= -1.0f;
	
	float4 pointOnSphere = input.Center + float4(fullNormal, 0.0) * particleScale;
	pointOnSphere = mul(pointOnSphere, matProj);

	float depthOnSphere = pointOnSphere.z / pointOnSphere.w;
	PS_OUTPUT output;
	output.DepthCol = float4(fullNormal, pointOnSphere.z);
	output.Depth = depthOnSphere;

	return output;
}