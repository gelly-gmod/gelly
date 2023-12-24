#include "DebugCB.hlsli"
#include "DebugRenderStages.hlsli"

struct VS_INPUT {
    float4 Pos : SV_POSITION;
    uint VertexID : SV_VERTEXID;
};

// Hash to generate good-enuf random numbers
// Taken from https://www.reedbeta.com/blog/hash-functions-for-gpu-rendering/
uint pcg_hash(uint input)
{
    uint state = input * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output;
    output.Pos = float4(input.Pos.xyz, 1.f);
    output.Color = float4(
        float(pcg_hash(input.VertexID + 0u) & 0xFFu) / 255.f,
        float(pcg_hash(input.VertexID + 1u) & 0xFFu) / 255.f,
        float(pcg_hash(input.VertexID + 2u) & 0xFFu) / 255.f,
        1.f
    );

    return output;
}