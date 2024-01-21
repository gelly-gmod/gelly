#include "CMarchingCubesVisualizer.h"

CMarchingCubesVisualizer::CMarchingCubesVisualizer()
	: m_densityField(1.f, 0.1f), m_points(225000), m_camera({}) {
	m_camera.position = Vector3{40.f, 40.f, 40.f};
	m_camera.target = Vector3{0.f, 0.f, 0.f};
	m_camera.up = Vector3{0.f, 1.f, 0.f};
	m_camera.fovy = 70.f;
	m_camera.projection = CAMERA_PERSPECTIVE;
}

void CMarchingCubesVisualizer::OnNewFrame() {
	// March to visualize the density field.

	UpdateCamera(&m_camera, CAMERA_THIRD_PERSON);

	const auto min = XMINT3{-20, -20, -20};
	const auto max = XMINT3{20, 20, 20};

	BeginMode3D(m_camera);
	March(Input{
		.m_points = m_points.data(),
		.m_pointCount = static_cast<uint32_t>(m_points.size()),
		.m_min = min,
		.m_max = max,
		.m_densityField = &m_densityField,
		.m_visualDebugFacility = &m_visualDebugFacility
	});
	// for (const auto &point : m_points) {
	// 	m_visualDebugFacility.Draw3DPoint(&point.x, 1.f, 1.f, 1.f);
	// }

	DrawGrid(50, 15.f);
	EndMode3D();
}

void CMarchingCubesVisualizer::Start() {
	// generate random points
	for (auto &point : m_points) {
		float theta = rand() / static_cast<float>(RAND_MAX) * 2.f * PI;
		float phi = rand() / static_cast<float>(RAND_MAX) * 2.f * PI;
		float r = 20.f;

		point.x = r * sinf(theta) * cosf(phi);
		point.y = r * sinf(theta) * sinf(phi);
		point.z = r * cosf(theta);
		point.w = 1.f;
	}
}

const char *CMarchingCubesVisualizer::GetName() const {
	return "Marching Cubes";
}

const char *CMarchingCubesVisualizer::GetCurrentStatus() const {
	return "Marching...";
}
