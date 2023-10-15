#include "DebugCB.hlsli"
#include "DebugRenderStages.hlsli"

struct VS_INPUT {
    float4 Pos : SV_POSITION;
    uint VertexID : SV_VERTEXID;
};

float4 GetColorFromID(uint index) {
    uint r = (index + 7737774) % 255;
    uint g = (index + 1237775) % 255;
    uint b = (index + 237776) % 255;

    return float4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
}

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output;
    output.Pos = float4(input.Pos.xyz, 1.f);
    output.Color = GetColorFromID(input.VertexID);
    return output;
}