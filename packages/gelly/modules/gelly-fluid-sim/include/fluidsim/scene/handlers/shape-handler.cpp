#include "shape-handler.h"

#include <stdexcept>

#include "fluidsim/scene/helpers/flex-types.h"

using namespace Gelly;

void ShapeHandler::CreateFleXBuffers() {
	flexBuffers.positions = NvFlexAllocBuffer(
		ctx.lib, MAX_SHAPES, sizeof(FleX::Float3), eNvFlexBufferHost
	);

	flexBuffers.rotations = NvFlexAllocBuffer(
		ctx.lib, MAX_SHAPES, sizeof(FleX::Quat), eNvFlexBufferHost
	);

	flexBuffers.prevPositions = NvFlexAllocBuffer(
		ctx.lib, MAX_SHAPES, sizeof(FleX::Float3), eNvFlexBufferHost
	);

	flexBuffers.prevRotations = NvFlexAllocBuffer(
		ctx.lib, MAX_SHAPES, sizeof(FleX::Quat), eNvFlexBufferHost
	);

	flexBuffers.info = NvFlexAllocBuffer(
		ctx.lib, MAX_SHAPES, sizeof(NvFlexCollisionGeometry), eNvFlexBufferHost
	);

	flexBuffers.flags = NvFlexAllocBuffer(
		ctx.lib, MAX_SHAPES, sizeof(uint32_t), eNvFlexBufferHost
	);
}

void ShapeHandler::DestroyFleXBuffers() {
	NvFlexFreeBuffer(flexBuffers.positions);
	NvFlexFreeBuffer(flexBuffers.rotations);
	NvFlexFreeBuffer(flexBuffers.prevPositions);
	NvFlexFreeBuffer(flexBuffers.prevRotations);
	NvFlexFreeBuffer(flexBuffers.info);
	NvFlexFreeBuffer(flexBuffers.flags);

	for (const auto &entry : objects) {
		const auto &object = entry.second;

		if (object.type == ShapeType::TRIANGLE_MESH) {
			NvFlexDestroyTriangleMesh(ctx.lib, object.triangleMesh.meshId);
		}
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
			static_cast<uint32_t *>(NvFlexMap(flexBuffers.flags, eNvFlexMapWait)
			)
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

uint32_t ShapeHandler::GetCollisionShapeFlags(const ShapeObject &object) {
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
	auto buffers = MapFleXBuffers();

	for (const auto &entry : objects) {
		const auto &object = entry.second;

		const auto &position = object.transform.position;
		const auto &rotation = object.transform.rotation;

		const auto index = entry.first;

		buffers.prevPositions[index] = buffers.positions[index];
		buffers.prevRotations[index] = buffers.rotations[index];

		buffers.positions[index] = {
			position[0], position[1], position[2], 0.0f
		};
		buffers.rotations[index] = {
			rotation[0], rotation[1], rotation[2], rotation[3]
		};

		buffers.info[index] = GetCollisionGeometryInfo(object);
		buffers.flags[index] = GetCollisionShapeFlags(object);
	}

	UnmapFleXBuffers(buffers);

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
