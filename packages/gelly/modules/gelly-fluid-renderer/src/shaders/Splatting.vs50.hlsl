#include "SplattingStructs.hlsli"
#include "FluidRenderCBuffer.hlsli"
#include "util/SolveQuadratic.hlsli"

Buffer<float3> g_Absorption : register(t0);
Buffer<float> g_Acceleration : register(t1);

float DotInvW(float4 a, float4 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z - a.w*b.w;
}

void SolveForWidth(in float4x4 invClip, out float xMin, out float xMax) {
    float a1 = DotInvW(invClip[3], invClip[3]);
    float b1 = -2.f * DotInvW(invClip[0], invClip[3]);
    float c1 = DotInvW(invClip[0], invClip[0]);

    SolveQuadratic(a1, b1, c1, xMin, xMax);
}

void SolveForHeight(in float4x4 invClip, out float yMin, out float yMax) {
    float a2 = DotInvW(invClip[3], invClip[3]);
    float b2 = -2.f * DotInvW(invClip[1], invClip[3]);
    float c2 = DotInvW(invClip[1], invClip[1]);

    SolveQuadratic(a2, b2, c2, yMin, yMax);
}

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = float4(input.Pos.xyz, 1.f);

    float4 q1 = input.AnisotropyQ1;
    float4 q2 = input.AnisotropyQ2;
    float4 q3 = input.AnisotropyQ3;

    float4x4 quadric;
    quadric._m00_m10_m20_m30 = float4(q1.xyz * q1.w, 0);
    quadric._m01_m11_m21_m31 = float4(q2.xyz * q2.w, 0);
    quadric._m02_m12_m22_m32 = float4(q3.xyz * q3.w, 0);
    quadric._m03_m13_m23_m33 = float4(input.Pos.xyz, 1);

	output.Variance = max(max(q1.w, q2.w), q3.w);

    float4x4 invClip = mul(mul(g_Projection, g_View), quadric);
    // Solve for boundaries
    float xMin, xMax, yMin, yMax = 0;
    SolveForWidth(invClip, xMin, xMax);
    SolveForHeight(invClip, yMin, yMax);

    // Create the inverse quadric for fragment shader (although we need to flip the variance multiplication *then* transpose it)
    float4x4 invQuadric;
    invQuadric._m00_m10_m20_m30 = float4(q1.xyz / q1.w, 0);
    invQuadric._m01_m11_m21_m31 = float4(q2.xyz / q2.w, 0);
    invQuadric._m02_m12_m22_m32 = float4(q3.xyz / q3.w, 0);
    invQuadric._m03_m13_m23_m33 = float4(0, 0, 0, 1);

    invQuadric = transpose(invQuadric);
    // and now we can add the inverse of the input position like the normal quadric
    invQuadric._m03_m13_m23_m33 = -(mul(invQuadric, output.Pos));
    
    // pre-multiplied by the inverse view matrix so we can jump straight from view to parameter space
    invQuadric = mul(invQuadric, g_InverseView);

    // send down to geometry shader
    output.Bounds = float4(xMin, xMax, yMin, yMax);
    output.InvQuadric = invQuadric;

    // and to speed things up we'll pass down the NDC position for frustum culling in the GS
    float4 ndcPos = mul(g_Projection, mul(g_View, float4(input.Pos.xyz, 1.f)));
    output.NDCPos = ndcPos.xy / ndcPos.w;

	output.Absorption = g_Absorption[input.ID];
	output.Acceleration = g_Acceleration[input.ID];
    return output;
}