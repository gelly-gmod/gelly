Buffer<float3> velocity0 : register(t0);
Buffer<float3> velocity1 : register(t1);
Buffer<float3> velocity2 : register(t2);
Buffer<float3> velocity3 : register(t3);
Buffer<float3> velocity4 : register(t4);

RWBuffer<float> acceleration : register(u0);

#include "ComputeAccelerationCBuffer.hlsli"

static const float MAGNITUDE_CUTOFF = 3.f;
static const float ACCEL_MULTIPLIER = 0.004f;
static const float FOAM_DECAY_RATE = 8.f;

[numthreads(64, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	uint index = DTid.x;

	float vel0 = length(velocity0[index]);
	float vel1 = length(velocity1[index]);
	float vel2 = length(velocity2[index]);
	float vel3 = length(velocity3[index]);
	float vel4 = length(velocity4[index]);
	float dv10 = abs(vel1 - vel0);
	float dv21 = abs(vel2 - vel1);
	float dv32 = abs(vel3 - vel2);
	float dv43 = abs(vel4 - vel3);

	float avgDv = (dv10 + dv21 + dv32 + dv43) / 4.f;
	float accelMagnitude = avgDv / g_DeltaTime * ACCEL_MULTIPLIER;

	float foaminess = acceleration[index];
	foaminess += accelMagnitude * 0.5f;
	foaminess -= g_DeltaTime / FOAM_DECAY_RATE;
	foaminess = saturate(foaminess);
	acceleration[index] = foaminess;
}