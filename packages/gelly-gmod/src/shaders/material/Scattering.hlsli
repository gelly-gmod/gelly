const float DISTORTION = 0.2f;
const float SCALE = 5.f;
const float POWER = 17.f;
const float AMBIENT = 0.0f;

float3 ComputeScatteringRadiance(
	float3 light,
	float3 normal,
	float3 eyeDir,
	float attenuation,
	float thickness,
	float3 diffuseAlbedo,
	float3 lightDiffuse
) {
	float3 ltLight = light + normal * DISTORTION;
	float ltDot = pow(saturate(dot(eyeDir, -ltLight)), POWER) * SCALE;
	float3 lt = attenuation * (ltDot + AMBIENT) * thickness;

	return diffuseAlbedo * lightDiffuse * lt;
}