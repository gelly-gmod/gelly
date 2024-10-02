#include "fluidsim/CFlexSimScene.h"

#include <NvFlex.h>

#include <stdexcept>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <float.h>

#include <algorithm>
#include <cmath>
#undef min
#undef max
// Would use XMVECTOR but we need to be able to pass this to NvFlex without
// having alignment issues
struct FlexFloat3 {
	float x, y, z;
};

struct FlexQuat {
	float x, y, z, w;
};

using FlexFloat4 = FlexQuat;

static NvFlexCollisionShapeType GetFlexShapeType(ObjectShape shape) {
	switch (shape) {
		case ObjectShape::TRIANGLE_MESH:
			return eNvFlexShapeTriangleMesh;
		case ObjectShape::CAPSULE:
			return eNvFlexShapeCapsule;
		default:
			throw std::runtime_error("GetFlexShapeType: Invalid object shape");
	}
}

CFlexSimScene::CFlexSimScene(NvFlexLibrary *library, NvFlexSolver *solver) :
	library(library), solver(solver), objects({}) {
	geometry.positions = NvFlexAllocBuffer(
		library, maxColliders, sizeof(FlexFloat4), eNvFlexBufferHost
	);

	geometry.rotations = NvFlexAllocBuffer(
		library, maxColliders, sizeof(FlexQuat), eNvFlexBufferHost
	);

	geometry.prevPositions = NvFlexAllocBuffer(
		library, maxColliders, sizeof(FlexFloat4), eNvFlexBufferHost
	);

	geometry.prevRotations = NvFlexAllocBuffer(
		library, maxColliders, sizeof(FlexQuat), eNvFlexBufferHost
	);

	geometry.info = NvFlexAllocBuffer(
		library,
		maxColliders,
		sizeof(NvFlexCollisionGeometry),
		eNvFlexBufferHost
	);

	geometry.flags = NvFlexAllocBuffer(
		library, maxColliders, sizeof(uint), eNvFlexBufferHost
	);

	forceFieldCallback = NvFlexExtCreateForceFieldCallback(solver);
}

CFlexSimScene::~CFlexSimScene() {
	for (const auto &object : objects) {
		if (object.second.shape == ObjectShape::TRIANGLE_MESH) {
			const auto &mesh =
				std::get<ObjectData::TriangleMesh>(object.second.shapeData);
			NvFlexDestroyTriangleMesh(library, mesh.id);
		}
	}

	NvFlexFreeBuffer(geometry.positions);
	NvFlexFreeBuffer(geometry.rotations);
	NvFlexFreeBuffer(geometry.prevPositions);
	NvFlexFreeBuffer(geometry.prevRotations);
	NvFlexFreeBuffer(geometry.info);
	NvFlexFreeBuffer(geometry.flags);
	NvFlexExtDestroyForceFieldCallback(forceFieldCallback);
}

ObjectHandle CFlexSimScene::CreateObject(const ObjectCreationParams &params) {
	ObjectData data = {};

	switch (params.shape) {
		case ObjectShape::TRIANGLE_MESH:
			data = CreateTriangleMesh(
				std::get<ObjectCreationParams::TriangleMesh>(params.shapeData)
			);
			break;
		case ObjectShape::CAPSULE:
			data = CreateCapsule(
				std::get<ObjectCreationParams::Capsule>(params.shapeData)
			);
			break;
		case ObjectShape::FORCEFIELD:
			data = CreateForcefield(
				std::get<ObjectCreationParams::Forcefield>(params.shapeData)
			);
			break;
		default:
			throw std::runtime_error(
				"CFlexSimScene::CreateObject: Invalid object shape"
			);
	}

	data.firstFrame = true;
	objects[monotonicObjectId] = data;
	dirty = true;

	return monotonicObjectId++;
}

void CFlexSimScene::RemoveObject(ObjectHandle handle) {
	if (handle == INVALID_OBJECT_HANDLE) {
		throw std::runtime_error(
			"CFlexSimScene::DestroyObject: Invalid object handle (received the "
			"invalid handle constant)"
		);
	}

	const auto &object = objects.find(handle);
	if (object == objects.end()) {
		return;
	}

	if (object->second.shape == ObjectShape::TRIANGLE_MESH) {
		const auto &mesh =
			std::get<ObjectData::TriangleMesh>(object->second.shapeData);
		NvFlexDestroyTriangleMesh(library, mesh.id);
	}

	objects.erase(object);
	dirty = true;
}

void CFlexSimScene::SetObjectPosition(
	ObjectHandle handle, float x, float y, float z
) {
	auto &object = objects.at(handle);

	if (handle != WORLD_HANDLE && x == 0 && y == 0 && z == 0) {
		// Prevents teleportation bugs from bad addons/maps that delay setting
		// the real position
		return;
	}

	if (object.appliedPosition[0] == x && object.appliedRotation[1] == y &&
		object.appliedPosition[2] == z) {
		return;
	}

	if (std::isnan(x) || std::isnan(y) || std::isnan(z)) {
		return;
	}

	object.position[0] = x;
	object.position[1] = y;
	object.position[2] = z;

	dirty = true;
}

void CFlexSimScene::SetObjectQuaternion(
	ObjectHandle handle, float x, float y, float z, float w
) {
	auto &object = objects.at(handle);
	if (object.appliedRotation[0] == x && object.appliedRotation[1] == y &&
		object.appliedRotation[2] == z && object.appliedRotation[3] == w) {
		return;
	}

	if (std::isnan(x) || std::isnan(y) || std::isnan(z) || std::isnan(w)) {
		return;
	}

	object.rotation[0] = x;
	object.rotation[1] = y;
	object.rotation[2] = z;
	object.rotation[3] = w;

	dirty = true;
}

void CFlexSimScene::Update() {
	if (dirty) {
		std::vector<NvFlexExtForceField> rawForceFields;

		auto *info = static_cast<NvFlexCollisionGeometry *>(
			NvFlexMap(geometry.info, eNvFlexMapWait)
		);

		auto *positions = static_cast<FlexFloat4 *>(
			NvFlexMap(geometry.positions, eNvFlexMapWait)
		);

		auto *rotations = static_cast<FlexQuat *>(
			NvFlexMap(geometry.rotations, eNvFlexMapWait)
		);

		auto *prevPositions = static_cast<FlexFloat4 *>(
			NvFlexMap(geometry.prevPositions, eNvFlexMapWait)
		);

		auto *prevRotations = static_cast<FlexQuat *>(
			NvFlexMap(geometry.prevRotations, eNvFlexMapWait)
		);

		auto *flags =
			static_cast<int *>(NvFlexMap(geometry.flags, eNvFlexMapWait));

		uint valueIndex = 0;
		for (auto &object : objects) {
			if (object.second.shape == ObjectShape::FORCEFIELD) {
				NvFlexExtForceField forceField = {};
				const auto &originalForceField =
					std::get<ObjectCreationParams::Forcefield>(
						object.second.shapeData
					);

				forceField.mPosition[0] = object.second.position[0];
				forceField.mPosition[1] = object.second.position[1];
				forceField.mPosition[2] = object.second.position[2];

				forceField.mRadius = originalForceField.radius;
				forceField.mStrength = originalForceField.strength;
				forceField.mMode = originalForceField.mode;
				forceField.mLinearFalloff = originalForceField.linearFalloff;

				object.second.appliedPosition[0] = object.second.position[0];
				object.second.appliedPosition[1] = object.second.position[1];
				object.second.appliedPosition[2] = object.second.position[2];

				rawForceFields.push_back(forceField);
				continue;
			}

			switch (object.second.shape) {
				case ObjectShape::TRIANGLE_MESH: {
					const auto &mesh = std::get<ObjectData::TriangleMesh>(
						object.second.shapeData
					);
					info[valueIndex].triMesh.mesh = mesh.id;
					info[valueIndex].triMesh.scale[0] = mesh.scale[0];
					info[valueIndex].triMesh.scale[1] = mesh.scale[1];
					info[valueIndex].triMesh.scale[2] = mesh.scale[2];
					break;
				}

				case ObjectShape::CAPSULE: {
					const auto &capsule =
						std::get<ObjectData::Capsule>(object.second.shapeData);
					info[valueIndex].capsule.radius = capsule.radius;
					info[valueIndex].capsule.halfHeight = capsule.halfHeight;
					break;
				}

				default:
					throw std::runtime_error(
						"CFlexSimScene::Update: Invalid object shape (an "
						"implementation for a ObjectShape was forgotten?)"
					);
			}

			if (object.second.firstFrame) {
				// set their previous transform to the new one
				object.second.firstFrame = false;
				prevPositions[valueIndex].x = object.second.position[0];
				prevPositions[valueIndex].y = object.second.position[1];
				prevPositions[valueIndex].z = object.second.position[2];
				prevPositions[valueIndex].w = 1.f;

				prevRotations[valueIndex].x = object.second.rotation[0];
				prevRotations[valueIndex].y = object.second.rotation[1];
				prevRotations[valueIndex].z = object.second.rotation[2];
				prevRotations[valueIndex].w = object.second.rotation[3];
			} else {
				prevPositions[valueIndex] = positions[valueIndex];
				prevRotations[valueIndex] = rotations[valueIndex];
			}

			positions[valueIndex].x = object.second.position[0];
			positions[valueIndex].y = object.second.position[1];
			positions[valueIndex].z = object.second.position[2];
			positions[valueIndex].w = 1.f;

			rotations[valueIndex].x = object.second.rotation[0];
			rotations[valueIndex].y = object.second.rotation[1];
			rotations[valueIndex].z = object.second.rotation[2];
			rotations[valueIndex].w = object.second.rotation[3];

			object.second.appliedPosition[0] = object.second.position[0];
			object.second.appliedPosition[1] = object.second.position[1];
			object.second.appliedPosition[2] = object.second.position[2];

			object.second.appliedRotation[0] = object.second.rotation[0];
			object.second.appliedRotation[1] = object.second.rotation[1];
			object.second.appliedRotation[2] = object.second.rotation[2];

			flags[valueIndex] = NvFlexMakeShapeFlags(
				GetFlexShapeType(object.second.shape), true
			);

			object.second.currentShapeIndex = valueIndex;
			valueIndex++;
		}

		NvFlexUnmap(geometry.info);
		NvFlexUnmap(geometry.positions);
		NvFlexUnmap(geometry.rotations);
		NvFlexUnmap(geometry.prevPositions);
		NvFlexUnmap(geometry.prevRotations);
		NvFlexUnmap(geometry.flags);

		dirty = false;

		NvFlexExtSetForceFields(
			forceFieldCallback, rawForceFields.data(), rawForceFields.size()
		);
	}

	auto forceFieldCount = 0;
	for (const auto &object : objects) {
		if (object.second.shape == ObjectShape::FORCEFIELD) {
			forceFieldCount++;
		}
	}

	NvFlexSetShapes(
		solver,
		geometry.info,
		geometry.positions,
		geometry.rotations,
		geometry.prevPositions,
		geometry.prevRotations,
		geometry.flags,
		objects.size() - forceFieldCount
	);
}

ObjectData CFlexSimScene::CreateTriangleMesh(
	const ObjectCreationParams::TriangleMesh &params
) const {
	// a bit of preprocessing is required, we need to find the min/max vertex
	FlexFloat3 minVertex = {FLT_MAX, FLT_MAX, FLT_MAX};
	FlexFloat3 maxVertex = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

	const auto *vertices =
		reinterpret_cast<const FlexFloat3 *>(params.vertices);
	for (uint i = 0; i < params.vertexCount; i++) {
		minVertex.x = std::min(minVertex.x, vertices[i].x);
		minVertex.y = std::min(minVertex.y, vertices[i].y);
		minVertex.z = std::min(minVertex.z, vertices[i].z);

		maxVertex.x = std::max(maxVertex.x, vertices[i].x);
		maxVertex.y = std::max(maxVertex.y, vertices[i].y);
		maxVertex.z = std::max(maxVertex.z, vertices[i].z);
	}

	NvFlexBuffer *indicesBuffer = NvFlexAllocBuffer(
		library, params.indexCount, sizeof(uint), eNvFlexBufferHost
	);

	NvFlexBuffer *verticesBuffer = NvFlexAllocBuffer(
		library, params.vertexCount, sizeof(FlexFloat4), eNvFlexBufferHost
	);

	{
		void *indicesDst = NvFlexMap(indicesBuffer, eNvFlexMapWait);
		void *verticesDst = NvFlexMap(verticesBuffer, eNvFlexMapWait);

		// We have to convert our indices since they're ushorts
		if (params.indexType ==
			ObjectCreationParams::TriangleMesh::IndexType::UINT16) {
			const ushort *indices = (params.indices16);
			for (uint i = 0; i < params.indexCount; i++) {
				static_cast<int *>(indicesDst)[i] =
					static_cast<int>(indices[i]);
			}
		} else {
			// We still need to convert our indices since they're uints
			const uint *indices = (params.indices32);
			for (uint i = 0; i < params.indexCount; i++) {
				static_cast<int *>(indicesDst)[i] =
					static_cast<int>(indices[i]);
			}
		}

		for (uint i = 0; i < params.vertexCount; i++) {
			static_cast<FlexFloat4 *>(verticesDst)[i] =
				FlexFloat4{vertices[i].x, vertices[i].y, vertices[i].z, 1.0f};
		}

		NvFlexUnmap(indicesBuffer);
		NvFlexUnmap(verticesBuffer);
	}

	const auto meshId = NvFlexCreateTriangleMesh(library);
	NvFlexUpdateTriangleMesh(
		library,
		meshId,
		verticesBuffer,
		indicesBuffer,
		params.vertexCount,
		params.indexCount / 3,
		&minVertex.x,
		&maxVertex.x
	);

	ObjectData data = {};
	data.shape = ObjectShape::TRIANGLE_MESH;

	data.position[0] = 0.0f;
	data.position[1] = 0.0f;
	data.position[2] = 0.0f;

	data.rotation[0] = 0.0f;
	data.rotation[1] = 0.0f;
	data.rotation[2] = 0.0f;
	data.rotation[3] = 1.0f;

	data.shapeData = ObjectData::TriangleMesh{
		meshId, {params.scale[0], params.scale[1], params.scale[2]}
	};

	return data;
}

ObjectData CFlexSimScene::CreateCapsule(
	const ObjectCreationParams::Capsule &params
) const {
	ObjectData data = {};

	data.shape = ObjectShape::CAPSULE;

	data.position[0] = 0.0f;
	data.position[1] = 0.0f;
	data.position[2] = 0.0f;

	data.rotation[0] = 0.0f;
	data.rotation[1] = 0.0f;
	data.rotation[2] = 0.0f;
	data.rotation[3] = 1.0f;

	data.shapeData = ObjectData::Capsule{params.radius, params.halfHeight};

	return data;
}

ObjectData CFlexSimScene::CreateForcefield(
	const ObjectCreationParams::Forcefield &params
) const {
	ObjectData data = {};

	data.shape = ObjectShape::FORCEFIELD;

	data.position[0] = 0.0f;
	data.position[1] = 0.0f;
	data.position[2] = 0.0f;

	data.rotation[0] = 0.0f;
	data.rotation[1] = 0.0f;
	data.rotation[2] = 0.0f;
	data.rotation[3] = 1.0f;

	data.shapeData = params;
	return data;
}

ObjectHandle CFlexSimScene::GetHandleFromShapeIndex(const uint &shapeIndex) {
	const auto it = std::find_if(
		objects.begin(),
		objects.end(),
		[&](const std::pair<ObjectHandle, ObjectData> &object) {
			return object.second.currentShapeIndex == shapeIndex;
		}
	);

	if (it == objects.end()) {
		throw std::runtime_error(
			"CFlexSimScene::GetHandleFromShapeIndex: Invalid shape index"
		);
	}

	return it->first;
}

NvFlexBuffer *CFlexSimScene::GetShapePositions() { return geometry.positions; }