#include "EntityManager.h"

EntityManager::EntityManager(ISimScene *scene) : simScene(scene) {}

EntityManager::~EntityManager() {
	for (auto &ent : entities) {
		simScene->RemoveObject(ent.second);
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
	EntIndex entIndex, const std::vector<Vector> &vertices
) {
	ObjectCreationParams params = {};
	params.shape = ObjectShape::TRIANGLE_MESH;

	ObjectCreationParams::TriangleMesh mesh = {};
	mesh.indexType = ObjectCreationParams::TriangleMesh::IndexType::UINT32;

	const auto [processedVertices, indices] = ProcessGModMesh(vertices);
	mesh.vertices = reinterpret_cast<const float *>(processedVertices.data());
	mesh.vertexCount = processedVertices.size();
	mesh.indices32 = indices.data();
	mesh.indexCount = indices.size();
	mesh.scale[0] = 1.f;
	mesh.scale[1] = 1.f;
	mesh.scale[2] = 1.f;

	params.shapeData = mesh;

	entities[entIndex] = simScene->CreateObject(params);
}

void EntityManager::AddPlayerObject(
	EntIndex entIndex, float radius, float halfHeight
) {
	ObjectCreationParams params = {};
	params.shape = ObjectShape::CAPSULE;

	ObjectCreationParams::Capsule capsule = {};
	capsule.radius = radius;
	capsule.halfHeight = halfHeight;

	params.shapeData = capsule;

	entities[entIndex] = simScene->CreateObject(params);
}

void EntityManager::RemoveEntity(EntIndex entIndex) {
	if (auto it = entities.find(entIndex); it != entities.end()) {
		simScene->RemoveObject(it->second);
		entities.erase(it);
	}
}

void EntityManager::UpdateEntityPosition(EntIndex entIndex, Vector position) {
	simScene->SetObjectPosition(
		entities[entIndex], position.x, position.y, position.z
	);
}

void EntityManager::UpdateEntityRotation(EntIndex entIndex, XMFLOAT4 rotation) {
	simScene->SetObjectQuaternion(
		entities[entIndex], rotation.y, rotation.z, rotation.w, rotation.x
	);
}