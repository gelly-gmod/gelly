#include "shape-handler.h"

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
