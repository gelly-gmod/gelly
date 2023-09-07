struct VS_INPUT {
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};

struct PS_OUTPUT {
    float4 Col : SV_TARGET0;
};

sampler2D depthSampler : register(s0);

float LinearizeDepth(float z, float near, float far) {
    return (2.0f * near) / (far + near - z * (far - near));
}

PS_OUTPUT main(VS_INPUT input) {
    PS_OUTPUT output;
    float4 depth = tex2D(depthSampler, input.Tex);
    if (depth.a <= 0.01f) {
        discard;
    }

    output.Col = float4(depth.xyz, 1.0f);
    return output;
}