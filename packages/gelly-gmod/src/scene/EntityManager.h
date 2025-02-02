#ifndef ENTITIES_H
#define ENTITIES_H

#include <memory>
#include <unordered_map>

#include "EntIndex.h"
#include "GarrysMod/Lua/SourceCompat.h"
#include "asset-cache.h"
#include "v2/simulation.h"

using namespace DirectX;

class EntityManager {
private:
	std::unordered_map<EntIndex, std::vector<ObjectID>> entities;
	// Gelly's interface uses raw pointers
	gelly::simulation::Scene *simScene;

	[[nodiscard]] std::pair<std::vector<Vector>, std::vector<uint32_t>>
	ProcessGModMesh(std::vector<Vector> vertices) const;

public:
	explicit EntityManager(gelly::simulation::Scene *scene);
	~EntityManager();

	void AddEntity(
		EntIndex entIndex,
		const std::shared_ptr<AssetCache> &cache,
		const char *assetName
	);
	void AddPlayerObject(EntIndex entIndex, float radius, float halfHeight);
	void RemoveEntity(EntIndex entIndex);
	void UpdateEntityPosition(
		EntIndex entIndex, Vector position, size_t boneIndex = 0
	);
	void UpdateEntityRotation(
		EntIndex entIndex, XMFLOAT4 rotation, size_t boneIndex = 0
	);
	void UpdateEntityScale(
		EntIndex entIndex, Vector scale, size_t boneIndex = 0
	);
};

#endif	// ENTITIES_H
