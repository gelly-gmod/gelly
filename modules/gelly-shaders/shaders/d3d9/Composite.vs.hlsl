struct VS_INPUT {
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};

struct VS_OUTPUT {
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};

// Simple passthrough
VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output;
    output.Pos = input.Pos;
    output.Tex = input.Tex;
    return output;
}