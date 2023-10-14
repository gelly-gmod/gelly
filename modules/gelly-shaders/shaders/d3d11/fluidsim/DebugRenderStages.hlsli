struct VS_OUTPUT {
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
}

struct GS_OUTPUT {
    float4 Position : SV_POSITION;
    float2 Texcoord : TEXCOORD0;
    float4 Color : COLOR;
}

struct PS_OUTPUT {
    float4 Color : SV_TARGET;
}