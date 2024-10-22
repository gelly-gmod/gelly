#include "FluidRenderCBuffer.hlsli"
#include "SplattingStructs.hlsli"
#include "util/SolveQuadratic.hlsli"
#include "util/Half.hlsli"

float sqr(float x) {
    return x * x;
}

PS_OUTPUT main(GS_OUTPUT input) {
    PS_OUTPUT output = (PS_OUTPUT)0;
	
	float4x4 invQuadric = input.InvQuadric;
    float4 ndcPos = float4(
        mad(input.Pos.x, g_InvViewport.x * 2.f, -1.f),
        (1.f - input.Pos.y * g_InvViewport.y) * 2.f - 1.f,
        0.f,
        1.f
    );

    float4 viewDir = mul(g_InverseProjection, ndcPos);

    float4 dir = mul(invQuadric, float4(viewDir.xyz, 0.f));
    float4 origin = invQuadric._m03_m13_m23_m33;

    // Solve the quadratic equation
    float a = dot(dir.xyz, dir.xyz);
    float b = -mad(dir.w, origin.w, -dot(dir.xyz, origin.xyz));
    float c = dot(origin.xyz, origin.xyz) - sqr(origin.w);

    float minT, maxT;

	[branch]
    if (!SolveQuadratic(a, 2.f * b, c, minT, maxT)) {
        discard;
    }

	minT = max(minT, 0.f);
	clip(maxT);

    float3 eyePos = minT * viewDir.xyz;
    float4 rayNDCPos = mul(g_Projection, float4(eyePos, 1.f));

    float projectionDepth = rayNDCPos.z * rcp(rayNDCPos.w);
    float eyeDepth = eyePos.z;

	output.Absorption = float4(input.Absorption.xyz, 1.f);
	output.FrontDepth = float3(projectionDepth, -eyeDepth, length(input.Velocity));
	output.Thickness = 0.1f; // arbitrary value, gets added up to form the thickness
	output.Depth = projectionDepth;
    return output;
}
