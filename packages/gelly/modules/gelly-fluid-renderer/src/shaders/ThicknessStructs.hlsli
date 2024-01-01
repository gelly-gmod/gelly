struct VS_INPUT {
    float4 Pos : SV_Position;
    float3 Absorption : ABSORPTION;
};

struct GS_OUTPUT {
    float4 Pos : SV_Position;
    float2 Tex : TEXCOORD0;
    float3 Absorption : ABSORPTION;
};

struct PS_OUTPUT {
    float4 Thickness : SV_Target0;
    float4 Absorption : SV_Target1;
};