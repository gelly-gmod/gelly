struct VS_OUTPUT {
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD;
};

struct PS_OUTPUT {
    float4 Color : SV_TARGET;
};