#include "EntityManager.h"

EntityManager::EntityManager(gelly::simulation::Scene *scene) :
	simScene(scene) {}

EntityManager::~EntityManager() {
	for (auto &ent : entities) {
		simScene->GetShapeHandler()->RemoveShape(ent.second);
	}
}

std::pair<std::vector<Vector>, std::vector<uint32_t>>
EntityManager::ProcessGModMesh(std::vector<Vector> vertices) const {
	std::vector<Vector> processedVertices = vertices;
	std::vector<uint32_t> indices;

	// FleX expects a different winding order
	for (size_t i = 0; i < vertices.size(); i += 3) {
		indices.push_back(i + 2);
		indices.push_back(i + 1);
		indices.push_back(i);
	}

	return std::make_pair(processedVertices, indices);
}

void EntityManager::AddEntity(
	EntIndex entIndex,
	const std::shared_ptr<AssetCache> &cache,
	const char *assetName
) {
	ShapeCreationInfo params = {};
	params.type = ShapeType::TRIANGLE_MESH;

	params.triMesh.indexType = IndexType::UINT32;

	const auto asset = cache->FetchAsset(assetName);
	std::vector<uint32_t> indices;
	indices.reserve(asset->rawVertices.size() / 3);
	for (size_t i = 0; i < asset->rawVertices.size() / 3; i += 3) {
		indices.push_back(i);
		indices.push_back(i + 1);
		indices.push_back(i + 2);
	}

	params.triMesh.vertices = asset->rawVertices.data();
	params.triMesh.vertexCount = asset->rawVertices.size() / 3;
	params.triMesh.indices32 = indices.data();
	params.triMesh.indexCount = indices.size();
	params.triMesh.scale[0] = 1.f;
	params.triMesh.scale[1] = 1.f;
	params.triMesh.scale[2] = 1.f;

	entities[entIndex] = simScene->GetShapeHandler()->MakeShape(params);
}

void EntityManager::AddPlayerObject(
	EntIndex entIndex, float radius, float halfHeight
) {
	ShapeCreationInfo params = {};
	params.type = ShapeType::CAPSULE;
	params.capsule.radius = radius;
	params.capsule.halfHeight = halfHeight;

	entities[entIndex] = simScene->GetShapeHandler()->MakeShape(params);
}

void EntityManager::RemoveEntity(EntIndex entIndex) {
	if (auto it = entities.find(entIndex); it != entities.end()) {
		simScene->GetShapeHandler()->RemoveShape(it->second);
		entities.erase(it);
	}
}

void EntityManager::UpdateEntityPosition(EntIndex entIndex, Vector position) {
	simScene->GetShapeHandler()->UpdateShape(
		entities[entIndex],
		[&](ShapeObject &object) {
			object.SetTransformPosition(position.x, position.y, position.z);
		}
	);
}

void EntityManager::UpdateEntityRotation(EntIndex entIndex, XMFLOAT4 rotation) {
	simScene->GetShapeHandler()->UpdateShape(
		entities[entIndex],
		[&](ShapeObject &object) {
			object.SetTransformRotation(
				rotation.y, rotation.z, rotation.w, rotation.x
			);
		}
	);
}