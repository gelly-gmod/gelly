// Test shader

struct PS_INPUT {
    float4 Pos : SV_POSITION;
};

float4 main(PS_INPUT input) : SV_TARGET {
    return float4(1, 1, 1, 1);
}