#include "GenericLitWorld.hlsli"
#include "WorldRenderCBuffer.hlsli"

PS_OUTPUT main(VS_OUTPUT input) {
    PS_OUTPUT output;
    float3 sunDir = normalize(float3(0.1, 0.5, -0.5));
    float angle = saturate(dot(input.Normal, sunDir));
    float3 sunColor = float3(1, 1, 1);

    float3 diffuse = saturate(input.Color * sunColor * angle);
    float3 ambient = saturate(input.Color * float3(0.1, 0.1, 0.1));
    float clipX = input.Pos.x / windowSize.x * 2.0 - 1.0;
    float clipY = (1.f - input.Pos.y / windowSize.y) * 2.0 - 1.0;
    float4 clipPos = float4(clipX, clipY, input.Pos.z, 1);
    float3 worldPos = mul(invMvp, clipPos).xyz;
    float3 viewDir = normalize(eyePos.xyz - worldPos.xyz);
    float3 reflectDir = reflect(viewDir, input.Normal);
    angle = saturate(dot(reflectDir, sunDir));

    float3 specular = saturate(input.Color * sunColor * pow(angle, 20));
    output.Color = float4(worldPos, 1);
    return output;
}