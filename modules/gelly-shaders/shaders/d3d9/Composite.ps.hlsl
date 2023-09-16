struct VS_INPUT {
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};

struct PS_OUTPUT {
    float4 Col : SV_TARGET0;
    float Depth : SV_DEPTH;
};

float4 debugConstants : register(c0);

sampler2D depthSampler : register(s0);

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
    float4 depth = tex2D(depthSampler, nudged);

    float reconstructedDepth = ReconstructBrokenFloat(depth.y, depth.x);
    if (reconstructedDepth <= 0.01f) {
        discard;
    }

    output.Depth = reconstructedDepth;
    float nearZ = 3;
	float farZ = 28377.919921875;
    float linearDepth = LinearizeDepth(reconstructedDepth, nearZ, farZ);
    // Make depth discontinuities more obvious
    linearDepth = linearDepth * 3.f;
    linearDepth = saturate(linearDepth);
    output.Col = float4(0.f, 0.f, linearDepth, 1.f);
    return output;
}