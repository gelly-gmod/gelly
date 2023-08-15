#include <stdexcept>
#include <BSPParser.h>
#include "MeshConvert.h"
#include <GellyScene.h>

MeshUploadInfo MeshConvert_LoadBSP(uint8_t* data, size_t dataSize) {
    BSPMap parsedMap(data, dataSize, false); // FleX considers CCW triangles to be front-facing, so we need to flip them
    if (!parsedMap.IsValid()) {
        throw std::runtime_error("Invalid map data");
    }

    MeshUploadInfo info{};
    info.vertexCount = static_cast<int>(parsedMap.GetNumTris()) * 3;
    info.indexCount = info.vertexCount;

    info.vertices = new Vec3[info.vertexCount];
    info.indices = new int[info.indexCount];

    memcpy(info.vertices, parsedMap.GetVertices(), sizeof(Vec3) * info.vertexCount);

    for (int i = 0; i < info.indexCount; i++) {
        info.indices[i] = i;
    }

    Vec3 upper = Vec3{0, 0, 0};
    Vec3 lower = Vec3{0, 0, 0};

    for (int i = 0; i < info.vertexCount; i++) {
        Vec3 vertex = info.vertices[i];
        if (vertex.x > upper.x) {
            upper.x = vertex.x;
        }
        if (vertex.y > upper.y) {
            upper.y = vertex.y;
        }
        if (vertex.z > upper.z) {
            upper.z = vertex.z;
        }
        if (vertex.x < lower.x) {
            lower.x = vertex.x;
        }
        if (vertex.y < lower.y) {
            lower.y = vertex.y;
        }
        if (vertex.z < lower.z) {
            lower.z = vertex.z;
        }
    }

    info.upper = upper;
    info.lower = lower;

    return info;
}

void MeshConvert_FreeBSP(MeshUploadInfo& info) {
    delete[] info.vertices;
    delete[] info.indices;

    info.vertices = nullptr;
    info.indices = nullptr;
}