#include "ParticleRenderStages.hlsli"
#include "PerFrameCB.hlsli"

bool CalculateNormal(float2 texcoord, out float3 normal) {
	float aspectRatio = res.x / res.y;

	float2 ndcNormal = texcoord * float2(2.0, -2.0) + float2(-1.0, 1.0);
	float mag = dot(ndcNormal, ndcNormal);

	if (mag >= 1.f) {
		return false;
	}

	normal = float3(ndcNormal, sqrt(1.f - mag));
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

	float3 clipOrigin = mul(float4(input.Center.xyz, 1.f), matProj).xyz;

/*
    float z = dot(vec4(fragPos, 1.0), transpose(projectionMatrix)[2]);
    float w = dot(vec4(fragPos, 1.0), transpose(projectionMatrix)[3]);
    gl_FragDepth = 0.5 * (z / w + 1.0);
	*/

	float z = dot(float4(pointOnHemisphere, 1.f), transpose(matProj)[2]);
	float w = dot(float4(pointOnHemisphere, 1.f), transpose(matProj)[3]);
	clipDepth = 0.5 * (z / w + 1.0);
	float dist = distance(clipPoint.xyz, clipOrigin);
	output.DepthColor = float4(clipDepth, clipDepth, clipDepth, 1.f);
	output.Depth = clipDepth;

	return output;
}