struct VS_INPUT {
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};

struct PS_OUTPUT {
    float4 Col : SV_TARGET0;
    float Depth : SV_DEPTH;
};

float4x4 matView : register(c0);
float4x4 matProj : register(c4);
float4x4 matInvView : register(c8);
float4x4 matInvProj : register(c12);

sampler2D depthSampler : register(s0);
sampler2D normalSampler : register(s1);

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
    float3 normal = tex2D(normalSampler, nudged).rgb;

    float4 pos = float4(nudged.x * 2 - 1, nudged.y * 2 - 1, depth.x, 1);
    pos = mul(pos, matInvProj);
    pos.xyz /= pos.w;
    pos = mul(pos, matInvView);

    output.Col = float4(pos.xyz, 1);
    output.Depth = ReconstructBrokenFloat(depth.y, depth.x);
    return output;
}