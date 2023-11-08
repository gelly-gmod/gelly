#include "GenericLitWorld.hlsli"
#include "WorldRenderCBuffer.hlsli"

PS_OUTPUT main(VS_OUTPUT input) {
    PS_OUTPUT output;
    float3 sunDir = normalize(float3(0.1, 0.5, -0.5));
    float angle = saturate(dot(input.Normal, sunDir));
    float3 sunColor = float3(1, 1, 1);

    float3 diffuse = saturate(input.Color * sunColor * angle);
    float3 ambient = saturate(input.Color * float3(0.1, 0.1, 0.1));
    float3 specular = saturate(input.Color * sunColor * pow(angle, 10));

    output.Color = float4(diffuse + ambient + specular, 1);

    return output;
}