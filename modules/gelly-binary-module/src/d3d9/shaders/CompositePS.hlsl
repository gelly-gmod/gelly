struct VS_INPUT {
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};

struct PS_OUTPUT {
    float4 Col : SV_TARGET0;
};

texture depthTexture;
sampler2D depthSampler;

PS_OUTPUT main(VS_INPUT input) {
    PS_OUTPUT output;
    float4 depth = tex2D(depthSampler, input.Tex);
    output.Col = float4(depth.xyz, 1.f);
    return output;
}