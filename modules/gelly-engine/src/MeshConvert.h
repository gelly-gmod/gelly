#ifndef GELLY_MESHCONVERT_H
#define GELLY_MESHCONVERT_H

#include <cstdint>

struct MeshUploadInfo;

/**
 * Converts a BSP map to the FleX mesh format. Meant to be used with the Colliders class.
 * @param data
 * @param dataSize
 * @return
 */
MeshUploadInfo MeshConvert_LoadBSP(uint8_t* data, size_t dataSize);
void MeshConvert_FreeBSP(MeshUploadInfo& info);

#endif //GELLY_MESHCONVERT_H
