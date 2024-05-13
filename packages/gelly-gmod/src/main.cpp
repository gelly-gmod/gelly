#include <GarrysMod/Lua/Interface.h>
// clang-format off
#include "scene/Scene.h"
// clang-format on

#include <cstdio>

#include "LoggingMacros.h"
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <GMFS.h>
#include <MinHook.h>
#include <Windows.h>

#include "composite/GModCompositor.h"
#include "source/D3DDeviceWrapper.h"
#include "source/GetCubemap.h"
#include "source/IBaseClientDLL.h"
#include "source/IVRenderView.h"
#include "tracy/Tracy.hpp"
#include "util/GellySharedPtrs.h"

#define DEFINE_LUA_FUNC(namespace, name)    \
	LUA->PushCFunction(namespace##_##name); \
	LUA->SetField(-2, #name);

#define START_GELLY_EXCEPTIONS() try {
#define CATCH_GELLY_EXCEPTIONS()        \
	}                                   \
	catch (const std::exception &e) {   \
		std::string error = e.what();   \
		error += "\n";                  \
		LUA->ThrowError(error.c_str()); \
	}

static std::shared_ptr<GModCompositor> compositor = nullptr;
static std::shared_ptr<Scene> scene = nullptr;
static std::shared_ptr<IFluidRenderer> renderer = nullptr;
static std::shared_ptr<IRenderContext> context = nullptr;
static std::shared_ptr<ISimContext> simContext = nullptr;
static std::shared_ptr<IFluidSimulation> sim = nullptr;

constexpr int DEFAULT_MAX_PARTICLES = 1000000;

void InjectConsoleWindow() {
	AllocConsole();
	freopen_s((FILE **)stdout, "CONOUT$", "w", stdout);
	freopen_s((FILE **)stdin, "CONIN$", "r", stdin);
}

void RemoveConsoleWindow() {
	fclose((FILE *)stdout);
	fclose((FILE *)stdin);
	FreeConsole();
}

LUA_FUNCTION(gelly_Composite) {
	START_GELLY_EXCEPTIONS()
	compositor->Render();
	CATCH_GELLY_EXCEPTIONS()
	return 0;
}

LUA_FUNCTION(gelly_Simulate) {
	START_GELLY_EXCEPTIONS()
	LUA->CheckType(1, GarrysMod::Lua::Type::Number);  // Delta time
	auto dt = static_cast<float>(LUA->GetNumber(1));

	scene->Simulate(dt);
	CATCH_GELLY_EXCEPTIONS()
	return 0;
}

LUA_FUNCTION(gelly_LoadMap) {
	START_GELLY_EXCEPTIONS()

	LUA->CheckType(1, GarrysMod::Lua::Type::String);  // Map name
	scene->LoadMap(LUA->GetString(1));

	CATCH_GELLY_EXCEPTIONS()
	return 0;
}

LUA_FUNCTION(gelly_AddObject) {
	START_GELLY_EXCEPTIONS()

	// The lua side will pass through the triangle mesh
	LUA->CheckType(1, GarrysMod::Lua::Type::Table);	  // Mesh
	LUA->CheckType(2, GarrysMod::Lua::Type::Number);  // Ent index
	const auto entIndex = static_cast<EntIndex>(LUA->GetNumber(2));
	LUA->Pop();	 // to not interfere with the loop

	const uint32_t vertexCount = LUA->ObjLen(1);
	if (vertexCount <= 0) {
		LUA->ThrowError("Cannot create object with no vertices!");
	}

	std::vector<Vector> vertices(vertexCount);

	for (uint32_t i = 0; i < vertexCount; i++) {
		LUA->PushNumber(i + 1);
		LUA->GetTable(-2);
		const auto vertex = LUA->GetVector(-1);
		vertices[i] = vertex;
		LUA->Pop();
	}

	scene->AddEntity(entIndex, vertices);
	CATCH_GELLY_EXCEPTIONS();
	return 0;
}

LUA_FUNCTION(gelly_AddPlayerObject) {
	START_GELLY_EXCEPTIONS();

	LUA->CheckType(1, GarrysMod::Lua::Type::Number);  // Radius
	LUA->CheckType(2, GarrysMod::Lua::Type::Number);  // Half height
	LUA->CheckType(3, GarrysMod::Lua::Type::Number);  // Ent index

	const auto radius = static_cast<float>(LUA->GetNumber(1));
	const auto halfHeight = static_cast<float>(LUA->GetNumber(2));
	const auto entIndex = static_cast<int>(LUA->GetNumber(3));

	scene->AddPlayerObject(entIndex, radius, halfHeight);

	CATCH_GELLY_EXCEPTIONS();
	return 0;
}

LUA_FUNCTION(gelly_RemoveObject) {
	START_GELLY_EXCEPTIONS();
	LUA->CheckType(1, GarrysMod::Lua::Type::Number);  // Handle
	scene->RemoveEntity(static_cast<EntIndex>(LUA->GetNumber(1)));
	CATCH_GELLY_EXCEPTIONS();
	return 0;
}

LUA_FUNCTION(gelly_SetObjectPosition) {
	START_GELLY_EXCEPTIONS();

	LUA->CheckType(1, GarrysMod::Lua::Type::Number);  // Handle
	LUA->CheckType(2, GarrysMod::Lua::Type::Vector);  // Position

	scene->UpdateEntityPosition(
		static_cast<EntIndex>(LUA->GetNumber(1)), LUA->GetVector(2)
	);

	CATCH_GELLY_EXCEPTIONS();
	return 0;
}

inline float rad(float deg) { return deg * (XM_PI / 180.0); }

LUA_FUNCTION(gelly_SetObjectRotation) {
	START_GELLY_EXCEPTIONS();

	LUA->CheckType(1, GarrysMod::Lua::Type::Number);  // Handle
	LUA->CheckType(2, GarrysMod::Lua::Type::Angle);	  // Rotation
	// we need to convert it from an ang to a quaternion
	QAngle ang = LUA->GetAngle(2);

	float p = rad(ang.x) / 2.f;
	float y = rad(ang.y) / 2.f;
	float r = rad(ang.z) / 2.f;
	float cr = cos(r);
	float sr = sin(r);
	float cp = cos(p);
	float sp = sin(p);
	float cy = cos(y);
	float sy = sin(y);

	XMFLOAT4 quat = {
		cr * cp * cy + sr * sp * sy,
		sr * cp * cy - cr * sp * sy,
		cr * sp * cy + sr * cp * sy,
		cr * cp * sy - sr * sp * cy
	};

	scene->UpdateEntityRotation(static_cast<EntIndex>(LUA->GetNumber(1)), quat);
	CATCH_GELLY_EXCEPTIONS();
	return 0;
}

LUA_FUNCTION(gelly_AddParticles) {
	START_GELLY_EXCEPTIONS();
	LUA->CheckType(1, GarrysMod::Lua::Type::Table);	  // Particles
	LUA->CheckType(2, GarrysMod::Lua::Type::Vector);  // Absorption

	const uint32_t particleCount = LUA->ObjLen(1);
	const auto absorption = LUA->GetVector(2);
	auto builder = ParticleManager::CreateParticleList();

	LUA->Pop();	 // To make the loop simpler

	builder.SetAbsorption(absorption.x, absorption.y, absorption.z);

	for (uint32_t i = 0; i < particleCount; i += 2) {
		LUA->PushNumber(i + 1);
		LUA->GetTable(-2);
		const auto position = LUA->GetVector(-1);
		LUA->PushNumber(i + 2);
		LUA->GetTable(-3);
		const auto velocity = LUA->GetVector(-1);

		builder.AddParticle(position, velocity);

		LUA->Pop(2);
	}

	scene->AddParticles(builder);
	CATCH_GELLY_EXCEPTIONS();

	return 0;
}

LUA_FUNCTION(gelly_GetStatus) {
	START_GELLY_EXCEPTIONS();
	// Current status table:
	//	- ComputeDeviceName: string
	//	- ActiveParticles: number
	//	- MaxParticles: number

	LUA->CreateTable();
	LUA->PushString(scene->GetComputeDevice());
	LUA->SetField(-2, "ComputeDeviceName");
	LUA->PushNumber(scene->GetActiveParticles());
	LUA->SetField(-2, "ActiveParticles");
	LUA->PushNumber(scene->GetMaxParticles());
	LUA->SetField(-2, "MaxParticles");
	CATCH_GELLY_EXCEPTIONS();
	return 1;
}

#define GET_LUA_TABLE_NUMBER(name)                 \
	LUA->GetField(-1, #name);                      \
	name = static_cast<float>(LUA->GetNumber(-1)); \
	LUA->Pop();

#define GET_LUA_TABLE_VECTOR(name) \
	LUA->GetField(-1, #name);      \
	name##_v = LUA->GetVector(-1); \
	LUA->Pop();

#define GET_LUA_TABLE_MEMBER(type, name)          \
	float name;                                   \
	Vector name##_v;                              \
	if constexpr (std::is_same_v<type, Vector>) { \
		GET_LUA_TABLE_VECTOR(name);               \
	} else {                                      \
		GET_LUA_TABLE_NUMBER(name);               \
	}

LUA_FUNCTION(gelly_SetFluidProperties) {
	START_GELLY_EXCEPTIONS();
	LUA->CheckType(1, GarrysMod::Lua::Type::Table);

	GET_LUA_TABLE_MEMBER(float, Viscosity);
	GET_LUA_TABLE_MEMBER(float, Cohesion);
	GET_LUA_TABLE_MEMBER(float, SurfaceTension);
	GET_LUA_TABLE_MEMBER(float, VorticityConfinement);
	GET_LUA_TABLE_MEMBER(float, Adhesion);
	GET_LUA_TABLE_MEMBER(float, DynamicFriction);

	SetFluidProperties props = {};
	props.viscosity = Viscosity;
	props.cohesion = Cohesion;
	props.surfaceTension = SurfaceTension;
	props.vorticityConfinement = VorticityConfinement;
	props.adhesion = Adhesion;
	props.dynamicFriction = DynamicFriction;

	scene->SetFluidProperties(props);
	CATCH_GELLY_EXCEPTIONS();

	return 0;
}

LUA_FUNCTION(gelly_SetFluidVisualParams) {
	START_GELLY_EXCEPTIONS();
	LUA->CheckType(1, GarrysMod::Lua::Type::Table);

	GET_LUA_TABLE_MEMBER(float, Shininess);
	GET_LUA_TABLE_MEMBER(Vector, Absorption);
	GET_LUA_TABLE_MEMBER(float, RefractionStrength);

	PipelineFluidMaterial material = {};
	material.refractionStrength = RefractionStrength;

	compositor->SetFluidMaterial(material);
	CATCH_GELLY_EXCEPTIONS();
	return 0;
}

LUA_FUNCTION(gelly_ChangeParticleRadius) {
	START_GELLY_EXCEPTIONS();
	LUA->CheckType(1, GarrysMod::Lua::Type::Number);

	const auto newRadius = static_cast<float>(LUA->GetNumber(1));
	auto config = compositor->GetConfig();
	config.particleRadius = newRadius;
	compositor->SetConfig(config);

	scene->ChangeRadius(newRadius);
	CATCH_GELLY_EXCEPTIONS();
	return 0;
}

LUA_FUNCTION(gelly_Reset) {
	START_GELLY_EXCEPTIONS();
	scene->ClearParticles();
	CATCH_GELLY_EXCEPTIONS();
	return 0;
}

LUA_FUNCTION(gelly_ChangeThresholdRatio) {
	START_GELLY_EXCEPTIONS();
	LUA->CheckType(1, GarrysMod::Lua::Type::Number);  // Ratio

	auto ratio = static_cast<float>(LUA->GetNumber(1));
	auto config = compositor->GetConfig();
	config.thresholdRatio = ratio;

	compositor->SetConfig(config);
	CATCH_GELLY_EXCEPTIONS();
	return 0;
}

LUA_FUNCTION(gelly_SetRenderSettings) {
	START_GELLY_EXCEPTIONS();
	LUA->CheckType(1, GarrysMod::Lua::Type::Table);	 // new render settings

	auto config = compositor->GetConfig();

	GET_LUA_TABLE_MEMBER(int, SmoothingIterations);
	GET_LUA_TABLE_MEMBER(int, ThicknessIterations);

	config.filterIterations = SmoothingIterations;
	config.thicknessIterations = ThicknessIterations;

	compositor->SetConfig(config);
	CATCH_GELLY_EXCEPTIONS();
	return 0;
}

GMOD_MODULE_OPEN() {
	InjectConsoleWindow();
	if (const auto status = FileSystem::LoadFileSystem();
		status != FILESYSTEM_STATUS::OK) {
		LOG_ERROR("Failed to load file system: %d", status);
		return 0;
	}

	if (const auto status = MH_Initialize(); status != MH_OK) {
		LOG_ERROR(
			"Failed to initialize MinHook: %s", MH_StatusToString(status)
		);
		return 0;
	}

	LOG_INFO("Hello, world!");
	LOG_INFO("Grabbing initial information...");

	CViewSetup currentView = {};
	GetClientViewSetup(currentView);

	// If we didnt get an Ex version, bail--there really isnt a point to running
	// 16-bit Gelly. Its awful, theres so many limitations, and it just doesnt
	// work well.
	if (!GetD3DDevice()) {
		LUA->ThrowError(
			"Gelly has detected that the current GMod instance is running in "
			"DirectX 9 mode. Gelly requires Direct3D9Ex to function. Check if "
			"you have any launch options that force DirectX 9 mode--such as "
			"'-nod3d9ex' "
			"Certain optimization configs/mods might also force DirectX 9 "
			"mode. "
		);
	}

	context = MakeRenderContext(currentView.width, currentView.height);
	renderer = MakeFluidRenderer(context.get());
	simContext = MakeSimContext(
		static_cast<ID3D11Device *>(
			context->GetRenderAPIResource(RenderAPIResource::D3D11Device)
		),
		static_cast<ID3D11DeviceContext *>(
			context->GetRenderAPIResource(RenderAPIResource::D3D11DeviceContext)
		)
	);
	sim = MakeFluidSimulation(simContext.get());

	compositor = std::make_shared<GModCompositor>(
		PipelineType::STANDARD, renderer, context
	);

	scene = std::make_shared<Scene>(
		renderer, simContext, sim, DEFAULT_MAX_PARTICLES
	);

	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
	// lets check sig version and this version first
	LUA->GetField(-1, "VERSION");
	int version = static_cast<int>(LUA->GetNumber(-1));
	LUA->Pop();

	if (version != sigs::VERSION) {
		LOG_WARNING(
			"Sig mismatch! Expected %d, got %d", sigs::VERSION, version
		);
	}

	LOG_INFO("Disabling the queued material system...");
	DisableMaterialSystemThreading();

	LUA->CreateTable();
	DEFINE_LUA_FUNC(gelly, Composite);
	DEFINE_LUA_FUNC(gelly, Simulate);
	DEFINE_LUA_FUNC(gelly, GetStatus);
	DEFINE_LUA_FUNC(gelly, AddParticles);
	DEFINE_LUA_FUNC(gelly, LoadMap);
	DEFINE_LUA_FUNC(gelly, AddObject);
	DEFINE_LUA_FUNC(gelly, AddPlayerObject);
	DEFINE_LUA_FUNC(gelly, RemoveObject);
	DEFINE_LUA_FUNC(gelly, SetObjectPosition);
	DEFINE_LUA_FUNC(gelly, SetObjectRotation);
	DEFINE_LUA_FUNC(gelly, SetFluidProperties);
	DEFINE_LUA_FUNC(gelly, SetFluidVisualParams);
	DEFINE_LUA_FUNC(gelly, ChangeParticleRadius);
	DEFINE_LUA_FUNC(gelly, Reset);
	DEFINE_LUA_FUNC(gelly, ChangeThresholdRatio);
	DEFINE_LUA_FUNC(gelly, SetRenderSettings);
	LUA->SetField(-2, "gelly");
	LUA->Pop();

	return 0;
}

GMOD_MODULE_CLOSE() {
	LOG_INFO("Goodbye, world!");
	if (const auto status = MH_Uninitialize(); status != MH_OK) {
		LOG_ERROR(
			"Failed to uninitialize MinHook: %s", MH_StatusToString(status)
		);
		return 0;
	}

	compositor.reset();
	scene.reset();
	renderer.reset();
	context.reset();
	sim.reset();
	simContext.reset();
	RemoveConsoleWindow();
	return 0;
}
