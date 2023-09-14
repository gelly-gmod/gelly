#include "ParticleRenderStages.hlsli"
#include "PerFrameCB.hlsli"
#include "SplitFloat.hlsli"

bool CalculateNormal(float2 texcoord, out float3 normal) {
	float2 ndcNormal = texcoord * float2(2.0, -2.0) + float2(-1.0, 1.0);
	float mag = dot(ndcNormal, ndcNormal);

	if (mag > 1.0f) {
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
		discard;
	}
	
	PS_OUTPUT output = (PS_OUTPUT)0;
	float3 pointOnHemisphere = GetPointOnHemisphere(input.Center.xyz, normal);
	float4 clipPoint = mul(float4(pointOnHemisphere, 1.f), matProj);
	float clipDepth = clipPoint.z / clipPoint.w;

	float2 depth = SplitFloat(clipDepth);
	output.DepthHighCol = float4(depth.x, 0.f, 0.f, 0.f);
	output.DepthLowCol = float4(depth.y, 0.f, 1.f, 1.f);
	output.Depth = clipDepth;

	return output;
}