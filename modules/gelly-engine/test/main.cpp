#include <GellyEngine.h>
#include <raylib.h>

#include <cstdio>
#include <glm/glm.hpp>
#include <vector>

#define RAYGUI_IMPLEMENTATION

#include <raygui.h>

#include <filesystem>
#include <fstream>

const int screenWidth = 1366;
const int screenHeight = 768;
const int fps = 60;
const char *title = "Gelly Engine Test Window";
static std::vector<Model> models;
static Camera3D camera;

void initCamera() {
	camera.position = Vector3{0.0f, 10.0f, 10.0f};
	camera.target = Vector3{0.0f, 0.0f, 0.0f};
	camera.up = Vector3{0.0f, 0.0f, 1.0f};
	camera.fovy = 15.0f;
	camera.projection = CAMERA_PERSPECTIVE;
}

void updateCamera() {
	glm::vec3 dir = glm::normalize(
		glm::vec3(camera.target.x, camera.target.y, camera.target.z) -
		glm::vec3(camera.position.x, camera.position.y, camera.position.z)
	);
	glm::vec3 right = glm::normalize(glm::cross(dir, glm::vec3(0, 0, 1)));
	glm::vec3 up = glm::normalize(glm::cross(right, dir));

	if (IsKeyDown(KEY_W)) {
		camera.position = Vector3{
			camera.position.x + dir.x * 0.1f,
			camera.position.y + dir.y * 0.1f,
			camera.position.z + dir.z * 0.1f};
	}

	if (IsKeyDown(KEY_S)) {
		camera.position = Vector3{
			camera.position.x - dir.x * 0.1f,
			camera.position.y - dir.y * 0.1f,
			camera.position.z - dir.z * 0.1f};
	}

	if (IsKeyDown(KEY_A)) {
		camera.position = Vector3{
			camera.position.x - right.x * 0.1f,
			camera.position.y - right.y * 0.1f,
			camera.position.z - right.z * 0.1f};
	}

	if (IsKeyDown(KEY_D)) {
		camera.position = Vector3{
			camera.position.x + right.x * 0.1f,
			camera.position.y + right.y * 0.1f,
			camera.position.z + right.z * 0.1f};
	}
}

void drawFluid(GellyScene *scene) {
	BeginMode3D(camera);
	scene->EnterGPUWork();
	scene->colliders.Update();

	for (auto &model : models) {
		DrawModelWires(model, Vector3{0, 0, 0}, 1.0f, WHITE);
		DrawBoundingBox(GetModelBoundingBox(model), RED);
	}

	Vec4 *positions = scene->GetPositions();
	for (int i = 0; i < scene->GetCurrentParticleCount(); i++) {
		Vector2 screenPosition = GetWorldToScreen(
			Vector3{positions[i].x, positions[i].y, positions[i].z}, camera
		);
		if (screenPosition.x < 0 || screenPosition.y < 0 ||
			screenPosition.x > screenWidth || screenPosition.y > screenHeight) {
			continue;
		}
		DrawSphereEx(
			Vector3{positions[i].x, positions[i].y, positions[i].z},
			0.1f,
			3,
			3,
			RED
		);
	}

	if (IsKeyDown(KEY_SPACE)) {
		SetRandomSeed(GetRandomValue(-400, 400) + GetTime());
		for (int i = 0; i < 10; i++) {
			float x = static_cast<float>(GetRandomValue(-10, 10)) / 10.f;
			float y = static_cast<float>(GetRandomValue(-10, 10)) / 10.f;
			scene->AddParticle(Vec4{x, y, 2, .5f}, Vec3{0, 0, 0});
		}
	}
	scene->ExitGPUWork();
	scene->Update(GetFrameTime());

	EndMode3D();
}

void drawInfo(GellyScene *scene) {
	DrawText("gelly-engine test simulation", 10, 10, 20, RAYWHITE);
	DrawText("press SPACE to add a particle", 10, 30, 20, RAYWHITE);
	char particleText[256];
	sprintf(particleText, "particles: %d", scene->GetCurrentParticleCount());
	DrawText(particleText, 10, 50, 20, RAYWHITE);
	char deviceText[256];
	sprintf(deviceText, "device: %s", scene->computeDeviceName);
	DrawText(deviceText, 10, 70, 20, RAYWHITE);
	DrawFPS(10, 90);
}

static float GUI_radiusValue = 0.f;
static float GUI_collisionDistanceValue = 0.f;
static float GUI_surfaceTensionValue = 0.f;
static float GUI_viscosityValue = 0.f;
#define PARAM_SLIDER(index, name, leftText, rightText, minValue, maxValue) \
	GuiSlider(                                                             \
		Rectangle{10, 180 + index * 30, 300, 20},                          \
		leftText,                                                          \
		rightText,                                                         \
		&GUI_##name##Value,                                                \
		minValue,                                                          \
		maxValue                                                           \
	);                                                                     \
	scene->params->name = GUI_##name##Value;
void drawGUI(GellyScene *scene) {
	if (GuiButton(Rectangle{10, 120, 100, 20}, "Shoot particles up")) {
		scene->EnterGPUWork();
		for (int i = 0; i < scene->GetCurrentParticleCount(); i++) {
			Vec3 oldVelocity = scene->GetVelocities()[i];
			scene->GetVelocities()[i] = Vec3{oldVelocity.x, 10, oldVelocity.z};
		}
		scene->ExitGPUWork();
	}

	if (GuiButton(Rectangle{10, 140, 100, 20}, "Add test mesh")) {
		scene->colliders.EnterGPUWork();
		Mesh plane = GenMeshKnot(1.f, 1.f, 16, 32);
		MeshUploadInfo info{};
		info.vertices = reinterpret_cast<Vec3 *>(plane.vertices);
		info.vertexCount = plane.vertexCount;
		int *indices =
			static_cast<int *>(malloc(sizeof(int) * plane.vertexCount));
		for (int i = 0; i < plane.vertexCount; i++) {
			indices[i] = i;
		}

		info.indices = indices;
		info.indexCount = plane.vertexCount;
		BoundingBox box = GetMeshBoundingBox(plane);
		info.lower = Vec3{box.min.x, box.min.y, box.min.z};
		info.upper = Vec3{box.max.x, box.max.y, box.max.z};

		scene->colliders.AddTriangleMesh("models/knot", info);
		GellyEntity entity{};
		entity.position = Vec3{0, 0, 0};
		entity.rotation = Quat{0, 0, 0, 1};
		entity.modelPath = "models/knot";
		scene->colliders.AddEntity(entity);
		scene->colliders.Update();
		scene->colliders.ExitGPUWork();

		free(indices);

		models.push_back(LoadModelFromMesh(plane));
	}

	if (GuiButton(Rectangle{10, 160, 100, 20}, "Add sphere")) {
		scene->colliders.EnterGPUWork();
		// For our visualization, not for the actual scene.
		Mesh sphere = GenMeshSphere(1.f, 32, 32);
		models.push_back(LoadModelFromMesh(sphere));

		scene->colliders.AddSphere("models/sphere", 1.f);
		GellyEntity entity{};
		entity.position = Vec3{0, 0, 0};
		entity.rotation = Quat{0, 0, 0, 1};
		entity.modelPath = "models/sphere";

		scene->colliders.AddEntity(entity);
		scene->colliders.Update();
		scene->colliders.ExitGPUWork();
	}

	PARAM_SLIDER(0, radius, "0.1 radius", "10 radius", 0.2f, 10.f);
	PARAM_SLIDER(
		1,
		collisionDistance,
		"0.1 collision distance",
		"10 collision distance",
		0.1f,
		10.f
	);
	PARAM_SLIDER(
		2,
		surfaceTension,
		"0.1 surface tension",
		"10 surface tension",
		0.1f,
		10.f
	);
	PARAM_SLIDER(3, viscosity, "0.1 viscosity", "10 viscosity", 0.1f, 10.f);
}

int main() {
	GellyScene *scene = GellyEngine_CreateScene(10000, 100);
	InitWindow(screenWidth, screenHeight, title);
	SetTargetFPS(fps);
	initCamera();

	scene->params->gravity[1] = 0;
	scene->params->gravity[2] = -1.f;

	while (!WindowShouldClose()) {
		updateCamera();
		BeginDrawing();
		ClearBackground(BLACK);
		drawFluid(scene);
		drawInfo(scene);
		drawGUI(scene);
		EndDrawing();
	}

	GellyEngine_DestroyScene(scene);
	CloseWindow();
	return 0;
}