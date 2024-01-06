uint VoxelToIndex(uint3 voxelPosition) {
    return voxelPosition.x + g_voxelCB.domainSize.x * (voxelPosition.y + g_voxelCB.domainSize.y * voxelPosition.z)
}