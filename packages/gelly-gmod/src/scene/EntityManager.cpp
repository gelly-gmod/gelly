#include "EntityManager.h"

EntityManager::EntityManager(gelly::simulation::Scene *scene) :
	simScene(scene) {}

EntityManager::~EntityManager() {
	for (auto &ent : entities) {
		for (const auto &id : ent.second) {
			simScene->GetShapeHandler()->RemoveShape(id);
		}
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

	std::vector<ObjectID> boneIds;
	boneIds.reserve(asset->bones.size());

	for (const auto &bone : asset->bones) {
		if (bone.vertices.empty()) {
			boneIds.push_back(INVALID_ID);
			continue;
		}

		const auto vertices = bone.vertices;

		std::vector<uint32_t> indices;
		indices.reserve(vertices.size() / 3);
		for (size_t i = 0; i < vertices.size() / 3; i += 3) {
			indices.push_back(i);
			indices.push_back(i + 1);
			indices.push_back(i + 2);
		}

		params.triMesh.vertices = vertices.data();
		params.triMesh.vertexCount = vertices.size() / 3;
		params.triMesh.indices32 = indices.data();
		params.triMesh.indexCount = indices.size();
		params.triMesh.scale[0] = 1.f;
		params.triMesh.scale[1] = 1.f;
		params.triMesh.scale[2] = 1.f;

		boneIds.push_back(simScene->GetShapeHandler()->MakeShape(params));
	}

	entities[entIndex] = std::move(boneIds);
}

void EntityManager::AddPlayerObject(
	EntIndex entIndex, float radius, float halfHeight
) {
	ShapeCreationInfo params = {};
	params.type = ShapeType::CAPSULE;
	params.capsule.radius = radius;
	params.capsule.halfHeight = halfHeight;

	entities[entIndex] = {simScene->GetShapeHandler()->MakeShape(params)};
}

void EntityManager::RemoveEntity(EntIndex entIndex) {
	if (auto it = entities.find(entIndex); it != entities.end()) {
		for (const auto &id : it->second) {
			simScene->GetShapeHandler()->RemoveShape(id);
		}

		entities.erase(it);
	}
}

void EntityManager::UpdateEntityPosition(
	EntIndex entIndex, Vector position, size_t boneIndex
) {
	// Filter out some known-bad data
	if (position.x == 0.f && position.y == 0.f && position.z == 0.f) {
		return;
	}

	simScene->GetShapeHandler()->UpdateShape(
		entities[entIndex][boneIndex],
		[&](ShapeObject &object) {
			object.SetTransformPosition(position.x, position.y, position.z);
		}
	);
}

void EntityManager::UpdateEntityRotation(
	EntIndex entIndex, XMFLOAT4 rotation, size_t boneIndex
) {
	simScene->GetShapeHandler()->UpdateShape(
		entities[entIndex][boneIndex],
		[&](ShapeObject &object) {
			object.SetTransformRotation(
				rotation.y, rotation.z, rotation.w, rotation.x
			);
		}
	);
}

void EntityManager::UpdateEntityScale(
	EntIndex entIndex, Vector scale, size_t boneIndex
) {
	simScene->GetShapeHandler()->UpdateShape(
		entities[entIndex][boneIndex],
		[&](ShapeObject &object) {
			if (object.type != ShapeType::TRIANGLE_MESH) {
				return;
			}

			object.triangleMesh.scale[0] = scale.x;
			object.triangleMesh.scale[1] = scale.y;
			object.triangleMesh.scale[2] = scale.z;
		}
	);
}