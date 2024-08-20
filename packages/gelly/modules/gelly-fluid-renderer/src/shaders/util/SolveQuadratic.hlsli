float Sign(float x) {
    return x < 0.0 ? -1.0 : 1.0;
}

bool SolveQuadratic(float a, float b, float c, out float minT, out float maxT) {
	float discriminant = fma(c * -4.0, (double)a, (double)b * b);
	if (discriminant <= 0.0f)
	{
		minT = maxT = 0.0f;
		return false;
	}

	float t = fma((double)Sign(b), (double)sqrt(discriminant), (double)b) * -0.5f;
	minT = t / a;
	maxT = c / t;

	if (minT > maxT)
	{
		float tmp = minT;
		minT = maxT;
		maxT = tmp;
	}

	return true;
}