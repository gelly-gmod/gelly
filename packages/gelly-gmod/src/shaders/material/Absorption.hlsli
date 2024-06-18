float3 ComputeAbsorption(float3 absorptionCoefficients, float distance) {
    // We don't exactly want light to be created so we saturate the result
    return saturate(exp(-absorptionCoefficients * distance));
}

float3 NormalizeAbsorption(float3 absorption, float thickness) {
    // cool technique here: since absorption will get darker as thickness increases, we can
    // normalize it so that from the top it looks the same as from the side

    return absorption;
}