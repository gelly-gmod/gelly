#ifndef GELLY_GMOD_MAP_H
#define GELLY_GMOD_MAP_H
// clang-format off
#include "BSPParser.h"
// clang-format on

#include <stdexcept>
#include <string>

#include "BSP.h"
#include "PHY.h"
#include "asset-cache.h"
#include "fluidsim/scene/handlers/shape-handler.h"
#include "fluidsim/scene/scene.h"

namespace gelly::gmod {
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
	gelly::simulation::Scene *simScene = nullptr;
	ObjectID mapObject;

	static void CheckMapPath(const std::string &mapPath);
	[[nodiscard]] static BSPMap LoadBSPMap(const std::string &mapPath);
	[[nodiscard]] static PHYParser::BSP::BSP LoadPHYMap(
		const std::string &mapPath
	);
	[[nodiscard]] static ShapeCreationInfo CreateMapParams(
		const float *vertices, size_t vertexCount, bool flip = false
	);
	[[nodiscard]] ObjectID CreateMapObject(const ShapeCreationInfo &params
	) const;
	[[nodiscard]] std::vector<float> ConvertBrushModelToVertices(
		const PHYParser::BSP::BSP::Model &model
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
