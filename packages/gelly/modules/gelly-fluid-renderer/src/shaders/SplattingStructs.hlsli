struct VS_INPUT {
    float4 Pos : SV_Position;
};

struct GS_OUTPUT {
    linear noperspective centroid float4 Pos : SV_Position;
    float2 Tex : TEXCOORD0;
};

struct PS_OUTPUT {
    float4 ShaderDepth : SV_Target0;
    float Depth : SV_DepthGreaterEqual;
};