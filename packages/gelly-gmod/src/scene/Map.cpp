#include "Map.h"

#include <GMFS.h>

#include <vector>

#include "../logging/global-macros.h"
#include "GarrysMod/Lua/SourceCompat.h"
#include "util/PHYToGMod.h"

namespace gelly::gmod {
namespace {
enum LumpType {
	/**
	 * Very important to note: up until 2009, all displacement physics data was
	 * stored in LUMP_PHYSCOLLIDE. Why it was suddenly moved to its own lump in
	 * 2009 is unknown. From an RE analysis, maps don't have any sort of
	 * tell-tale sign other than that LUMP_2009_PHYSDISP's `filelen` is always 0
	 * in pre-2009 maps.
	 */
	LUMP_2009_PHYSDISP = 28,
	LUMP_PHYSCOLLIDE = 29,
};

struct lump_t {
	int fileofs;
	int filelen;
	int version;
	char fourCC[4];
};

struct dheader_t {
	int ident;
	int version;
	lump_t lumps[64];
	int mapRevision;
};

struct dphysmodel_t {
	int modelIndex;
	int dataSize;
	int keyDataSize;
	int solidCount;
};

constexpr auto BSP_HEADER = ('P' << 24) + ('S' << 16) + ('B' << 8) + 'V';

}  // namespace

PHYMap::PHYMap(std::unique_ptr<std::byte[]> mapData, size_t length) :
	models(), mapData(std::move(mapData)), errorReason(), valid(true) {
	try {
		const auto *header = View<dheader_t>(0);
		if (header->ident != BSP_HEADER) {
			throw std::runtime_error("Invalid BSP header.");
		}

		if (header->lumps[LUMP_2009_PHYSDISP].filelen > 0) {
			LOG_WARNING(
				"Map is using PhysDisp encoding. It is not supported and the "
				"map may have holes or missing collision."
			)
		}

		if (header->lumps[LUMP_PHYSCOLLIDE].filelen <= 0) {
			throw std::runtime_error("Map has no physics data.");
		}

		auto physModelsOffset =
			static_cast<size_t>(header->lumps[LUMP_PHYSCOLLIDE].fileofs);
		const auto *physModel = View<dphysmodel_t>(physModelsOffset);
		const auto mapDataSpan =
			std::span<std::byte>(this->mapData.get(), length);
		const PhyParser::OffsetDataView dataView(mapDataSpan);

		while (physModel->modelIndex != -1) {
			Model model = {};
			model.index = physModel->modelIndex;
			model.solids.reserve(physModel->solidCount);

			auto solidOffset =
				physModelsOffset +
				sizeof(dphysmodel_t
				);	// solid data is laid out contigously after the model header

			for (int i = 0; i < physModel->solidCount; i++) {
				const auto surfaceHeader =
					View<PhyParser::Structs::SurfaceHeader>(solidOffset);

				auto parsedSolids =
					PhyParser::Phy::parseCompactSurface(dataView.withOffset(
						solidOffset + sizeof(PhyParser::Structs::SurfaceHeader)
					));

				model.solids.insert(
					model.solids.end(),
					std::make_move_iterator(parsedSolids.begin()),
					std::make_move_iterator(parsedSolids.end())
				);

				solidOffset += surfaceHeader->size + sizeof(int);
			}

			models.push_back(model);
			const auto nextModelOffset = physModel->dataSize +
										 physModel->keyDataSize +
										 sizeof(dphysmodel_t);
			physModelsOffset += nextModelOffset;

			physModel = View<dphysmodel_t>(physModelsOffset);
		}

		return;
	} catch (std::runtime_error &e) {
		errorReason = e.what();
		valid = false;
		return;
	}
}

bool PHYMap::IsValid() const { return valid; }

std::string PHYMap::GetErrorReason() const { return errorReason; }

const std::vector<PHYMap::Model> &PHYMap::GetModels() const { return models; }

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

PHYMap Map::LoadPHYMap(const std::string &mapPath) {
	CheckMapPath(mapPath);
	const auto file = FileSystem::Open(mapPath.c_str(), "rb");
	size_t fileSize = FileSystem::Size(file);
	auto fileData = std::make_unique<std::byte[]>(fileSize);
	FileSystem::Read(
		reinterpret_cast<uint8_t *>(fileData.get()), fileSize, file
	);
	FileSystem::Close(file);

	return PHYMap{std::move(fileData), fileSize};
}

ShapeCreationInfo Map::CreateMapParams(
	const float *vertices, size_t vertexCount, bool flip
) {
	ShapeCreationInfo params = {};
	params.type = ShapeType::TRIANGLE_MESH;

	params.triMesh.vertices = vertices;
	params.triMesh.vertexCount = vertexCount;

	auto *indices = new uint32_t[vertexCount];

	for (int i = 0; i < vertexCount; i += 3) {
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

	params.triMesh.indexCount = vertexCount;
	params.triMesh.indices32 = indices;
	params.triMesh.indexType = IndexType::UINT32;
	params.triMesh.scale[0] = 1.0f;
	params.triMesh.scale[1] = 1.0f;
	params.triMesh.scale[2] = 1.0f;
	return params;
}

ObjectID Map::CreateMapObject(const ShapeCreationInfo &params) const {
	const auto handle = simScene->GetShapeHandler()->MakeShape(params);
	delete[] params.triMesh.indices32;

	return handle;
}

std::vector<float> Map::ConvertBrushModelToVertices(
	const PHYMap::Model &model
) const {
	auto vertices = std::vector<float>{};
	for (const auto &solid : model.solids) {
		for (int i = 0; i < solid.indices.size(); i += 3) {
			const auto index = solid.indices[i];

			float v0x = solid.vertices[index].x;
			float v0y = solid.vertices[index].y;
			float v0z = solid.vertices[index].z;

			float v1x = solid.vertices[solid.indices[i + 1]].x;
			float v1y = solid.vertices[solid.indices[i + 1]].y;
			float v1z = solid.vertices[solid.indices[i + 1]].z;

			float v2x = solid.vertices[solid.indices[i + 2]].x;
			float v2y = solid.vertices[solid.indices[i + 2]].y;
			float v2z = solid.vertices[solid.indices[i + 2]].z;

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
	gelly::simulation::Scene *scene,
	const std::string &mapPath
) :
	simScene(scene), mapObject(0) {
	ShapeCreationInfo params = {};

	try {
		const auto phyMap = LoadPHYMap(mapPath);
		if (!phyMap.IsValid()) {
			throw std::runtime_error(
				"Failed to load PHY map: " + phyMap.GetErrorReason()
			);
		}

		const auto &worldspawn = phyMap.GetModels()[0];
		auto worldspawnVertices = ConvertBrushModelToVertices(worldspawn);

		params = CreateMapParams(
			worldspawnVertices.data(), worldspawnVertices.size() / 3
		);

		LOG_INFO(
			"Map is %d vertices and %d triangles",
			worldspawnVertices.size() / 3,
			worldspawnVertices.size() / 3 / 3
		);

		mapObject = CreateMapObject(params);

		for (const auto &model : phyMap.GetModels()) {
			if (model.index == 0) {
				// no other entity uses the worldspawn model
				continue;
			}

			auto vertices = ConvertBrushModelToVertices(model);
			AssetCache::Bone rootBone = {
				.name = "root",
				.vertices = vertices,
			};

			assetCache->InsertAsset(
				"*" + std::to_string(model.index), {rootBone}
			);

			LOG_INFO(
				"Map is contributing brush model, '*%d', to the asset cache! "
				"(%d vertices, %d triangles)",
				model.index,
				vertices.size() / 3,
				vertices.size() / 3 / 3
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
}  // namespace gelly::gmod