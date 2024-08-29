#include "Map.h"

#include <GMFS.h>

#include <vector>

#include "../logging/global-macros.h"
#include "GarrysMod/Lua/SourceCompat.h"
#include "util/PHYToGMod.h"

void Map::CheckMapPath(const std::string &mapPath) {
	if (mapPath.empty()) {
		throw std::invalid_argument("Map path is empty.");
	}

	if (!FileSystem::Exists(mapPath.c_str())) {
		throw std::invalid_argument("Map path does not exist: " + mapPath);
	}
}

BSPMap Map::LoadBSPMap(const std::string &mapPath) {
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

PHYParser::BSP::BSP Map::LoadPHYMap(const std::string &mapPath) {
	CheckMapPath(mapPath);
	const auto file = FileSystem::Open(mapPath.c_str(), "rb");
	size_t fileSize = FileSystem::Size(file);
	auto fileData = std::make_unique<char[]>(fileSize);
	FileSystem::Read(
		reinterpret_cast<uint8_t *>(fileData.get()), fileSize, file
	);
	FileSystem::Close(file);

	return {{std::move(fileData), fileSize}};
}

ObjectCreationParams Map::CreateMapParams(
	const float *vertices, size_t vertexCount, bool flip
) {
	ObjectCreationParams params = {};
	params.shape = ObjectShape::TRIANGLE_MESH;
	ObjectCreationParams::TriangleMesh mesh = {};
	mesh.vertices = vertices;
	mesh.vertexCount = vertexCount;

	auto *indices = new uint32_t[mesh.vertexCount];

	for (int i = 0; i < mesh.vertexCount; i += 3) {
		if (flip) {
			indices[i] = i + 2;
			indices[i + 1] = i + 1;
			indices[i + 2] = i;
		} else {
			indices[i] = i;
			indices[i + 1] = i + 1;
			indices[i + 2] = i + 2;
		}
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

std::vector<float> Map::ConvertBrushModelToVertices(
	const PHYParser::BSP::BSP::Model &model
) const {
	auto vertices = std::vector<float>{};
	for (const auto &solid : model.solids) {
		for (const auto &triangle : solid.GetTriangles()) {
			float v0x = triangle.vertices[0].x;
			float v0y = triangle.vertices[0].y;
			float v0z = triangle.vertices[0].z;

			float v1x = triangle.vertices[1].x;
			float v1y = triangle.vertices[1].y;
			float v1z = triangle.vertices[1].z;

			float v2x = triangle.vertices[2].x;
			float v2y = triangle.vertices[2].y;
			float v2z = triangle.vertices[2].z;

			gelly::gmod::helpers::ConvertPHYPositionToGMod(v0x, v0y, v0z);
			gelly::gmod::helpers::ConvertPHYPositionToGMod(v1x, v1y, v1z);
			gelly::gmod::helpers::ConvertPHYPositionToGMod(v2x, v2y, v2z);

			vertices.push_back(v0x);
			vertices.push_back(v0y);
			vertices.push_back(v0z);

			vertices.push_back(v1x);
			vertices.push_back(v1y);
			vertices.push_back(v1z);

			vertices.push_back(v2x);
			vertices.push_back(v2y);
			vertices.push_back(v2z);
		}
	}

	return vertices;
}

Map::Map(
	const std::shared_ptr<AssetCache> &assetCache,
	ISimScene *scene,
	const std::string &mapPath
) :
	simScene(scene), mapObject(INVALID_OBJECT_HANDLE) {
	ObjectCreationParams params;

	try {
		const auto phyMap = LoadPHYMap(mapPath);
		if (phyMap.GetModelCount() <= 0) {
			// bail
			throw std::runtime_error("Failed to load PHY map: " + mapPath);
		}

		if (!phyMap.IsDisplacementDataAvailable()) {
			// well... for some maps this is ok, e.g. gm_bigcity_improved (the
			// only displacements are insignificant little mounds of grass) but
			// for other maps, say koth_harvest_final, the *entire* ground is
			// made of displacements, so it becomes a problem

			// for now we'll just bail
			throw std::runtime_error(
				"Physics displacement data is not available for map: " + mapPath
			);
		}

		const auto &worldspawn = phyMap.GetModel(0);
		auto worldspawnVertices = ConvertBrushModelToVertices(worldspawn);

		params = CreateMapParams(
			worldspawnVertices.data(), worldspawnVertices.size() / 3
		);

		mapObject = CreateMapObject(params);

		for (int i = 1; i < phyMap.GetModelCount(); i++) {
			const auto &model = phyMap.GetModel(i);
			auto vertices = ConvertBrushModelToVertices(model);
			assetCache->InsertAsset(
				"*" + std::to_string(model.index),
				reinterpret_cast<Vector *>(vertices.data()),
				vertices.size() / 3
			);

			LOG_INFO(
				"Map is contributing brush model, '*%d', to the asset cache! "
				"(%d vertices)",
				model.index,
				vertices.size() / 3
			);
		}
	} catch (const std::exception &e) {
		LOG_ERROR(
			"Failed to load map using PHYParser: %s\n%s",
			mapPath.c_str(),
			e.what()
		);

		const auto bspMap = LoadBSPMap(mapPath);
		if (!bspMap.IsValid()) {
			throw std::runtime_error("Failed to load BSP map: " + mapPath);
		}

		params = CreateMapParams(
			reinterpret_cast<const float *>(bspMap.GetVertices()),
			bspMap.GetNumVertices(),
			true  // bsp maps are flipped
		);

		mapObject = CreateMapObject(params);
	} catch (...) {
		LOG_ERROR("Fatal error when loading map: %s", mapPath.c_str());
	}

	LOG_INFO("Map loaded: %s\nID: %u", mapPath.c_str(), mapObject);
}

Map::~Map() {
	if (mapObject != INVALID_OBJECT_HANDLE) {
		simScene->RemoveObject(mapObject);
	}
}