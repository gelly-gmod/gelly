#define BREAKPOINT 0.98f
float ReconstructHighBits(float high, float low) {
    return low * BREAKPOINT + high * (1.f - BREAKPOINT);
}