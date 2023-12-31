#ifndef CFLEXSIMRIGIDS_H
#define CFLEXSIMRIGIDS_H

#include "fluidsim/ISimRigids.h"
#include <map>
#include <NvFlexExt.h>

using std::map;

struct InternalRigidData {
	NvFlexExtInstance* instance;
	XMFLOAT3 position;
	XMFLOAT4 rotation;
};

class CFlexSimRigids : public ISimRigids {
private:
	map<RigidModelName, NvFlexExtAsset*> m_rigidAssets;
	map<RigidHandle, InternalRigidData> m_rigids;
	NvFlexExtContainer* m_container;

	void RecomputeRigidTransforms();
public:
	CFlexSimRigids(int maxParticles, NvFlexLibrary* library, NvFlexSolver* solver);
	~CFlexSimRigids() override;

	void AddRigidModel(RigidModelName name, cref<RigidModelCreationParams> params) override;
	RigidHandle CreateRigid(RigidModelName name) override;
	void DestroyRigid(RigidHandle handle) override;

	void Update() override;
	void ComputeNewPositions() override;

	RigidInfo GetRigidInfo(RigidHandle handle) override;
};

#endif //CFLEXSIMRIGIDS_H
