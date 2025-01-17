#include "forcefield-handler.h"

#include <stdexcept>
#include <utility>

#include "v2/scene/helpers/monotonic-counter.h"
#include "v2/scene/object.h"

using namespace Gelly;

ForcefieldHandler::ForcefieldHandler(
	ObjectHandlerContext ctx, std::shared_ptr<MonotonicCounter> counter
) :
	ctx(ctx), counter(std::move(counter)) {
	forcefieldCallback = NvFlexExtCreateForceFieldCallback(ctx.solver);
}

ForcefieldHandler::~ForcefieldHandler() {
	NvFlexExtDestroyForceFieldCallback(forcefieldCallback);
}

ObjectID ForcefieldHandler::MakeForcefield(const ForcefieldCreationInfo &info) {
	ForcefieldObject forcefield = {};

	forcefield.forcefield.mMode = info.mode;
	forcefield.forcefield.mLinearFalloff = info.linearFalloff;
	forcefield.forcefield.mRadius = info.radius;
	forcefield.forcefield.mStrength = info.strength;
	forcefield.SetPosition(
		info.position[0], info.position[1], info.position[2]
	);

	const ObjectID id = counter->Increment();
	forcefields[id] = forcefield;

	return id;
}

void ForcefieldHandler::RemoveForcefield(ObjectID id) {
	const auto it = forcefields.find(id);
	if (it == forcefields.end()) {
		throw std::runtime_error("Forcefield does not exist");
	}

	forcefields.erase(it);
}

void ForcefieldHandler::UpdateForcefield(
	ObjectID id, const ForcefieldUpdateCallback &callback
) {
	const auto it = forcefields.find(id);
	if (it == forcefields.end()) {
		throw std::runtime_error("Forcefield does not exist");
	}

	callback(it->second);
}

void ForcefieldHandler::Update() {
	std::vector<NvFlexExtForceField> fields;

	for (const auto &[id, forcefield] : forcefields) {
		fields.push_back(forcefield.forcefield);
	}

	NvFlexExtSetForceFields(
		forcefieldCallback, fields.data(), static_cast<int>(fields.size())
	);
}
