#ifndef SHAPE_HANDLER_H
#define SHAPE_HANDLER_H

#include <functional>
#include <unordered_map>

#include "../object-handler.h"
#include "../object.h"
#include "v2/scene/helpers/flex-types.h"
#include "v2/scene/helpers/monotonic-counter.h"

namespace Gelly {
enum class ShapeType : uint8_t {
	TRIANGLE_MESH,
	CAPSULE,
};

struct ShapeObject {
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

	struct {
		float position[3];
		float rotation[4];

		// We take care of the previous position and rotation by ourselves
		// so that we don't accidently use previous values from another object.
		float prevPosition[3];
		float prevRotation[4];
	} transform;

	void SetTransformPosition(float x, float y, float z) {
		transform.position[0] = x;
		transform.position[1] = y;
		transform.position[2] = z;
	}

	void SetTransformRotation(float x, float y, float z, float w) {
		transform.rotation[0] = x;
		transform.rotation[1] = y;
		transform.rotation[2] = z;
		transform.rotation[3] = w;
	}

	void SetPreviousPositionToCurrent() {
		transform.prevPosition[0] = transform.position[0];
		transform.prevPosition[1] = transform.position[1];
		transform.prevPosition[2] = transform.position[2];
	}

	void SetPreviousRotationToCurrent() {
		transform.prevRotation[0] = transform.rotation[0];
		transform.prevRotation[1] = transform.rotation[1];
		transform.prevRotation[2] = transform.rotation[2];
		transform.prevRotation[3] = transform.rotation[3];
	}

	bool IsAtOrigin() {
		return transform.position[0] == 0.0f && transform.position[1] == 0.0f &&
			   transform.position[2] == 0.0f;
	}

	bool WasAtOrigin() {
		return transform.prevPosition[0] == 0.0f &&
			   transform.prevPosition[1] == 0.0f &&
			   transform.prevPosition[2] == 0.0f;
	}
};

enum class IndexType {
	UINT16,
	UINT32,
};

struct ShapeCreationInfo {
	ShapeType type;
	union {
		struct {
			IndexType indexType;
			/**
			 * \brief This array is not copied, and is never modified. It is up
			 * to the caller to ensure that this array is valid for the lifetime
			 * of the create object function call.
			 */
			const float *vertices;

			union {
				const uint16_t *indices16;
				const uint32_t *indices32;
			};

			uint32_t vertexCount;
			uint32_t indexCount;

			float scale[3];
		} triMesh;
		struct {
			float radius;
			float halfHeight;
		} capsule;
	};
};
};	// namespace Gelly

class ShapeHandler : public ObjectHandler {
public:
	using ShapeUpdateCallback = std::function<void(ShapeObject &)>;

	// Maximum number of shapes that can be created.
	// It's unlikely anyone could ever reach this limit, but it is
	// technically possible. See the link below for more information.
	// https://developer.valvesoftware.com/wiki/Entity_limit
	const uint32_t MAX_SHAPES = 8192;

	explicit ShapeHandler(
		ObjectHandlerContext ctx,
		std::shared_ptr<MonotonicCounter> objectCounter
	);

	~ShapeHandler() override;

	void Update() override;

	ObjectID MakeShape(const ShapeCreationInfo &info);
	void RemoveShape(ObjectID id);

	void UpdateShape(ObjectID id, const ShapeUpdateCallback &callback);

private:
	ObjectHandlerContext ctx;
	std::shared_ptr<MonotonicCounter> counter;
	std::unordered_map<ObjectID, ShapeObject> objects;
	bool isUpdateRequired = false;

	struct {
		NvFlexBuffer *positions;
		NvFlexBuffer *rotations;
		NvFlexBuffer *prevPositions;
		NvFlexBuffer *prevRotations;
		NvFlexBuffer *info;
		NvFlexBuffer *flags;
	} flexBuffers = {};

	void CreateFleXBuffers();
	void DestroyFleXBuffers();

	struct MappedBuffers {
		FleX::Float4 *positions;
		FleX::Quat *rotations;
		FleX::Float4 *prevPositions;
		FleX::Quat *prevRotations;
		NvFlexCollisionGeometry *info;
		int *flags;
	};

	[[nodiscard]] MappedBuffers MapFleXBuffers() const;
	void UnmapFleXBuffers(MappedBuffers &buffers) const;

	static NvFlexCollisionGeometry GetCollisionGeometryInfo(
		const ShapeObject &object
	);
	static int GetCollisionShapeFlags(const ShapeObject &object);

	void MakeTriangleMesh(const ShapeCreationInfo &info, ShapeObject &object);

	void MakeCapsule(const ShapeCreationInfo &info, ShapeObject &object);
};

#endif	// SHAPE_HANDLER_H