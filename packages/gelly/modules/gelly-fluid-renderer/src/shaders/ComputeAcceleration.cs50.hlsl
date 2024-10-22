Buffer<float3> previousVelocity : register(t0);
Buffer<float3> currentVelocity : register(t1);
RWBuffer<float> acceleration : register(u0);

// we process 64 particles at a time- enough to satisfy most nvidia warp sizes
[numthreads(64, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	uint index = DTid.x;

	float3 prevVel = previousVelocity[index];
	float3 currVel = currentVelocity[index];
	float3 dv = currVel - prevVel;
	float3 accel = dv / 0.001f; // the simulation has a fixed timestep of 60Hz
	float accelMagnitude = length(accel);
	accelMagnitude /= 400.f;
	acceleration[index] = accelMagnitude * accelMagnitude;
}