#ifndef VOXEL_TO_INDEX_HLSLI
#define VOXEL_TO_INDEX_HLSLI
uint VoxelToIndex(uint3 voxelPosition) {
    return voxelPosition.x + voxelPosition.y * g_domainSize.x + voxelPosition.z * g_domainSize.x * g_domainSize.y;
}
#endif