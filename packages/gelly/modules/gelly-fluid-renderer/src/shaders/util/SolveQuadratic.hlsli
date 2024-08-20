float Sign(float x) {
    return x < 0.0 ? -1.0 : 1.0;
}

bool SolveQuadratic(float a, float b, float c, out float minT, out float maxT) {
    if (a == 0.0 && b == 0.0)
	{
		minT = maxT = 0.0;
		return true;
	}

	float discriminant = b * b - 4.0 * a * c;

	if (discriminant < 0.0)
	{
		return false;
	}

	float t = -0.5*(b + Sign(b)*sqrt(discriminant));
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