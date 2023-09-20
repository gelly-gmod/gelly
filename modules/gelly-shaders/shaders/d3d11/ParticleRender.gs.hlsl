#include "ParticleRenderStages.hlsli"
#include "PerFrameCB.hlsli"

const float2 corners[4] = {
    float2(0.0, 1.0), float2(0.0, 0.0), float2(1.0, 1.0), float2(1.0, 0.0)
};

[maxvertexcount(4)]
void main(point VS_OUTPUT input[1], inout TriangleStream<GS_OUTPUT> stream) {
    for (int i = 0; i < 4; ++i) {
        GS_OUTPUT output = (GS_OUTPUT)0;
        float2 corner = corners[i];
        float4 projectedPosition = input[0].ViewPos;
        projectedPosition.xy += (corner - 0.5f) * particleRadius;
        projectedPosition = mul(projectedPosition, matProj);
        output.Position = projectedPosition;
        output.Texcoord = float2(corner.x, 1.f - corner.y);
        output.CenterViewPos = input[0].ViewPos;

        stream.Append(output);
    }
}