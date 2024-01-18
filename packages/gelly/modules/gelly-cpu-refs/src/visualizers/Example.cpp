#include "Example.h"

#include <raylib.h>

#include <cmath>

#include "../Logging.h"

static Color GetRGBFromHSL(float hue, float saturation, float lightness) {
	// https://stackoverflow.com/a/64090995
	float a = saturation * fminf(lightness, 1.0f - lightness);
	float k0 = fmodf(hue / 30.f, 12.f);
	float f0 =
		lightness - a * fmaxf(-1.f, fminf(fminf(k0 - 3.f, 9.f - k0), 1.f));

	float k8 = fmodf(k0 + 8.f, 12.f);
	float f8 =
		lightness - a * fmaxf(-1.f, fminf(fminf(k8 - 3.f, 9.f - k8), 1.f));

	float k4 = fmodf(k0 + 4.f, 12.f);
	float f4 =
		lightness - a * fmaxf(-1.f, fminf(fminf(k4 - 3.f, 9.f - k4), 1.f));

	float r, g, b;
	r = f0 * 255.f;
	g = f4 * 255.f;
	b = f8 * 255.f;

	const auto red = static_cast<unsigned char>(floorf(r));
	const auto green = static_cast<unsigned char>(floorf(g));
	const auto blue = static_cast<unsigned char>(floorf(b));
	const unsigned char alpha = 255;

	return Color{red, green, blue, alpha};
}

void CExampleVisualizer::Start() {
	GCR_LOG_INFO("Starting the example visualizer");
}

void CExampleVisualizer::OnNewFrame() {
	// convert hue to rgb
	m_hue += GetFrameTime() * 120.0f;
	m_hue = fmodf(m_hue, 360.0f);

	ClearBackground(GetRGBFromHSL(m_hue, 1.0f, 0.5f));
}

const char *CExampleVisualizer::GetCurrentStatus() const { return "Running"; }

const char *CExampleVisualizer::GetName() const { return "Example"; }
