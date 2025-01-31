#include "shape-handler.h"

#include <stdexcept>

#include "v2/scene/helpers/flex-types.h"

using namespace Gelly;

void ShapeHandler::CreateFleXBuffers() {
	flexBuffers.positions = NvFlexAllocBuffer(
		ctx.lib, MAX_SHAPES, sizeof(FleX::Float4), eNvFlexBufferHost
	);

	flexBuffers.rotations = NvFlexAllocBuffer(
		ctx.lib, MAX_SHAPES, sizeof(FleX::Quat), eNvFlexBufferHost
	);

	flexBuffers.prevPositions = NvFlexAllocBuffer(
		ctx.lib, MAX_SHAPES, sizeof(FleX::Float4), eNvFlexBufferHost
	);

	flexBuffers.prevRotations = NvFlexAllocBuffer(
		ctx.lib, MAX_SHAPES, sizeof(FleX::Quat), eNvFlexBufferHost
	);

	flexBuffers.info = NvFlexAllocBuffer(
		ctx.lib, MAX_SHAPES, sizeof(NvFlexCollisionGeometry), eNvFlexBufferHost
	);

	flexBuffers.flags =
		NvFlexAllocBuffer(ctx.lib, MAX_SHAPES, sizeof(int), eNvFlexBufferHost);
}

void ShapeHandler::DestroyFleXBuffers() {
	NvFlexFreeBuffer(flexBuffers.positions);
	NvFlexFreeBuffer(flexBuffers.rotations);
	NvFlexFreeBuffer(flexBuffers.prevPositions);
	NvFlexFreeBuffer(flexBuffers.prevRotations);
	NvFlexFreeBuffer(flexBuffers.info);
	NvFlexFreeBuffer(flexBuffers.flags);

	// Decoupling the iteration from the removal prevents invalidating the
	// iterator
	std::vector<ObjectID> ids;
	ids.reserve(objects.size());

	for (const auto &entry : objects) {
		ids.push_back(entry.first);
	}

	for (const auto id : ids) {
		RemoveShape(id);
	}
}

ShapeHandler::ShapeHandler(
	ObjectHandlerContext ctx, std::shared_ptr<MonotonicCounter> objectCounter
) :
	ctx(ctx), counter(std::move(objectCounter)) {
	CreateFleXBuffers();
}

ShapeHandler::~ShapeHandler() { DestroyFleXBuffers(); }

ShapeHandler::MappedBuffers ShapeHandler::MapFleXBuffers() const {
	return {
		.positions = static_cast<FleX::Float4 *>(
			NvFlexMap(flexBuffers.positions, eNvFlexMapWait)
		),
		.rotations = static_cast<FleX::Quat *>(
			NvFlexMap(flexBuffers.rotations, eNvFlexMapWait)
		),
		.prevPositions = static_cast<FleX::Float4 *>(
			NvFlexMap(flexBuffers.prevPositions, eNvFlexMapWait)
		),
		.prevRotations = static_cast<FleX::Quat *>(
			NvFlexMap(flexBuffers.prevRotations, eNvFlexMapWait)
		),
		.info = static_cast<NvFlexCollisionGeometry *>(
			NvFlexMap(flexBuffers.info, eNvFlexMapWait)
		),
		.flags =
			static_cast<int *>(NvFlexMap(flexBuffers.flags, eNvFlexMapWait))
	};
}

void ShapeHandler::UnmapFleXBuffers(MappedBuffers &buffers) const {
	NvFlexUnmap(flexBuffers.positions);
	NvFlexUnmap(flexBuffers.rotations);
	NvFlexUnmap(flexBuffers.prevPositions);
	NvFlexUnmap(flexBuffers.prevRotations);
	NvFlexUnmap(flexBuffers.info);
	NvFlexUnmap(flexBuffers.flags);

	buffers = {};
}

NvFlexCollisionGeometry ShapeHandler::GetCollisionGeometryInfo(
	const ShapeObject &object
) {
	NvFlexCollisionGeometry info = {};

	switch (object.type) {
		case ShapeType::CAPSULE:
			info.capsule.radius = object.capsule.radius;
			info.capsule.halfHeight = object.capsule.halfHeight;
			break;
		case ShapeType::TRIANGLE_MESH:
			info.triMesh.mesh = object.triangleMesh.meshId;
			info.triMesh.scale[0] = object.triangleMesh.scale[0];
			info.triMesh.scale[1] = object.triangleMesh.scale[1];
			info.triMesh.scale[2] = object.triangleMesh.scale[2];
			break;
		default:
			throw std::runtime_error("Invalid shape type");
	}

	return info;
}

int ShapeHandler::GetCollisionShapeFlags(const ShapeObject &object) {
	NvFlexCollisionShapeType shapeType = eNvFlexShapeBox;

	switch (object.type) {
		case ShapeType::CAPSULE:
			shapeType = eNvFlexShapeCapsule;
			break;
		case ShapeType::TRIANGLE_MESH:
			shapeType = eNvFlexShapeTriangleMesh;
			break;
		default:
			throw std::runtime_error("Invalid shape type");
	}

	return NvFlexMakeShapeFlags(shapeType, true);
}

void ShapeHandler::Update() {
	if (isUpdateRequired) {
		auto buffers = MapFleXBuffers();
		int index = 0;

		for (auto &entry : objects) {
			auto &object = entry.second;

			if ((object.IsAtOrigin() || object.WasAtOrigin()) &&
				entry.first != WORLD_ID) {
				// Defer the addition of the object to the next frame when they
				// have an actual position

				// We do want to update their last position to the current one
				// so that we can use it for collision response

				object.SetPreviousPositionToCurrent();
				object.SetPreviousRotationToCurrent();
				continue;
			}

			const auto &position = object.transform.position;
			const auto &rotation = object.transform.rotation;

			buffers.positions[index] = {
				position[0], position[1], position[2], 1.0f
			};
			buffers.rotations[index] = {
				rotation[0], rotation[1], rotation[2], rotation[3]
			};

			buffers.prevPositions[index] = {
				object.transform.prevPosition[0],
				object.transform.prevPosition[1],
				object.transform.prevPosition[2],
				1.0f
			};

			buffers.prevRotations[index] = {
				object.transform.prevRotation[0],
				object.transform.prevRotation[1],
				object.transform.prevRotation[2],
				object.transform.prevRotation[3]
			};

			buffers.info[index] = GetCollisionGeometryInfo(object);
			buffers.flags[index] = GetCollisionShapeFlags(object);

			// Lock previous transforms to last timestep to significantly
			// improve collision response
			object.SetPreviousPositionToCurrent();
			object.SetPreviousRotationToCurrent();

			index++;
		}

		UnmapFleXBuffers(buffers);
		isUpdateRequired = false;
	}

	NvFlexSetShapes(
		ctx.solver,
		flexBuffers.info,
		flexBuffers.positions,
		flexBuffers.rotations,
		flexBuffers.prevPositions,
		flexBuffers.prevRotations,
		flexBuffers.flags,
		objects.size()
	);
}

void ShapeHandler::MakeTriangleMesh(
	const ShapeCreationInfo &info, ShapeObject &object
) {
	using namespace FleX;

	Float3 minVertex = {FLT_MAX, FLT_MAX, FLT_MAX};
	Float3 maxVertex = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

	const auto *vertices =
		reinterpret_cast<const Float3 *>(info.triMesh.vertices);
	for (uint32_t i = 0; i < info.triMesh.vertexCount; i++) {
		minVertex.x = std::min(minVertex.x, vertices[i].x);
		minVertex.y = std::min(minVertex.y, vertices[i].y);
		minVertex.z = std::min(minVertex.z, vertices[i].z);

		maxVertex.x = std::max(maxVertex.x, vertices[i].x);
		maxVertex.y = std::max(maxVertex.y, vertices[i].y);
		maxVertex.z = std::max(maxVertex.z, vertices[i].z);
	}

	NvFlexBuffer *indicesBuffer = NvFlexAllocBuffer(
		ctx.lib, info.triMesh.indexCount, sizeof(uint32_t), eNvFlexBufferHost
	);

	NvFlexBuffer *verticesBuffer = NvFlexAllocBuffer(
		ctx.lib, info.triMesh.vertexCount, sizeof(Float4), eNvFlexBufferHost
	);

	{
		void *indicesDst = NvFlexMap(indicesBuffer, eNvFlexMapWait);
		void *verticesDst = NvFlexMap(verticesBuffer, eNvFlexMapWait);

		// We have to convert our indices since they're ushorts
		if (info.triMesh.indexType == IndexType::UINT16) {
			const uint16_t *indices = info.triMesh.indices16;
			for (uint16_t i = 0; i < info.triMesh.indexCount; i++) {
				static_cast<int *>(indicesDst)[i] =
					static_cast<int>(indices[i]);
			}
		} else {
			// We still need to convert our indices since they're uints
			const uint32_t *indices = info.triMesh.indices32;
			for (uint32_t i = 0; i < info.triMesh.indexCount; i++) {
				static_cast<int *>(indicesDst)[i] =
					static_cast<int>(indices[i]);
			}
		}

		for (uint32_t i = 0; i < info.triMesh.vertexCount; i++) {
			static_cast<Float4 *>(verticesDst)[i] =
				Float4{vertices[i].x, vertices[i].y, vertices[i].z, 1.0f};
		}

		NvFlexUnmap(indicesBuffer);
		NvFlexUnmap(verticesBuffer);
	}

	const auto meshId = NvFlexCreateTriangleMesh(ctx.lib);
	NvFlexUpdateTriangleMesh(
		ctx.lib,
		meshId,
		verticesBuffer,
		indicesBuffer,
		info.triMesh.vertexCount,
		info.triMesh.indexCount / 3,
		&minVertex.x,
		&maxVertex.x
	);

	object.triangleMesh.meshId = meshId;
	object.triangleMesh.scale[0] = info.triMesh.scale[0];
	object.triangleMesh.scale[1] = info.triMesh.scale[1];
	object.triangleMesh.scale[2] = info.triMesh.scale[2];
}

void ShapeHandler::MakeCapsule(
	const ShapeCreationInfo &info, ShapeObject &object
) {
	object.capsule.radius = info.capsule.radius;
	object.capsule.halfHeight = info.capsule.halfHeight;
}

ObjectID ShapeHandler::MakeShape(const ShapeCreationInfo &info) {
	ShapeObject object = {};
	object.type = info.type;

	switch (object.type) {
		case ShapeType::TRIANGLE_MESH:
			MakeTriangleMesh(info, object);
			break;
		case ShapeType::CAPSULE:
			MakeCapsule(info, object);
			break;
	}

	// ensure we have a clean transform
	object.SetTransformPosition(0.0f, 0.0f, 0.0f);
	object.SetTransformRotation(0.0f, 0.0f, 0.0f, 1.0f);

	const auto id = counter->Increment();
	objects[id] = object;
	isUpdateRequired = true;

	return id;
}

void ShapeHandler::RemoveShape(ObjectID id) {
	const auto it = objects.find(id);
	if (it == objects.end()) {
		return;
	}

	if (it->second.type == ShapeType::TRIANGLE_MESH) {
	}

	objects.erase(it);
	isUpdateRequired = true;
}

void ShapeHandler::UpdateShape(
	ObjectID id, const ShapeUpdateCallback &callback
) {
	const auto it = objects.find(id);
	if (it == objects.end()) {
		return;
	}

	auto &object = it->second;
	callback(object);
	isUpdateRequired = true;
}
