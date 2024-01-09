#ifndef VOXEL_CB_HLSLI
#define VOXEL_CB_HLSLI
cbuffer g_voxelCB : register(b0) {
    float g_voxelSize;
    uint3 g_domainSize;
    uint g_maxParticlesInVoxel;
    uint g_maxParticles;
    uint2 _PAD;
};
#endif