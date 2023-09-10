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

PS_OUTPUT main(VS_INPUT input) {
    PS_OUTPUT output;
    float4 depth = tex2D(depthSampler, input.Tex);
    if (depth.r <= 0.01f) {
        discard;
    }

    if (depth.r == debugConstants.x) {
        output.Col = float4(1.0f, 1.0f, 1.0f, 1.0f);
    } else if (depth.r < debugConstants.x) {
        output.Col = float4(1.0f, 0.0f, 0.0f, 1.0f);
    } else {
        output.Col = float4(0.0f, 1.0f, 0.0f, 1.0f);
    }

	float nearZ = 3;
	float farZ = 28377.919921875;
	// Output rasterized depth since we're compositing the depth buffer of the particles
    output.Depth = depth.r;
    return output;
}