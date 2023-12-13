// schlicks approximation of fresnel term

float SchlicksDielectric(float cosTheta, float refractionIndex)
{
    float r0 = (1.0f - refractionIndex) / (1.0f + refractionIndex);
    r0 = r0 * r0;
    return r0 + (1.0f - r0) * pow(1.0f - cosTheta, 5.0f);
}