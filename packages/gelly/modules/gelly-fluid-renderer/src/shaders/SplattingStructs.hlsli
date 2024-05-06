struct VS_INPUT {
    float4 Pos : SV_Position;
    float4 AnisotropyQ1 : ANISOTROPY0;
    float4 AnisotropyQ2 : ANISOTROPY1;
    float4 AnisotropyQ3 : ANISOTROPY2;
};

struct VS_OUTPUT {
    float4 Pos : SV_Position;
    float4 Bounds : BOUNDS;
    float4 InvQ0 : INVQ0;
    float4 InvQ1 : INVQ1;
    float4 InvQ2 : INVQ2;
    float4 InvQ3 : INVQ3;
    float4 NDCPos : NDCPos;
};

struct GS_OUTPUT {
    float4 Pos : SV_Position;
    float4 InvQ0 : INVQ0;
    float4 InvQ1 : INVQ1;
    float4 InvQ2 : INVQ2;
    float4 InvQ3 : INVQ3;
};

struct PS_OUTPUT {
    float4 ShaderDepth : SV_Target0;
    float Depth : SV_DEPTH;
};