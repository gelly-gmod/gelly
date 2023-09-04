struct VS_INPUT {
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};

struct PS_OUTPUT {
    float4 Col : SV_TARGET0;
    float Depth : SV_Depth;
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

    float zfar = 28377.919921875;
    float znear = 3; // From GMod

    float linearDepth = LinearizeDepth(depth.a, znear, zfar);
    output.Col = float4(linearDepth, 0.f, 0.f, 1.0f);
    output.Depth = depth.a;
    return output;
}