struct VS_INPUT {
    float4 Pos : SV_Position;
};

struct GS_OUTPUT {
    float4 Pos : SV_Position;
    float2 Tex : TEXCOORD0;
};

struct PS_OUTPUT {
    float4 DepthOut : SV_Target0;
    float Depth : SV_Depth;
};

static const float particleRadius = 0.03f;