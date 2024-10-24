struct VS_OUTPUT {
    float4 Pos : SV_POSITION;
    float4 WorldPos : WORLD_POS;
    // XY = NDC position, W = lifetime
    float4 FrustrumLifetime : FRUSTRUM_LIFETIME;
    float4 ViewVelocity : VELOCITY;
};

struct GS_OUTPUT {
    float4 Pos : SV_POSITION;
    // W replaced with lifetime
    float LifeTime : LIFETIME;
    float4 ViewVelocity : VELOCITY;
    float2 Tex : TEXCOORD0;
};

struct PS_OUTPUT {
    float3 Thickness : SV_TARGET0;
    float Depth : SV_DEPTH;
};