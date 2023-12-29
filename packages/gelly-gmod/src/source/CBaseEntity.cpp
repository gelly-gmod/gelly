#include "CBaseEntity.h"

#include <hooking/Library.h>

#include <stdexcept>

static Library serverDLL;

using ApplyAbsVelocityImpulseFn = void(__fastcall*)(CBaseEntity* thisptr, const Vector& impulse);

static ApplyAbsVelocityImpulseFn ApplyAbsVelocityImpulse = nullptr;

void EnsureServerDLLLoaded() {
	if (!serverDLL.IsInitialized()) {
		serverDLL.Init("server.dll");
	}
}

void EnsureApplyAbsVelocityImpulseFound() {
	if (ApplyAbsVelocityImpulse == nullptr) {
		ApplyAbsVelocityImpulse = serverDLL.FindFunction<ApplyAbsVelocityImpulseFn>(
			CBaseEntity_ApplyAbsVelocityImpulse_Signature
		);

		if (ApplyAbsVelocityImpulse == nullptr) {
			throw std::runtime_error("Failed to find CBaseEntity::ApplyAbsVelocityImpulse");
		}
	}
}

void ApplyImpulseToServerEntity(CBaseEntity* entity, const Vector& impulse) {
	EnsureServerDLLLoaded();
	EnsureApplyAbsVelocityImpulseFound();

	ApplyAbsVelocityImpulse(entity, impulse);
}