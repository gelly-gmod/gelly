static const float DISTORTION = 0.f;
static const float SCALE = 0.3f;
static const float POWER = 0.2f;
static const float AMBIENT = 0.0f;

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