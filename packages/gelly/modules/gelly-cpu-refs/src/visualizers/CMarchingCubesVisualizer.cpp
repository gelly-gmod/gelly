#include "CMarchingCubesVisualizer.h"

CMarchingCubesVisualizer::CMarchingCubesVisualizer()
	: m_points(5000), m_camera({}) {
	m_camera.position = Vector3{40.f, 40.f, 40.f};
	m_camera.target = Vector3{0.f, 0.f, 0.f};
	m_camera.up = Vector3{0.f, 1.f, 0.f};
	m_camera.fovy = 70.f;
	m_camera.projection = CAMERA_PERSPECTIVE;
}

void CMarchingCubesVisualizer::OnNewFrame() {
	// March to visualize the density field.

	UpdateCamera(&m_camera, CAMERA_THIRD_PERSON);

	const auto min = XMINT3{-5, -5, -5};
	const auto max = XMINT3{5, 5, 5};

	BeginMode3D(m_camera);
	March(
		Input{
			.m_points = m_points.data(),
			.m_pointCount = static_cast<uint32_t>(m_points.size()),
			.m_min = min,
			.m_max = max,
			.m_visualDebugFacility = &m_visualDebugFacility
		},
		Settings{
			.m_radius = 1.f,
			.m_isovalue = 0.9f,
			.m_voxelSize = 1.f,
		}
	);
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
		float r = 3.f;

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
