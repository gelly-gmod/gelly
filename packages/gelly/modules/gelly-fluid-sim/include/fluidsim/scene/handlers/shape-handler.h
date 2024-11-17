#ifndef SHAPE_HANDLER_H
#define SHAPE_HANDLER_H

#include <unordered_map>

#include "../object-handler.h"
#include "../object.h"
#include "fluidsim/scene/helpers/monotonic-counter.h"

namespace Gelly {
enum class ShapeType : uint8_t {
	TRIANGLE_MESH,
	CAPSULE,
};

struct ShapeMetadata {
	ShapeType type;
	union {
		struct {
			NvFlexTriangleMeshId meshId;
			float scale[3];
		} triangleMesh;
		struct {
			float radius;
			float halfHeight;
		} capsule;
	};
};

using ShapeObject = Object<ShapeMetadata>;

struct ShapeCreationInfo {
	enum class IndexType {
		UINT16,
		UINT32,
	};

	IndexType indexType;
	/**
	 * \brief This array is not copied, and is never modified. It is up to
	 * the caller to ensure that this array is valid for the lifetime of the
	 * create object function call.
	 */
	const float *vertices;

	union {
		const uint16_t *indices16;
		const uint32_t *indices32;
	};

	uint32_t vertexCount;
	uint32_t indexCount;

	float scale[3];
};

};	// namespace Gelly

class ShapeHandler : public ObjectHandler {
public:
	explicit ShapeHandler(ObjectHandlerContext ctx);

	~ShapeHandler() override;

	void Update() override;

	ObjectID MakeShape(const Gelly::ShapeCreationInfo &info);
	void RemoveShape(ObjectID id);

	void UpdateShapePosition(ObjectID id, float x, float y, float z);
	void UpdateShapeRotation(ObjectID id, float x, float y, float z, float w);

private:
	ObjectHandlerContext ctx;
	MonotonicCounter counter;
	std::unordered_map<ObjectID, ShapeObject> objects;
};

#endif	// SHAPE_HANDLER_H