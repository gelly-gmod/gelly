#define UNDERWATER_DEPTH_MINIMUM 0.7f

bool IsProjDepthUnderwater(float projDepth) {
	return projDepth <= UNDERWATER_DEPTH_MINIMUM;
}