struct VS_INPUT {
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};

struct PS_OUTPUT {
    float4 Col : SV_TARGET0;
    float Depth : SV_Depth;
};

float4 debugConstants : register(c0);

sampler2D depthSampler : register(s0);
sampler2D normalSampler : register(s1);

float LinearizeDepth(float z, float near, float far) {
    return (2.0f * near) / (far + near - z * (far - near));
}

float SampleGradient(float min, float max, float value) {
    float range = max - min;
    float sample = (value - min) / range;
    return sample;
}

PS_OUTPUT main(VS_INPUT input) {
    PS_OUTPUT output;
    // Remember that D3D10 and above use 0, 0 as the top left of the screen, while
    // D3D9 uses -.5, -.5 as the top left of the screen.

    float2 nudged = input.Tex;
    float4 depth = tex2D(depthSampler, nudged);
    float reconstructedDepth = depth.x;
    if (reconstructedDepth <= 0.01f) {
        discard;
    }

    output.Depth = reconstructedDepth;
    output.Col = float4(reconstructedDepth, reconstructedDepth, reconstructedDepth, 1.0f);

    return output;
}