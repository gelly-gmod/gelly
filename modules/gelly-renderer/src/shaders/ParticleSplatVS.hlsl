struct VS_INPUT {
    float4 position : POSITION;
};

struct VS_OUTPUT {
    float4 position : SV_Position;
};

cbuffer MVPBuffer : register(b0)
{
    matrix view;
    matrix projection;
}

VS_OUTPUT VSMain(VS_INPUT input) {
    VS_OUTPUT output;

    output.position = float4(input.position.xyz, 1.0);
    output.position = mul(output.position, view);
    output.position = mul(output.position, projection);

    return output;
}