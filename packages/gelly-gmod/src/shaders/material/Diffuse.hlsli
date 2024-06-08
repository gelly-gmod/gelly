// From the "Moving Frostbite to Physically Based Rendering" paper
float3 F_Schlick(in float3 f0, in float f90, in float u) {
    return f0 + (f90 - f0) * pow(1.f - u, 5);
}

float Fr_DisneyDiffuse(float NdotV, float NdotL, float LdotH, float linearRoughness) {
    float energyBias = lerp(0, 0.5, linearRoughness);
    float energyFactor = lerp(1.0, 1.0 / 1.51, linearRoughness);
    float fd90 = energyBias + 2.0 * LdotH * LdotH * linearRoughness;
    float3 f0 = float3(1.f, 1.f, 1.f);

    float lightScatter = F_Schlick(f0, fd90, NdotL).r;
    float viewScatter = F_Schlick(f0, fd90, NdotV).r;

    return lightScatter * viewScatter * energyFactor;
}

float3 GetNdotV(in float3 normal, in float3 viewDir) {
    return dot(normal, viewDir);
}

float3 GetNdotL(in float3 normal, in float3 lightDir) {
    return dot(normal, lightDir);
}

float3 GetHalfwayDir(in float3 viewDir, in float3 lightDir) {
    return normalize(viewDir + lightDir);
}

float3 GetLdotH(in float3 lightDir, in float3 halfDir) {
    return dot(lightDir, halfDir);
}