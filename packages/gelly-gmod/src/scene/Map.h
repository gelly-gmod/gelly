#ifndef MAP_H
#define MAP_H
// clang-format off
#include "BSPParser.h"
// clang-format on

#include <stdexcept>
#include <string>

#include "BSP.h"
#include "PHY.h"
#include "asset-cache.h"
#include "fluidsim/ISimScene.h"

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
	ISimScene *simScene = nullptr;
	ObjectHandle mapObject;

	static void CheckMapPath(const std::string &mapPath);
	[[nodiscard]] static BSPMap LoadBSPMap(const std::string &mapPath);
	[[nodiscard]] static PHYParser::BSP::BSP LoadPHYMap(
		const std::string &mapPath
	);
	[[nodiscard]] static ObjectCreationParams CreateMapParams(
		const float *vertices, size_t vertexCount
	);
	[[nodiscard]] ObjectHandle CreateMapObject(
		const ObjectCreationParams &params
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
		ISimScene *scene,
		const std::string &mapPath);
	Map(Map &&other) = delete;

	~Map();
};

#endif	// MAP_H
