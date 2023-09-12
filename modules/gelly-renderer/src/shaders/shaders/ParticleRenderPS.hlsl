// References:
// Section 3.1 of https://graphics.cs.kuleuven.be/publications/PSIRPBSD/PSIRPBSD_paper.pdf
cbuffer cbPerFrame : register(b0) {
	float2 res;
	float2 padding;
	float4x4 matProj;
	float4x4 matView;
	float4x4 matInvProj;
	float4x4 matInvView;
	float3 eye;
	float padding2;
};

struct GS_OUTPUT {
	float4 Position : SV_Position;
	float4 Center : CENTER;
	float2 Texcoord : TEXCOORD;
};

struct PS_OUTPUT {
	float4 DepthLowCol : SV_TARGET0;
	float4 DepthHighCol : SV_TARGET1;
	float Depth : SV_DEPTH;
};

float LinearizeDepth(float z, float near, float far) {
	return (2.0f * near) / (far + near - z * (far - near));
}

bool CalculateNormal(float2 texcoord, out float3 normal) {
	float2 ndcNormal = texcoord * float2(2.0, -2.0) + float2(-1.0, 1.0);
	float mag = dot(ndcNormal, ndcNormal);

	if (mag > 1.0f) {
		return false;
	}

	normal = float3(ndcNormal, sqrt(1.0f - mag));
	return true;
}

#define BREAKPOINT 0.98f
float2 SplitFloat(float value) {
	float lowerRange = clamp(value, 0, BREAKPOINT) / BREAKPOINT;
	float upperRange = clamp(value - BREAKPOINT, 0, 1 - BREAKPOINT) / (1 - BREAKPOINT);

	return float2(upperRange, lowerRange);
}

float3 NudgeParticleByNormal(float3 viewCenter, float3 normal) {
	// TODO: Make this a parameter in the per-frame constant buffer.
	#ifdef SHADERED
		float particleScale = 0.3f;
	#else
		float particleScale = 6.f;
	#endif

	return viewCenter + normal * particleScale;
}

float GetViewLinearDepth(float3 viewPosition) {
	float4 clipPosition = mul(float4(viewPosition, 1.0f), matInvProj);
	float ndcDepth = clipPosition.z / clipPosition.w;

	// TODO: make this a parameter in the per-frame constant buffer.
	float nearZ = 3;	
	float farZ = 28377.919921875;
	return LinearizeDepth(ndcDepth, nearZ, farZ);
}

float GetViewDepth(float4 clipPosition) {
	return clipPosition.z / clipPosition.w;
}

float GetRasterizedDepth(float4 clipPosition) {
	return clipPosition.z / clipPosition.w;
}

PS_OUTPUT main(GS_OUTPUT input) {
	float3 normal;
	if (!CalculateNormal(input.Texcoord, normal)) {
		discard;
	}

	PS_OUTPUT output = (PS_OUTPUT)0;
	float rasterizedDepth = GetRasterizedDepth(input.Center);
	float2 depth = SplitFloat(rasterizedDepth);
	output.DepthHighCol = float4(depth.x, 0.f, 0.f, 0.f);
	output.DepthLowCol = float4(depth.y, 0.f, 1.f, 1.f);
	output.Depth =  GetViewDepth(input.Center);

	return output;
}