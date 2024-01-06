uint3 VoxelizePosition(float3 position) {
    float3 voxelPosition = position / float3(g_voxelCB.voxelSize, g_voxelCB.voxelSize, g_voxelCB.voxelSize);
    // floor to get the voxel index
    uint3 voxelIndex = uint3(floor(voxelPosition));
    voxelIndex = min(voxelIndex, g_voxelCB.domainSize - 1);

    return voxelIndex;
}