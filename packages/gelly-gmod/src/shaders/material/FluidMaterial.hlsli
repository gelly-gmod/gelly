#ifndef FLUIDMATERIAL_HLSLI
#define FLUIDMATERIAL_HLSLI

struct FluidMaterial {
    // roughness - x
    // specularTransmission - y
    // refractiveIndex - z
	// isScattering - w (0 or 1)
    float4 r_st_ior;
    float4 diffuseAlbedo;
};

#endif