#include "ParticleRenderStages.hlsli"
#include "PerFrameCB.hlsli"

PS_OUTPUT main(GS_OUTPUT input) {
    PS_OUTPUT output = (PS_OUTPUT)0;

    float3 normal;
    normal.xy = input.Texcoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
    float mag = dot(normal.xy, normal.xy);

    if (mag > 1.f) {
        discard;
    }

    normal.z = sqrt(1.0f - mag);

    // We need to write out the depth of the hemisphere calculated by `normal` to the shader depth buffer.
    // We do this by finding the height of the point on the hemisphere.
    float height = -normal.z * particleRadius;
    height *= input.Position.w; // Scales the height according to the perspective distortion.

    output.DepthColor = float4(height, 0, 0, 1);

    return output;
}