cbuffer g_voxelCB : register(b0) {
    float voxelSize;
    uint3 domainSize;
    uint maxParticlesInVoxel;
    uint maxParticles;
    uint3 pad;
};