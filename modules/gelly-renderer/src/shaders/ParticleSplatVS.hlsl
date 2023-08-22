struct VS_INPUT {
    float3 position : POSITION;
    float4 instancePos : INSTANCEPOS;
};

struct VS_OUTPUT {
    float4 worldPos : worldPos;
    float4 centerPos : centerPos;
};

cbuffer MVPBuffer : register(b0)
{
    matrix vp;
}

VS_OUTPUT VSMain(VS_INPUT input) {
    VS_OUTPUT output;

    output.worldPos = float4(input.position + input.instancePos.xyz, 1.0);
    output.worldPos = mul(output.position, vp);
    output.centerPos = mul(float4(input.position + input.instancePos.xyz, 1.0), vp);

    return output;
}