#ifndef CFLEXSIMSCENE_H
#define CFLEXSIMSCENE_H

#include <NvFlex.h>

#include <unordered_map>

#include "ISimScene.h"

/**
 * \brief Internal representation of scene objects for FleX.
 */
struct ObjectData {
	struct TriangleMesh {
		NvFlexTriangleMeshId id;
		float scale[3];
	};

	struct Capsule {
		float radius;
		float halfHeight;
	};

	ObjectShape shape;
	float position[3];
	float rotation[4];

	std::variant<TriangleMesh, Capsule> shapeData;
	uint currentShapeIndex;
};

class CFlexSimScene : public ISimScene {
private:
	static constexpr uint maxColliders = 8192;

	uint monotonicObjectId = 0;
	bool dirty = false;

	NvFlexLibrary *library = nullptr;
	NvFlexSolver *solver = nullptr;

	struct {
		NvFlexBuffer *positions;
		NvFlexBuffer *rotations;
		NvFlexBuffer *prevPositions;
		NvFlexBuffer *prevRotations;
		NvFlexBuffer *info;
		NvFlexBuffer *flags;
	} geometry = {};

	std::unordered_map<ObjectHandle, ObjectData> objects;

	// FYI: In FleX, the triangle mesh is the only special case.
	// Anything else is POD, but since these have meshes,
	// they need to be managed on the GPU hence the triangle mesh ID.
	// We still have CreateXXXX functions for the other shapes for consistency.
	[[nodiscard]] ObjectData CreateTriangleMesh(
		const ObjectCreationParams::TriangleMesh &params
	) const;

	[[nodiscard]] ObjectData CreateCapsule(
		const ObjectCreationParams::Capsule &params
	) const;

public:
	CFlexSimScene(NvFlexLibrary *library, NvFlexSolver *solver);
	~CFlexSimScene() override;

	ObjectHandle CreateObject(const ObjectCreationParams &params) override;
	void RemoveObject(ObjectHandle handle) override;

	void SetObjectPosition(ObjectHandle handle, float x, float y, float z)
		override;

	void SetObjectQuaternion(
		ObjectHandle handle, float x, float y, float z, float w
	) override;

	ObjectHandle GetHandleFromShapeIndex(const uint& shapeIndex);

	void Update() override;
};

#endif	// CFLEXSIMSCENE_H
