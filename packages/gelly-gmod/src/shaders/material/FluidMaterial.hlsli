#ifndef FLUIDMATERIAL_HLSLI
#define FLUIDMATERIAL_HLSLI

struct FluidMaterial {
    // roughness - x
    // specularTransmission - y
    // refractiveIndex - z
    float4 r_st_ior;
    float3 diffuseAlbedo;
    float padding;
};

#endif