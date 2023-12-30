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
	CFlexSimRigids(NvFlexLibrary* library, NvFlexSolver* solver);
	~CFlexSimRigids() override;

	RigidHandle CreateRigid(cref<RigidCreationParams> params) override;
	void DestroyRigid(RigidHandle handle) override;

	void Update() override;
	RigidInfo GetRigidInfo(RigidHandle handle) override;
};

#endif //CFLEXSIMRIGIDS_H
