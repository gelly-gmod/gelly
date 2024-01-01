#include "SplattingStructs.hlsli"

Buffer<float4> g_Absorption : register(t0);

// quick hack to make everything still work without a refactor
struct VS_REAL_INPUT {
    float4 Pos : SV_Position;
    uint ID : SV_VertexID;
};

VS_INPUT main(VS_REAL_INPUT input) {
    VS_INPUT output = (VS_INPUT)0;
    // This is pretty much just a pass-through shader (hence returning an input), but we do want to make sure that the w-component of the position is 1.0
    output.Pos = float4(input.Pos.xyz, 1.0f);
    output.Absorption = float3(g_Absorption.Load(input.ID).xyz);
    
    return output;
}