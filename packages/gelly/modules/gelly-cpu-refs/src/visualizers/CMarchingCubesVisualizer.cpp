#include "CMarchingCubesVisualizer.h"

#include "raymath.h"

CMarchingCubesVisualizer::CMarchingCubesVisualizer()
	: m_points(13200), m_camera({}), m_mesh({}) {
	m_camera.position = Vector3{40.f, 40.f, 40.f};
	m_camera.target = Vector3{0.f, 0.f, 0.f};
	m_camera.up = Vector3{0.f, 1.f, 0.f};
	m_camera.fovy = 70.f;
	m_camera.projection = CAMERA_PERSPECTIVE;
}

void CMarchingCubesVisualizer::OnNewFrame() {
	// March to visualize the density field.
	UpdateCamera(&m_camera, CAMERA_THIRD_PERSON);

	BeginMode3D(m_camera);

	for (const auto &point : m_points) {
		m_visualDebugFacility.Draw3DPoint(&point.x, 1.f, 1.f, 1.f);
	}

	const auto min = XMINT3{-5, -5, -5};
	const auto max = XMINT3{5, 5, 5};

	auto output = March(
		Input{
			.m_points = m_points.data(),
			.m_pointCount = static_cast<uint32_t>(m_points.size()),
			.m_min = min,
			.m_max = max,
			.m_visualDebugFacility = &m_visualDebugFacility
		},
		Settings{
			.m_radius = 1.f,
			.m_isovalue = 0.5f,
			.m_voxelSize = 0.125f,
		}
	);

	// DrawMesh(m_mesh, LoadMaterialDefault(), MatrixIdentity());

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

	// const auto min = XMINT3{-5, -5, -5};
	// const auto max = XMINT3{5, 5, 5};
	//
	// auto output = March(
	// 	Input{
	// 		.m_points = m_points.data(),
	// 		.m_pointCount = static_cast<uint32_t>(m_points.size()),
	// 		.m_min = min,
	// 		.m_max = max,
	// 		.m_visualDebugFacility = &m_visualDebugFacility
	// 	},
	// 	Settings{
	// 		.m_radius = 3.f,
	// 		.m_isovalue = 0.5f,
	// 		.m_voxelSize = 0.125f,
	// 	}
	// );
	//
	// m_mesh = Mesh{};
	//
	// m_mesh.vertices = reinterpret_cast<float *>(output.m_vertices.data());
	// m_mesh.normals = reinterpret_cast<float *>(output.m_normals.data());
	// m_mesh.texcoords = reinterpret_cast<float *>(output.m_normals.data());
	//
	// m_mesh.vertexCount = output.m_vertices.size();
	// m_mesh.triangleCount = output.m_vertices.size() / 3;
	//
	// UploadMesh(&m_mesh, false);
	// ExportMesh(m_mesh, "gelly_march_output.obj");
}

const char *CMarchingCubesVisualizer::GetName() const {
	return "Marching Cubes";
}

const char *CMarchingCubesVisualizer::GetCurrentStatus() const {
	return "Marching...";
}
