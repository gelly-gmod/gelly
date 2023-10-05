#include "GellyEngineGMod.h"

#include <cassert>

#include "BSPParser.h"

GellyEngineGMod::GellyEngineGMod(
	int maxParticles, int maxColliders, void *d3d11Device
)
	: scene(nullptr) {
	scene = GellyEngine_CreateScene(maxParticles, maxColliders, d3d11Device);

	// This is really a completely unrecoverable error, and this should exit the
	// app while giving a reasonably detailed explanation to the user, but I
	// haven't standardized this across modules. For the time being, this should
	// do.
#ifdef _DEBUG
	assert(scene != nullptr);
#endif
}

GellyEngineGMod::~GellyEngineGMod() {
	if (!scene) {
		return;
	}

	GellyEngine_DestroyScene(scene);
}

GellyScene *GellyEngineGMod::GetScene() { return scene; }

MeshUploadInfo GellyEngineGMod::ProcessBSP(uint8_t *data, size_t dataSize) {
	BSPMap parsedMap(data, dataSize);  // FleX considers CCW triangles to be
									   // front-facing, so we need to flip them
	if (!parsedMap.IsValid()) {
		throw std::runtime_error("Invalid map data");
	}

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

void GellyEngineGMod::FreeBSP(MeshUploadInfo &info) {
	delete[] info.vertices;
	delete[] info.indices;

	info.vertices = nullptr;
	info.indices = nullptr;
}
