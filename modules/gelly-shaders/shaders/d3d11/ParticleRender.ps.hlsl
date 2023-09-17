#include "ParticleRenderStages.hlsli"
#include "PerFrameCB.hlsli"

bool CalculateNormal(float2 texcoord, out float3 normal) {
	float2 ndcNormal = texcoord * float2(2.0, -2.0) + float2(-1.0, 1.0);
	float mag = dot(ndcNormal, ndcNormal);

	if (mag >= 1.f) {
		return false;
	}

	normal = float3(ndcNormal, sqrt(1.0f - mag));
	return true;
}

float3 GetPointOnHemisphere(float3 viewCenter, float3 normal) {
	return viewCenter + normal * particleRadius;
}

PS_OUTPUT main(GS_OUTPUT input) {
	float3 normal;
	if (!CalculateNormal(input.Texcoord, normal)) {
		PS_OUTPUT output = (PS_OUTPUT)0;
		output.DepthColor = float4(0.f, 0.f, 0.f, 0.f);
		output.Depth = 1.f;
		return output;
	}
	
	PS_OUTPUT output = (PS_OUTPUT)0;
	float3 pointOnHemisphere = GetPointOnHemisphere(input.Center.xyz, normal);
	float4 clipPoint = mul(float4(pointOnHemisphere, 1.f), matProj);
	float clipDepth = clipPoint.z / clipPoint.w;

	float3 clipOrigin = mul(float4(input.Center.xyz, 1.f), matProj).xyz;

	output.DepthColor = float4(clipOrigin, clipDepth);
	output.Depth = clipDepth;

	return output;
}