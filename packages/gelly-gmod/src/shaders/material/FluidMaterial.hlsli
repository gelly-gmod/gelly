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

float TexRefractFromMaterial(in FluidMaterial material) {
    // Completely phyiscally incorrect, but it looks ok and we're not exactly able to refract in a single pass
    return material.r_st_ior.z * 0.01f;
}

#endif