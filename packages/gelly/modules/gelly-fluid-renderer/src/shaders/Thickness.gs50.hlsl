#include "FluidRenderCBuffer.hlsli"
#include "ThicknessStructs.hlsli"

static const float2 corners[4] = {
    float2(0.0, 1.0), float2(0.0, 0.0),
    float2(1.0, 1.0), float2(1.0, 0.0)
};

[maxvertexcount(4)]
void main(point VS_INPUT input[1], inout TriangleStream<GS_OUTPUT> stream) {
    GS_OUTPUT output = (GS_OUTPUT)0;
    float4 viewPosition = mul(g_View, input[0].Pos);

    [unroll]
    for (uint i = 0; i < 4; i++) {
        const float2 corner = corners[i];
        float4 position = viewPosition;
        float size = g_ParticleRadius * 2;
        position.xy += size * (corner - float2(0.5, 0.5));
        position = mul(g_Projection, position);

        float2 tex = corner;
        tex.y = 1.0 - tex.y;

        output.Pos = position;
        output.Tex = tex;

        stream.Append(output);
    }
}