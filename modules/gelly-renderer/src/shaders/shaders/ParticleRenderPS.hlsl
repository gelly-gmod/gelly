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

float LinearizeDepth(float z, float near, float far) {
	return (2.0f * near) / (far + near - z * (far - near));
}

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
	float particleScale = 6.f;
#endif

	float3 fullNormal = float3(normal, sqrt(1.0f - mag));
	fullNormal.y *= -1.0f;

	// Check for when the normal is invalid
	if (isnan(fullNormal.x) || isnan(fullNormal.y) || isnan(fullNormal.z)) {
		discard;
	}

	float4 pointOnSphere =
		input.Center + float4(fullNormal, 0.0) * particleScale;

	float zfar = 28377.919921875;
	float znear = 3;  // From GMod
	float depthOnSphere = pointOnSphere.y;

	PS_OUTPUT output = (PS_OUTPUT)0;

	float linearDepth = LinearizeDepth(-pointOnSphere.z, znear, zfar);

	output.DepthCol = float4(linearDepth, 0.f, 0.f, 1.f);
	output.Depth = pointOnSphere.z / pointOnSphere.w;

	return output;
}