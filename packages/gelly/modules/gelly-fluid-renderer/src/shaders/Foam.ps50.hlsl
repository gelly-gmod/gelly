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
    normal.z = 1.0 - magnitude;
}

float sqr(float x) {
    return x * x;
}

PS_OUTPUT main(GS_OUTPUT input) {
    PS_OUTPUT output = (PS_OUTPUT)0;

    float3 normal;
    float magnitude;

    CalculateNormal(input.Tex, normal, magnitude);
    ClipQuadCorners(magnitude);

    float lifetime = input.LifeTime;
    float lifeTimeFade = min(1.f, lifetime * 0.125f);
    float velocityFade = input.ViewVelocity.w;

    float foamThickness = lifeTimeFade * velocityFade * sqr(normal.z) * 0.01f;

    output.FoamEncoding = float4(0.f, 0.f, foamThickness, input.Pos.z);
    output.Depth = input.Pos.z;
    return output;
}