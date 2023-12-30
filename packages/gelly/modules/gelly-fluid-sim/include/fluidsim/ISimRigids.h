#ifndef ISIMRIGIDS_H
#define ISIMRIGIDS_H

#include <GellyInterface.h>
#include <GellyDataTypes.h>
#include <directxmath.h>

#include <memory>
#include <string>

using std::shared_ptr;
using namespace DirectX;

namespace Gelly {
	struct RigidCreationParams {
		shared_ptr<XMFLOAT3> vertices;
		int numVertices;
		shared_ptr<int> indices;
		int numIndices;

		float radius;
		float expansion;
	};

	/**
	 * \brief You can use this struct to get a position and rotation to set the original triangle mesh to.
	 */
	struct RigidInfo {
		XMFLOAT3 position;
		XMFLOAT4 rotation;
	};
}

using namespace Gelly;
using namespace Gelly::DataTypes;

/**
 * \brief This interface is used for managing classic physics augmentation with unified particle physics.
 * This allows for the simulation of rigid bodies with fluid particles.
 */
gelly_interface ISimRigids {
public:
	using RigidModelName = std::string;
	using RigidHandle = uint32_t;

	virtual ~ISimRigids() = default;
	virtual RigidHandle CreateRigid(cref<RigidCreationParams> params) = 0;
	virtual void DestroyRigid(RigidHandle handle) = 0;

	virtual void Update() = 0;
	virtual RigidInfo GetRigidInfo(RigidHandle handle) = 0;
};

#endif //ISIMRIGIDS_H
