float3 SampleAmbientCube(in float4 ambientCube[6], float3 worldSpaceNormal) {
    // re-orient the normal
    float3 normalSquared = worldSpaceNormal * worldSpaceNormal;
    int3 lobeIndex = int3(
        worldSpaceNormal.x < 0.f,
		worldSpaceNormal.y < 0.f,
		worldSpaceNormal.z < 0.f
    );

    float3 xAxisIrradiance = ambientCube[lobeIndex.x].xyz;
    float3 yAxisIrradiance = ambientCube[lobeIndex.y + 2].xyz;
    float3 zAxisIrradiance = ambientCube[lobeIndex.z + 4].xyz;

    float3 totalIrradiance = normalSquared.x * xAxisIrradiance + normalSquared.y * yAxisIrradiance + normalSquared.z * zAxisIrradiance;
    return totalIrradiance;
}