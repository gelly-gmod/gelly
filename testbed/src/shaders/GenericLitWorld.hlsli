/**
Basic dot-shading material for rendering the world
*/

struct VS_OUTPUT {
    float3 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float4 Color : COLOR;
}

struct VS_INPUT {
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
}

struct PS_OUTPUT {
    float4 Color : SV_TARGET;
}