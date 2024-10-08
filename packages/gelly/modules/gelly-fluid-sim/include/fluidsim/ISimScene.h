#ifndef ISIMSCENE_H
#define ISIMSCENE_H

#include <GellyInterface.h>

#include <variant>

#include "GellyDataTypes.h"
#include "NvFlexExt.h"

using namespace Gelly::DataTypes;

namespace Gelly {
enum class ObjectShape : uint8_t {
	TRIANGLE_MESH,
	CAPSULE,
	FORCEFIELD,
};

struct ObjectCreationParams {
	struct TriangleMesh {
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

		uint vertexCount;
		uint indexCount;

		float scale[3];
	};

	struct Capsule {
		float radius;
		float halfHeight;
	};

	/**
	 * Forcefields aren't physical objects, so any calls which don't make sense
	 * (like setting its rotation) *is* ok and will be ignored.
	 */
	struct Forcefield {
		float position[3];
		float radius;
		float strength;
		NvFlexExtForceMode mode;
		bool linearFalloff;
	};

	ObjectShape shape = ObjectShape::TRIANGLE_MESH;
	std::variant<TriangleMesh, Capsule, Forcefield> shapeData;
};

using ObjectHandle = uint;

constexpr ObjectHandle INVALID_OBJECT_HANDLE = 0xFFFFFFFF;
constexpr ObjectHandle WORLD_HANDLE = 0;
}  // namespace Gelly

using namespace Gelly;

/**
 * This abstraction allows the user to add objects to the simulation.
 * Currently, this only allows for triangle meshes and capsules. Convex hulls
 * are not supported.
 */
gelly_interface ISimScene {
public:
	virtual ~ISimScene() = default;

	virtual ObjectHandle CreateObject(const ObjectCreationParams &params) = 0;
	virtual void RemoveObject(ObjectHandle handle) = 0;

	virtual void SetObjectPosition(
		ObjectHandle handle, float x, float y, float z
	) = 0;

	virtual void SetObjectQuaternion(
		ObjectHandle handle, float x, float y, float z, float w
	) = 0;

	/**
	 * \brief Updates the internal representation of the scene.
	 * \note Different implementations may have different requirements for
	 * getting scene data somewhere, so depending on the implementation this may
	 * incur a pipeline stall or something similar.
	 * \note It's highly recommended to call this function after many objects
	 * have been updated, rather than calling it after each object update.
	 */
	virtual void Update() = 0;
};

#endif	// ISIMSCENE_H
