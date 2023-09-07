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

bool CalculateNormal(float2 texcoord, out float3 normal) {
	float2 ndcNormal = texcoord * float2(2.0, -2.0) + float2(-1.0, 1.0);
	float mag = dot(ndcNormal, ndcNormal);

	if (mag > 1.0f) {
		return false;
	}

	normal = float3(ndcNormal, sqrt(1.0f - mag));
	return true;
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

float GetViewDepth(float3 viewPosition) {
	float4 clipPosition = mul(float4(viewPosition, 1.0f), matProj);
	return clipPosition.z / clipPosition.w;
}

float GetRasterizedDepth(float3 viewPosition) {
	float depth_eye = viewPosition.z;
	float nearZ = 3;
	float farZ = 28377.919921875;

	return -((farZ + nearZ) / (farZ - nearZ) * depth_eye - (2 * farZ * nearZ) / (farZ - nearZ)) / -depth_eye;
}

PS_OUTPUT main(GS_OUTPUT input) {
	float3 normal;
	if (!CalculateNormal(input.Texcoord, normal)) {
		discard;
	}

	float3 viewParticlePosition = NudgeParticleByNormal(input.Center.xyz, normal);

	PS_OUTPUT output = (PS_OUTPUT)0;
	float linearDepth = GetViewLinearDepth(viewParticlePosition);
	float rasterizedDepth = GetRasterizedDepth(viewParticlePosition);
	output.DepthCol = float4(rasterizedDepth, linearDepth, linearDepth, linearDepth);
	output.Depth = GetViewDepth(viewParticlePosition);

	return output;
}