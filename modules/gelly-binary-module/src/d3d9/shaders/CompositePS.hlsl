struct VS_INPUT {
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};

struct PS_OUTPUT {
    float4 Col : SV_TARGET0;
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

#define BREAKPOINT 0.5f
float ReconstructBrokenFloat(float low, float high) {
    return low * BREAKPOINT + high * (1.f - BREAKPOINT);
}

PS_OUTPUT main(VS_INPUT input) {
    PS_OUTPUT output;
    // Remember that D3D10 and above use 0, 0 as the top left of the screen, while
    // D3D9 uses -.5, -.5 as the top left of the screen.

    // float2 nudged = input.Tex;
    // float4 depthLow = tex2D(depthLowSampler, nudged);
    // float4 depthHigh = tex2D(depthHighSampler, nudged);

    // float reconstructedDepth = ReconstructBrokenFloat(depthLow.x, depthHigh.x);
    // if (reconstructedDepth <= 0.01f) {
    //     discard;
    // }

    // output.Depth = reconstructedDepth;
    // output.Col = float4(reconstructedDepth, reconstructedDepth, reconstructedDepth, 1.0f);

    float2 texcoord = tex2D(depthLowSampler, input.Tex).xy;
    output.Col = float4(texcoord, 1.f, 2.f);
    return output;
}