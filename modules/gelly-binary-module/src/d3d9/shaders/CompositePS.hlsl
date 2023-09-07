struct VS_INPUT {
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};

struct PS_OUTPUT {
    float4 Col : SV_TARGET0;
    float Depth : SV_Depth;
};

sampler2D depthSampler : register(s0);
sampler2D normalSampler : register(s1);

float LinearizeDepth(float z, float near, float far) {
    return (2.0f * near) / (far + near - z * (far - near));
}

PS_OUTPUT main(VS_INPUT input) {
    PS_OUTPUT output;
    float4 depth = tex2D(depthSampler, input.Tex);
    if (depth.r <= 0.01f) {
        discard;
    }

    float4 normal = tex2D(normalSampler, input.Tex);

    output.Col = float4(normal.xyz, 1.0f);
    output.Depth = depth.r;
    return output;
}