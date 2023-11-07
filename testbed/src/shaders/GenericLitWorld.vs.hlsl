#include "GenericLitWorld.hlsli"
#include "WorldRenderCBuffer.hlsli"

VS_OUTPUT main(VS_INPUT input) {
    float4x4 mvp = model * view * projection;

    VS_OUTPUT output;
    output.Pos = mul(float4(input.Pos, 1.0f), mvp).xyz;
    output.Normal = input.Normal;
    output.Color = float3(0.8, 0.8, 0.8);

    return output;
}