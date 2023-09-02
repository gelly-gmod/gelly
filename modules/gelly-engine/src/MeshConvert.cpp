#include "MeshConvert.h"
// clang-format off
#include <stdexcept>
#include <BSPParser.h>
#include <GellyScene.h>
// clang-format on

MeshUploadInfo MeshConvert_LoadBSP(uint8_t *data, size_t dataSize) {
	BSPMap parsedMap(data, dataSize);  // FleX considers CCW triangles to be
									   // front-facing, so we need to flip them
	if (!parsedMap.IsValid()) {
		throw std::runtime_error("Invalid map data");
	}

	// MENTAL RECAP: THIS IS NOT COLLIDING WITH PARTICLES CORRECTLY! ITS ALL
	// MESSED UP.

	MeshUploadInfo info{};
	info.vertexCount = static_cast<int>(parsedMap.GetNumTris()) * 3;
	info.indexCount = info.vertexCount;

	info.vertices = new Vec3[info.vertexCount];
	info.indices = new int[info.indexCount];

	memcpy(
		info.vertices, parsedMap.GetVertices(), sizeof(Vec3) * info.vertexCount
	);

	for (int i = 0; i < info.indexCount; i += 3) {
		info.indices[i] = i + 2;
		info.indices[i + 1] = i + 1;
		info.indices[i + 2] = i;
	}

	Vec3 upper = Vec3{16384, 16384, 16384};
	Vec3 lower = Vec3{-16384, -16384, -16384};

	info.upper = upper;
	info.lower = lower;

	return info;
}

void MeshConvert_FreeBSP(MeshUploadInfo &info) {
	delete[] info.vertices;
	delete[] info.indices;

	info.vertices = nullptr;
	info.indices = nullptr;
}