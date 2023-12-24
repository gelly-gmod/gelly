// Simple low-cost method of encoding precise projection depth into 2 channels of a low-bit texture
// by allocating more bits to the most significant digits.

#define BREAKPOINT 0.98f
float2 EncodeProjectionDepth(float value) {
	float lowerRange = clamp(value, 0, BREAKPOINT) / BREAKPOINT;
	float upperRange = clamp(value - BREAKPOINT, 0, 1 - BREAKPOINT) / (1 - BREAKPOINT);

	return float2(upperRange, lowerRange);
}