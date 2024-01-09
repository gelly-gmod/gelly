#ifndef VOXELIZE_POSITION_HLSLI
#define VOXELIZE_POSITION_HLSLI
uint3 VoxelizePosition(float3 position) {
    position.xyz += 0.5f * g_voxelSize;
    return uint3(
        (uint)floor(position.x / g_voxelSize),
        (uint)floor(position.y / g_voxelSize),
        (uint)floor(position.z / g_voxelSize)
    );
}
#endif