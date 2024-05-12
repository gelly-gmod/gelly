#ifndef MAP_H
#define MAP_H
#include <string>

#include "BSPParser.h"
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
	[[nodiscard]] static BSPMap LoadMap(const std::string &mapPath);
	[[nodiscard]] static ObjectCreationParams CreateMapParams(BSPMap map);
	[[nodiscard]] ObjectHandle CreateMapObject(
		const ObjectCreationParams &params
	) const;

public:
	Map(ISimScene *scene, const std::string &mapPath);
	~Map();
};

#endif	// MAP_H
