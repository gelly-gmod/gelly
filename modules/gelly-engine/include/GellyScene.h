#ifndef GELLY_GELLYSCENE_H
#define GELLY_GELLYSCENE_H

#include <NvFlex.h>
#include <NvFlexExt.h>

#include <cstring>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

// highly common types
namespace gelly_engine {
struct Vec4 {
	float x, y, z, w;
};

struct Vec3 {
	float x, y, z;
};
}  // namespace gelly_engine

using namespace gelly_engine;
using Quat = Vec4;

class GPUCriticalObject {
protected:
	bool gpuWork;

public:
	virtual ~GPUCriticalObject() = default;

	virtual void EnterGPUWork() = 0;

	virtual void ExitGPUWork() = 0;
};

struct MeshUploadInfo {
	Vec3 *vertices;
	int vertexCount;
	int *indices;
	int indexCount;
	Vec3 upper;
	Vec3 lower;
};

using MeshID = NvFlexTriangleMeshId;

struct GellyEntity {
	Vec3 position;
	Quat rotation;
	std::string modelPath;
};

/**
 * @brief Internal mesh properties used to provide finer control over how the
 * mesh is interpreted in FleX.
 */
union ColliderMeshProperties {
	struct {
		Vec3 scale;
	} triangleMesh;
	struct {
		float radius;
	} sphere;
	struct {
		float radius;
		float halfHeight;
	} capsule;
};

/**
 * @brief Internal mesh used to track the mesh ID and buffers for later
 * deletion.
 */
struct ColliderMesh {
	MeshID id;
	NvFlexCollisionShapeType geometryType;
	/**
	 * The active member for this union is determined by the geometryType
	 * property.
	 */
	ColliderMeshProperties properties;
	NvFlexBuffer *vertices;
	NvFlexBuffer *indices;
};

using EntityHandle = unsigned int;
class Colliders : GPUCriticalObject {
	friend class GellyScene;

private:
	NvFlexLibrary *library;
	NvFlexVector<NvFlexCollisionGeometry> geometries;
	NvFlexVector<Vec4> positions;
	NvFlexVector<Quat> rotations;
	NvFlexVector<Vec4> prevPositions;
	NvFlexVector<Quat> prevRotations;
	NvFlexVector<int> flags;

	std::map<std::string, ColliderMesh> meshes;
	std::unordered_map<EntityHandle, GellyEntity> entities;

public:
	void EnterGPUWork() override;

	void ExitGPUWork() override;

	Colliders(NvFlexLibrary *library, int maxColliders);

	~Colliders() noexcept override;

	void AddTriangleMesh(const std::string &modelPath, MeshUploadInfo &info);
	void AddSphere(const std::string &modelPath, float radius);
	void AddCapsule(
		const std::string &modelPath, float radius, float halfHeight
	);

	/**
	 * Adds an entity to the collider simulation. It's recommended to use this
	 * method during GPU work mode.
	 * @note The entity structure passed is expected to not be used again as
	 * this method moves the entity into the internal map for efficient
	 * transfer.
	 * @param entity
	 * @return
	 */
	EntityHandle AddEntity(GellyEntity entity);

	/**
	 * Modifies an entity, this method requires that the class is in GPU work
	 * mode.
	 * @param handle Entity handle received from AddEntity
	 * @return Temporary pointer to the entity, this pointer is only valid while
	 * the class is in GPU work mode.
	 */
	GellyEntity *ModifyEntity(EntityHandle handle);

	[[nodiscard]] int GetEntityCount() const;

	/**
	 * Updates the colliders, this method requires that the class is in GPU work
	 * mode.
	 */
	void Update();
};

enum class SceneRegisterTarget : unsigned char {
	NEIGHBORS,
	NEIGHBOR_COUNTS,
	API_TO_INTERNAL,
	INTERNAL_TO_API
};

class GellyScene : GPUCriticalObject {
private:
	int maxParticles;
	int currentParticleCount;

	NvFlexSolver *solver;
	NvFlexLibrary *library;
	NvFlexVector<Vec4> positions;
	NvFlexVector<Vec3> velocities;
	NvFlexVector<int> phases;
	NvFlexVector<int> activeIndices;

	NvFlexBuffer *d3dParticleBuffer;
	NvFlexBuffer *d3dNeighborBuffer;
	NvFlexBuffer *d3dNeighborCountBuffer;
	NvFlexBuffer *d3dInternalToApiBuffer;
	NvFlexBuffer *d3dApiToInternalBuffer;

public:
	const char *computeDeviceName;
	Colliders colliders;
	NvFlexParams *params;

	GellyScene(NvFlexLibrary *library, int maxParticles, int maxColliders);

	~GellyScene() override;

	/**
	 * This method ensures that the GPU is not calculating anything while we
	 * are. When this method is called, all buffers become available for
	 * reading/writing. This method should be called before any read/write
	 * operations on the buffers.
	 */
	void EnterGPUWork() override;

	/**
	 * This method ensures that the GPU can start calculating again without us
	 * interfering. When this method is called, all buffers become closed for
	 * reading/writing.
	 */
	void ExitGPUWork() override;

	void Update(float deltaTime);

	void AddParticle(Vec4 position, Vec3 velocity);

	void Clear();

	/**
	 * This method registers a D3D11 buffer with FleX. Currently, this mechanism
	 * is only used to deliver particle data straight to the supplied buffer.
	 * @param buffer
	 * @param elementCount
	 * @param elementStride
	 */
	void LinkD3DBuffer(
		void *buffer,
		SceneRegisterTarget target,
		int elementCount,
		int elementStride
	);

	[[nodiscard]] Vec4 *GetPositions() const;

	[[nodiscard]] Vec3 *GetVelocities() const;

	[[nodiscard]] int GetCurrentParticleCount() const;
};

#endif