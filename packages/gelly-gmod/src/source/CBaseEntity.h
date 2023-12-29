#ifndef CBASEENTITY_H
#define CBASEENTITY_H

#include "MathTypes.h"
using CBaseEntity = void;

/**
 * \brief Signature for the CBaseEntity::ApplyAbsVelocityImpulse function.
 * \note Function signature: void __fastcall CBaseEntity::ApplyAbsVelocityImpulse(CBaseEntity* thisptr, void* edx, const Vector& impulse)
 */

constexpr const char* CBaseEntity_ApplyAbsVelocityImpulse_Signature = "48 89 5c 24 08 57 48 83 ec 50 f3 0f 10 05 f6 6d bc 00 48 8b fa 0f 2e 02 48 8b d9 75 20 f3 0f 10 05 e7 6d bc 00 0f 2e 42 04";

void ApplyImpulseToServerEntity(CBaseEntity* entity, const Vector& impulse);

#endif //CBASEENTITY_H
