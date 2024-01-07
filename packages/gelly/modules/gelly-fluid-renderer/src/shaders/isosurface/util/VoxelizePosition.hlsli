uint3 VoxelizePosition(float3 position) {
    float3 voxelPosition = position / float3(g_voxelSize, g_voxelSize, g_voxelSize);
    // floor to get the voxel index
    uint3 voxelIndex = uint3(floor(voxelPosition.x), floor(voxelPosition.y), floor(voxelPosition.z));
    voxelIndex = min(voxelIndex, g_domainSize - uint3(1, 1, 1));

    return voxelIndex;
}