#ifndef FORCEFIELD_HANDLER_H
#define FORCEFIELD_HANDLER_H

#include <NvFlexExt.h>

#include <functional>
#include <memory>

#include "../object-handler.h"
#include "v2/scene/helpers/monotonic-counter.h"
#include "v2/scene/object.h"

namespace Gelly {
struct ForcefieldObject {
	NvFlexExtForceField forcefield;
	void SetPosition(float x, float y, float z) {
		forcefield.mPosition[0] = x;
		forcefield.mPosition[1] = y;
		forcefield.mPosition[2] = z;
	}
};

struct ForcefieldCreationInfo {
	float position[3];
	float radius;
	float strength;
	NvFlexExtForceMode mode;
	bool linearFalloff;
};

}  // namespace Gelly

class ForcefieldHandler : public ObjectHandler {
public:
	using ForcefieldUpdateCallback = std::function<void(ForcefieldObject &)>;

	ForcefieldHandler(
		ObjectHandlerContext ctx, std::shared_ptr<MonotonicCounter> counter
	);
	~ForcefieldHandler() override;

	void Update() override;

	ObjectID MakeForcefield(const ForcefieldCreationInfo &info);
	void RemoveForcefield(ObjectID id);

	void UpdateForcefield(
		ObjectID id, const ForcefieldUpdateCallback &callback
	);

private:
	ObjectHandlerContext ctx;
	std::shared_ptr<MonotonicCounter> counter;
	NvFlexExtForceFieldCallback *forcefieldCallback;

	std::unordered_map<ObjectID, ForcefieldObject> forcefields;
};

#endif	// FORCEFIELD_HANDLER_H
