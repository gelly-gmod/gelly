/**
 * Compute the light contribution of a light source at the given position
 */
float3 CalculateLightContribution(float3 color, float power, float radius, float3 lightPosition, float3 position) {
    float distanceSquared = abs(dot(lightPosition - position, lightPosition - position));
    float falloff = radius / (1.f + distanceSquared);
    // No shadowing term here yet
    float3 incomingLightRadiance = (color * power) * falloff;

    return incomingLightRadiance;
};

float3 CalculateCosineLaw(float3 normal, float3 lightPosition, float3 position) {
    return max(0.5f, dot(normal, normalize(lightPosition - position)));
}