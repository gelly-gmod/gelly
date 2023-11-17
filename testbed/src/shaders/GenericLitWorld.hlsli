/**
Basic dot-shading material for rendering the world
*/

struct VS_OUTPUT {
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float4 Color : COLOR;
};

struct VS_INPUT {
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
};

struct PS_OUTPUT {
    float3 Albedo : SV_TARGET0;
    float3 Normal : SV_TARGET1;
    float Depth : SV_TARGET2;
    float4 Position : SV_TARGET3;
};