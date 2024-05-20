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

float3x3 CreateQuadRotationMatrix(float3 worldPos, float3 viewVelocity) {
    // for a screen-facing quad, we can define a few constraints
    // 1. the quad should be parallel to the screen, aka
    //    an axis is facing the camera
    // 2. one of the axes should be aligned with the view velocity
    // 3. the third axis is the cross product of the other two

    float velocityLength = length(viewVelocity);
    if (velocityLength < 0.0001f) {
        return float3x3(1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f);
    }

    float3 y = viewVelocity / velocityLength;
    float3 z = normalize(mul(g_View, float4(normalize(worldPos - g_CameraPosition), 0.f)).xyz);
    float3 x = cross(z, y);

    // Remap to source engine coordinate system
    // x: forward, y: right, z: up
    // our camera-facing axis is the z axis
    // the velocity is the y axis
    // the x axis is the cross product of the other two
    // so we need to swap x and z
    return float3x3(x, y, z);
}

void PushVertex(VS_OUTPUT output, float2 corner, inout TriangleStream<GS_OUTPUT> triStream) {
    GS_OUTPUT vertex = (GS_OUTPUT)0;
    float3x3 rotation = CreateQuadRotationMatrix(output.WorldPos.xyz, output.ViewVelocity.xyz);

    vertex.Pos = output.Pos;
    float3 alignedCorner = float3(corner.x - 0.5, corner.y - 0.5, 0.0);
    vertex.Pos.xyz += mul(rotation, alignedCorner) * g_DiffuseScale * 2.f;

    vertex.Pos = mul(g_Projection, vertex.Pos);
    vertex.Tex = float2(corner.x, 1.0 - corner.y);
    vertex.ViewVelocity = float4(output.ViewVelocity.xyz, output.FrustrumLifetime.w);
    triStream.Append(vertex);
}

[maxvertexcount(4)]
void main(point VS_OUTPUT input[1], inout TriangleStream<GS_OUTPUT> triStream) {
    GS_OUTPUT output = (GS_OUTPUT)0;
    CullParticle(input[0].FrustrumLifetime.xy);

    PushVertex(input[0], corners[0], triStream);
    PushVertex(input[0], corners[1], triStream);
    PushVertex(input[0], corners[2], triStream);
    PushVertex(input[0], corners[3], triStream);
}