#define BREAKPOINT 0.98f
float2 SplitFloat(float value) {
	float lowerRange = clamp(value, 0, BREAKPOINT) / BREAKPOINT;
	float upperRange = clamp(value - BREAKPOINT, 0, 1 - BREAKPOINT) / (1 - BREAKPOINT);

	return float2(upperRange, lowerRange);
}