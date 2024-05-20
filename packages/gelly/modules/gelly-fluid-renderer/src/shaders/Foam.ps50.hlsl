#include "FoamStructs.hlsli"
#include "FluidRenderCBuffer.hlsli"

void ClipQuadCorners(float magnitude) {
    if (magnitude > 1.0) {
        discard;
    }
}

void CalculateNormal(float2 tex, out float3 normal, out float magnitude) {
    normal.xy = tex * float2(2.0, -2.0) + float2(-1.0, 1.0);
    magnitude = dot(normal.xy, normal.xy);
    normal.z = sqrt(1.0 - magnitude);
}

PS_OUTPUT main(GS_OUTPUT input) {
    PS_OUTPUT output = (PS_OUTPUT)0;

    float3 normal;
    float magnitude;

    CalculateNormal(input.Tex, normal, magnitude);
    ClipQuadCorners(magnitude);

    float lifetime = input.ViewVelocity.w;
    float lifeTimeFade = lifetime / 2.f;
    float foamThickness = lifeTimeFade * normal.z * 0.15f;

    output.FoamEncoding = float4(0.f, input.Pos.z, 0.0, foamThickness);
    return output;
}