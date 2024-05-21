#include "FoamStructs.hlsli"
#include "FluidRenderCBuffer.hlsli"

static const float2 corners[4] = {
    float2(0.0, 1.0), float2(0.0, 0.0),
    float2(1.0, 1.0), float2(1.0, 0.0)
};

void CullParticle(float2 ndcPos) {
    if (any(abs(ndcPos)) > 1.0) {
        discard;
    }
}

float sqr(float x) {
    return x * x;
}

[maxvertexcount(4)]
void main(point VS_OUTPUT input[1], inout TriangleStream<GS_OUTPUT> triStream) {
    GS_OUTPUT output = (GS_OUTPUT)0;
    CullParticle(input[0].FrustrumLifetime.xy);

    float3 viewOrigin = input[0].Pos.xyz; // VS multiplied by g_View
    float3 viewVelocity = input[0].ViewVelocity.xyz;

    float3 heightAxis = float3(0.f, g_DiffuseScale, 0.f);
    float3 widthAxis = float3(g_DiffuseScale, 0.f, 0.f);

    float particleLifetime = input[0].FrustrumLifetime.w;
    float spriteFade = lerp(1.f - 0.794, 1.f, min(1.f, particleLifetime * 0.25f));

    heightAxis *= spriteFade;
    widthAxis *= spriteFade;

    float velocityFade = 1.f / sqr(spriteFade);
    float viewVelLength = length(viewVelocity) * g_DiffuseMotionBlur;

    if (viewVelLength > 0.5f) {
        float newPointLength = max(g_DiffuseScale, viewVelLength * 0.016f); // todo: pass dt
        velocityFade = min(1.f, 2.f / (newPointLength / g_DiffuseScale));

        heightAxis = normalize(viewVelocity.xyz) * newPointLength;
        widthAxis = normalize(cross(heightAxis, float3(0.f, 0.f, -1.f))) * g_DiffuseScale;
    }

    output.ViewVelocity = float4(viewVelocity, velocityFade);
    output.LifeTime = particleLifetime;
    
    output.Pos = mul(g_Projection, float4(viewOrigin + heightAxis - widthAxis, 1.f));
    output.Tex = corners[0];
    triStream.Append(output);

    output.Pos = mul(g_Projection, float4(viewOrigin - heightAxis - widthAxis, 1.f));
    output.Tex = corners[1];
    triStream.Append(output);

    output.Pos = mul(g_Projection, float4(viewOrigin + heightAxis + widthAxis, 1.f));
    output.Tex = corners[2];
    triStream.Append(output);

    output.Pos = mul(g_Projection, float4(viewOrigin - heightAxis + widthAxis, 1.f));
    output.Tex = corners[3];
    triStream.Append(output);
}