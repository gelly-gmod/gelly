#include "Map.h"

#include <GMFS.h>

#include <vector>

#include "LoggingMacros.h"

void Map::CheckMapPath(const std::string &mapPath) {
	if (mapPath.empty()) {
		throw std::invalid_argument("Map path is empty.");
	}

	if (!FileSystem::Exists(mapPath.c_str())) {
		throw std::invalid_argument("Map path does not exist: " + mapPath);
	}
}

BSPMap Map::LoadMap(const std::string &mapPath) {
	CheckMapPath(mapPath);
	const auto file = FileSystem::Open(mapPath.c_str(), "rb");
	size_t fileSize = FileSystem::Size(file);
	std::vector<uint8_t> fileData(fileSize);
	FileSystem::Read(fileData.data(), fileSize, file);

	const BSPMap map(fileData.data(), fileSize);
	if (!map.IsValid()) {
		throw std::runtime_error("Failed to load map: " + mapPath);
	}

	FileSystem::Close(file);
	return map;
}

ObjectCreationParams Map::CreateMapParams(const BSPMap &map) {
	ObjectCreationParams params = {};
	params.shape = ObjectShape::TRIANGLE_MESH;
	ObjectCreationParams::TriangleMesh mesh = {};
	mesh.vertices = map.GetVertices<const float>().data();
	mesh.vertexCount = map.GetNumVertices();

	auto *indices = new uint32_t[mesh.vertexCount];

	for (int i = 0; i < mesh.vertexCount; i += 3) {
		indices[i] = i + 2;
		indices[i + 1] = i + 1;
		indices[i + 2] = i;
	}

	mesh.indexCount = mesh.vertexCount;
	mesh.indices32 = indices;
	mesh.indexType = ObjectCreationParams::TriangleMesh::IndexType::UINT32;
	mesh.scale[0] = 1.0f;
	mesh.scale[1] = 1.0f;
	mesh.scale[2] = 1.0f;

	params.shapeData = mesh;
	return params;
}

ObjectHandle Map::CreateMapObject(const ObjectCreationParams &params) const {
	const auto handle = simScene->CreateObject(params);
	const auto triangleMesh = std::get<0>(params.shapeData);
	delete[] triangleMesh.indices32;

	return handle;
}

Map::Map(ISimScene *scene, const std::string &mapPath)
	: simScene(scene), mapObject(INVALID_OBJECT_HANDLE) {
	const auto map = LoadMap(mapPath);
	const auto params = CreateMapParams(map);
	mapObject = CreateMapObject(params);

	LOG_INFO("Map loaded: %s\nID: %u", mapPath.c_str(), mapObject);
}

Map::~Map() {
	if (mapObject != INVALID_OBJECT_HANDLE) {
		simScene->RemoveObject(mapObject);
	}
}