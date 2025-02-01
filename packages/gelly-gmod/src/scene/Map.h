#ifndef GELLY_GMOD_MAP_H
#define GELLY_GMOD_MAP_H
// clang-format off
#include "BSPParser.h"
// clang-format on

#include <PHYParser.hpp>
#include <stdexcept>
#include <string>

#include "asset-cache.h"
#include "v2/scene/handlers/shape-handler.h"
#include "v2/scene/scene.h"

namespace gelly::gmod {
// Basic wrapper over BSP lumps for easy access to the solid data of a brush
// model
class PHYMap {
public:
	struct Model {
		std::vector<PhyParser::Phy::Solid> solids;
		uint32_t index;
	};

	PHYMap(std::unique_ptr<std::byte[]> mapData, size_t length);

	bool IsValid() const;
	std::string GetErrorReason() const;
	const std::vector<Model> &GetModels() const;

private:
	std::vector<Model> models;
	std::unique_ptr<std::byte[]> mapData;
	std::string errorReason;
	bool valid;

	template <typename T>
	[[nodiscard]] const T *View(size_t offset) const {
		return reinterpret_cast<const T *>(mapData.get() + offset);
	}
};

/**
 * Instantiates a map object in the simulation's scene.
 * \note Maps are not entities. They don't have a position or rotation,
 * or even a lifetime.
 * \note Maps are given with the path to the map file, relative to garrysmod/
 *
 * For example:
 * @code{.cpp}
 * Map map(scene, "maps/some_map.bsp");
 * @endcode
 *
 * \throws runtime_error if the map could not be loaded.
 * \throws invalid_argument if the map path is empty/non-existent.
 */
class Map {
private:
	simulation::Scene *simScene = nullptr;
	ObjectID mapObject;

	static void CheckMapPath(const std::string &mapPath);
	[[nodiscard]] static BSPMap LoadBSPMap(const std::string &mapPath);
	[[nodiscard]] static PHYMap LoadPHYMap(const std::string &mapPath);
	[[nodiscard]] static ShapeCreationInfo CreateMapParams(
		const float *vertices, size_t vertexCount, bool flip = false
	);
	[[nodiscard]] ObjectID CreateMapObject(
		const ShapeCreationInfo &params
	) const;
	[[nodiscard]] std::vector<float> ConvertBrushModelToVertices(
		const PHYMap::Model &model
	) const;

public:
	/**
	 * \note Upon successful usage of PHYParser, the Map object will contribute
	 * brushmodels to the asset cache.
	 */
	Map(const std::shared_ptr<AssetCache> &assetCache,
		gelly::simulation::Scene *scene,
		const std::string &mapPath);
	Map(Map &&other) = delete;

	// Objects are always managed by their respective handlers, so we can safely
	// discard the map object.
	~Map() = default;
};
}  // namespace gelly::gmod

#endif	// GELLY_GMOD_MAP_H
