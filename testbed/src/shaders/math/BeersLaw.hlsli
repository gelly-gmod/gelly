float3 ComputeAbsorption(float3 absorption, float distance)
{
    // from the exponential attenuation law, we can reduce the natural log to e^(-absorption * distance)
    return exp(-absorption * distance);
}