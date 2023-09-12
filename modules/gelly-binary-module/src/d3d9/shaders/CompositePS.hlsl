struct VS_INPUT {
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};

struct PS_OUTPUT {
    float4 Col : SV_TARGET0;
    float Depth : SV_DEPTH;
};

float4 debugConstants : register(c0);

sampler2D depthLowSampler : register(s0);
sampler2D depthHighSampler : register(s1);
sampler2D normalSampler : register(s2);

float LinearizeDepth(float z, float near, float far) {
    return (2.0f * near) / (far + near - z * (far - near));
}

float Remap(float value, float2 originalRange, float2 newRange) {
    return (value - originalRange.x) / (originalRange.y - originalRange.x) * (newRange.y - newRange.x) + newRange.x;
}

#define BREAKPOINT 0.98f
float ReconstructBrokenFloat(float low, float high) {
    return low * BREAKPOINT + high * (1.f - BREAKPOINT);
}

PS_OUTPUT main(VS_INPUT input) {
    PS_OUTPUT output;

    float2 nudged = input.Tex;
    float4 depthLow = tex2D(depthLowSampler, nudged);
    float4 depthHigh = tex2D(depthHighSampler, nudged);

    float reconstructedDepth = ReconstructBrokenFloat(depthLow.x, depthHigh.x);
    if (reconstructedDepth <= 0.01f) {
        discard;
    }

    output.Depth = reconstructedDepth;
    output.Col = float4(reconstructedDepth, reconstructedDepth, reconstructedDepth, 1.0f);

    return output;
}