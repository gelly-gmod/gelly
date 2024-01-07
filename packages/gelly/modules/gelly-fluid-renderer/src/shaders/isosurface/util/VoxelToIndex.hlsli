uint VoxelToIndex(uint3 voxelPosition) {
    return voxelPosition.x + g_domainSize.x * (voxelPosition.y + g_domainSize.y * voxelPosition.z);
}