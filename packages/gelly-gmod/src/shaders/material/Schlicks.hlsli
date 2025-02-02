float Schlicks(float cosTheta, float refractionIndex) {
    float r0 = (1.0 - refractionIndex) / (1.0 + refractionIndex);
    r0 = r0 * r0;
    return r0 + (1.0 - r0) * pow(1.0 - cosTheta, 5.0);
}

float3 SchlicksConductor(float3 color, float cosTheta) {
	return color + (1.0 - color) * pow(1.0 - cosTheta, 5.0);
}