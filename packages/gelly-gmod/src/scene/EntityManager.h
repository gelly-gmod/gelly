#ifndef ENTITIES_H
#define ENTITIES_H

#include <memory>
#include <unordered_map>

#include "EntIndex.h"
#include "GarrysMod/Lua/SourceCompat.h"
#include "asset-cache.h"
#include "fluidsim/IFluidSimulation.h"
#include "fluidsim/ISimScene.h"

class EntityManager {
private:
	std::unordered_map<EntIndex, ObjectHandle> entities;
	// Gelly's interface uses raw pointers
	ISimScene *simScene;

	[[nodiscard]] std::pair<std::vector<Vector>, std::vector<uint32_t>>
	ProcessGModMesh(std::vector<Vector> vertices) const;

public:
	explicit EntityManager(ISimScene *scene);
	~EntityManager();

	void AddEntity(
		EntIndex entIndex,
		const std::shared_ptr<AssetCache> &cache,
		const char *assetName
	);
	void AddPlayerObject(EntIndex entIndex, float radius, float halfHeight);
	void RemoveEntity(EntIndex entIndex);
	void UpdateEntityPosition(EntIndex entIndex, Vector position);
	void UpdateEntityRotation(EntIndex entIndex, XMFLOAT4 rotation);
};

#endif	// ENTITIES_H
