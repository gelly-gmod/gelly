#include "DebugRenderStages.hlsli"
#include "DebugCB.hlsli"

static const float2 corners[4] = {
    float2(0.0, 1.0), float2(0.0, 0.0), float2(1.0, 1.0), float2(1.0, 0.0)
};

[maxvertexcount(4)]
void main(point VS_OUTPUT input[1], inout TriangleStream<GS_OUTPUT> stream) {
    float4 viewPos = mul(input[0].Pos, matView);

    for (int i = 0; i < 4; i++) {
        GS_OUTPUT output = (GS_OUTPUT)0;
        float2 corner = corners[i];
        float4 viewCorner = viewPos;
        viewCorner.xy += (corner - 0.5f) * particleRadius;
        float4 projectedPosition = mul(viewCorner, matProj);

        output.Position = projectedPosition;
        output.Texcoord = float2(corner.x, 1.f - corner.y);
        output.Color = input[0].Color;

        stream.Append(output);
    }
}