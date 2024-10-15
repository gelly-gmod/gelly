#include <GarrysMod/Lua/Interface.h>
// clang-format off
#include "scene/Scene.h"
// clang-format on

#include <cstdio>

#include "logging/global-macros.h"
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
// According to MSDN, this means we're targeting Windows 10 and above
#define _WIN32_WINNT 0x0A00
#include <DbgHelp.h>
#include <GMFS.h>
#include <MinHook.h>
#include <Windows.h>

#include <fstream>

#include "binaries/gbp.h"
#include "composite/GModCompositor.h"
#include "exceptions/generate-stack-trace.h"
#include "exceptions/get-stack-size.h"
#include "logging/helpers/dev-console-logging.h"
#include "luajit/raw-lua-access.h"
#include "luajit/setup-atpanic-handler.h"
#include "scene/asset-cache.h"
#include "source/D3DDeviceWrapper.h"
#include "source/GetCubemap.h"
#include "source/IBaseClientDLL.h"
#include "source/IVRenderView.h"
#include "tracy/Tracy.hpp"
#include "util/GellySharedPtrs.h"
#include "util/lua-table.h"
#include "util/parse-asset-from-filesystem.h"
#include "version.h"

#define DEFINE_LUA_FUNC(namespace, name)    \
	LUA->PushCFunction(namespace##_##name); \
	LUA->SetField(-2, #name);

#define START_GELLY_EXCEPTIONS() try {
#define CATCH_GELLY_EXCEPTIONS()                                 \
	}                                                            \
	catch (const std::exception &e) {                            \
		LOG_ERROR("GELLY EXCEPTION: %s", e.what());              \
		LUA->ThrowError(e.what());                               \
	}                                                            \
	catch (...) {                                                \
		LOG_ERROR("GELLY EXCEPTION: Unknown exception caught!"); \
		LUA->ThrowError("Unknown exception caught!");            \
	}

static std::shared_ptr<gelly::renderer::Device> rendererDevice = nullptr;
static std::shared_ptr<GModCompositor> compositor = nullptr;
static std::shared_ptr<Scene> scene = nullptr;
static std::shared_ptr<ISimContext> simContext = nullptr;
static std::shared_ptr<IFluidSimulation> sim = nullptr;
static std::shared_ptr<luajit::LuaShared> luaShared = nullptr;
static std::shared_ptr<AssetCache> assetCache = nullptr;

constexpr int DEFAULT_MAX_PARTICLES = 100000;
constexpr int MAXIMUM_PARTICLES = 10000000;
constexpr DWORD LUAJIT_UNHANDLED_PCALL = 0xE24C4A02;

static PVOID emergencyHandler = nullptr;
LONG WINAPI SaveLogInEmergency(LPEXCEPTION_POINTERS exceptionInfo) {
	// We want to log the exception code
	switch (exceptionInfo->ExceptionRecord->ExceptionCode) {
		case EXCEPTION_ACCESS_VIOLATION:
			LOG_ERROR("EMERGENCY EXCEPTION HANDLER: Access violation detected!"
			);
			break;
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
			LOG_ERROR(
				"EMERGENCY EXCEPTION HANDLER: Array bounds exceeded detected!"
			);
			break;
		case LUAJIT_UNHANDLED_PCALL:
		case DBG_CONTROL_C:
		case EXCEPTION_BREAKPOINT:
			return EXCEPTION_CONTINUE_EXECUTION;
		case EXCEPTION_DATATYPE_MISALIGNMENT:
			LOG_ERROR(
				"EMERGENCY EXCEPTION HANDLER: Data type misalignment detected!"
			);
			break;
		case EXCEPTION_STACK_INVALID:
			LOG_ERROR(
				"EMERGENCY EXCEPTION HANDLER: Stack smash/corruption detected!"
			);
			break;
		default:
			LOG_ERROR(
				"EMERGENCY EXCEPTION HANDLER: Unknown exception detected! "
				"(Code: %lu)",
				exceptionInfo->ExceptionRecord->ExceptionCode
			);
			break;
	}

	// Now we can also log some more information about the exception
	LOG_ERROR(
		"EMERGENCY EXCEPTION HANDLER: Exception address: 0x%p",
		exceptionInfo->ExceptionRecord->ExceptionAddress
	);

	LOG_ERROR(
		"EMERGENCY EXCEPTION HANDLER: Exception flags: 0x%08X",
		exceptionInfo->ExceptionRecord->ExceptionFlags
	);

	for (DWORD i = 0; i < exceptionInfo->ExceptionRecord->NumberParameters;
		 i++) {
		LOG_ERROR(
			"EMERGENCY EXCEPTION HANDLER: Parameter %d: 0x%p",
			i,
			exceptionInfo->ExceptionRecord->ExceptionInformation[i]
		);
	}

	LOG_ERROR(
		"EMERGENCY EXCEPTION HANDLER: RIP: 0x%p",
		exceptionInfo->ContextRecord->Rip
	);

	// log some gelly info if we can
	if (scene) {
		LOG_ERROR("EMERGENCY EXCEPTION HANDLER: Scene is still uncorrupted!");
		LOG_ERROR(
			"EMERGENCY EXCEPTION HANDLER: Active particles: %d",
			scene->GetActiveParticles()
		);
		LOG_ERROR(
			"EMERGENCY EXCEPTION HANDLER: Max particles: %d",
			scene->GetMaxParticles()
		);
		LOG_ERROR(
			"EMERGENCY EXCEPTION HANDLER: Compute device at time of crash: %s",
			scene->GetComputeDevice()
		);
	}

	if (compositor) {
		LOG_ERROR(
			"EMERGENCY EXCEPTION HANDLER: Compositor is still uncorrupted!"
		);
		LOG_ERROR(
			"EMERGENCY EXCEPTION HANDLER: Cubemap strength: %f",
			compositor->GetConfig().cubemapStrength
		);
		LOG_ERROR(
			"EMERGENCY EXCEPTION HANDLER: Particle radius: %f",
			compositor->GetConfig().particleRadius
		);
		LOG_ERROR(
			"EMERGENCY EXCEPTION HANDLER: Threshold ratio: %f",
			compositor->GetConfig().thresholdRatio
		);
		LOG_ERROR(
			"EMERGENCY EXCEPTION HANDLER: Diffuse motion blur: %f",
			compositor->GetConfig().diffuseMotionBlur
		);
		LOG_ERROR(
			"EMERGENCY EXCEPTION HANDLER: Diffuse scale: %f",
			compositor->GetConfig().diffuseScale
		);
		LOG_ERROR(
			"EMERGENCY EXCEPTION HANDLER: Refraction strength: %f",
			compositor->GetConfig().refractionStrength
		);
	}

	if (SymInitialize(GetCurrentProcess(), nullptr, TRUE)) {
		DWORD symOptions = SymGetOptions();
		symOptions |= SYMOPT_LOAD_LINES;
		symOptions |= SYMOPT_UNDNAME;
		symOptions |= SYMOPT_FAIL_CRITICAL_ERRORS;
		SymSetOptions(symOptions);

		// we need to add the lua/bin path to the symbol path
		char workingDir[MAX_PATH];
		GetCurrentDirectoryA(MAX_PATH, workingDir);

		std::string symbolPath = workingDir;
		symbolPath += R"(\garrysmod\lua\bin)";

		LOG_INFO(
			"EMERGENCY EXCEPTION HANDLER: Symbol path: %s", symbolPath.c_str()
		);

		if (!SymSetSearchPath(GetCurrentProcess(), symbolPath.c_str())) {
			LOG_ERROR(
				"EMERGENCY EXCEPTION HANDLER: Failed to set symbol search "
				"path! "
				"Stack trace will be incomplete."
			);
		} else {
			const auto stackTrace = logging::exceptions::GetFormattedStackTrace(
				exceptionInfo->ContextRecord
			);

			LOG_INFO("Stack trace: \n%s", stackTrace.c_str());
		}
	} else {
		LOG_ERROR(
			"EMERGENCY EXCEPTION HANDLER: Failed to initialize symbol handler! "
			"Stack trace will be incomplete."
		);
	}

	LOG_ERROR("EMERGENCY EXCEPTION HANDLER: Saving log to file...");
	LOG_SAVE_TO_FILE();
	LOG_ERROR("EMERGENCY EXCEPTION HANDLER: Log saved to file!");
	LOG_ERROR("EMERGENCY EXCEPTION HANDLER: Crashing now!");
	return EXCEPTION_EXECUTE_HANDLER;
}

void DumpLuaStack(const std::string &caption, GarrysMod::Lua::ILuaBase *LUA) {
	LOG_INFO("Stack dump (%s):", caption.c_str());
	const auto stackSize = LUA->Top();
	LOG_INFO("Stack size: %d", stackSize);

	for (int i = 1; i <= stackSize; i++) {
		std::string value = "<unknown>";
		if (LUA->GetType(i) == GarrysMod::Lua::Type::Number) {
			value = std::to_string(LUA->GetNumber(i));
		} else if (LUA->GetType(i) == GarrysMod::Lua::Type::String) {
			value = LUA->GetString(i);
		} else if (LUA->GetType(i) == GarrysMod::Lua::Type::Bool) {
			value = LUA->GetBool(i) ? "true" : "false";
		} else if (LUA->GetType(i) == GarrysMod::Lua::Type::Table) {
			value = "table#" + std::to_string(LUA->ObjLen(i));
		}
		LOG_INFO(
			"Stack[%d]: %s (%s)",
			i,
			LUA->GetTypeName(LUA->GetType(i)),
			value.c_str()
		);
	}
}

LUA_FUNCTION(gelly_Render) {
	START_GELLY_EXCEPTIONS();
	compositor->Render();
	CATCH_GELLY_EXCEPTIONS();
	return 0;
}
LUA_FUNCTION(gelly_Composite) {
	START_GELLY_EXCEPTIONS()
	compositor->Composite();
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
	scene->LoadMap(assetCache, LUA->GetString(1));

	CATCH_GELLY_EXCEPTIONS()
	return 0;
}

LUA_FUNCTION(gelly_AddObject) {
	START_GELLY_EXCEPTIONS()

	// The lua side will pass through the triangle mesh
	LUA->CheckType(
		1, GarrysMod::Lua::Type::String
	);	// Mesh name (asset name, to us)
	LUA->CheckType(2, GarrysMod::Lua::Type::Number);  // Ent index
	const auto assetName = LUA->GetString(1);
	const auto entIndex = static_cast<EntIndex>(LUA->GetNumber(2));

	if (!assetCache->FetchAsset(assetName).has_value()) {
		LOG_WARNING("Asset cache miss on asset %s", assetName);

		// We'll just parse the asset here
		auto vertices =
			gelly::gmod::helpers::ParseAssetFromFilesystem(assetName);

		assetCache->InsertAsset(assetName, vertices.data(), vertices.size());
		LOG_WARNING(
			"Asset %s inserted into cache with %d vertices",
			assetName,
			vertices.size()
		);
	}

	scene->AddEntity(entIndex, assetCache, assetName);

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

LUA_FUNCTION(gelly_AddForcefieldObject) {
	START_GELLY_EXCEPTIONS();
	LUA->CheckType(1, GarrysMod::Lua::Type::Table);	 // Forcefield data

	auto nullVector = Vector{};
	nullVector.x = 0;
	nullVector.y = 0;
	nullVector.z = 0;

	const auto luaTable = gelly::gmod::helpers::LuaTable(LUA);
	const Vector position = luaTable.Get("Position", nullVector);
	const float radius = luaTable.Get("Radius", 0.f);
	const float strength = luaTable.Get("Strength", 0.f);
	const bool linearFalloff = luaTable.Get("LinearFalloff", false);
	const int mode = luaTable.Get(
		"Mode", static_cast<int>(NvFlexExtForceMode::eNvFlexExtModeForce)
	);

	ObjectCreationParams forcefield = {};
	forcefield.shape = ObjectShape::FORCEFIELD;
	forcefield.shapeData = ObjectCreationParams::Forcefield{
		.position = {position.x, position.y, position.z},
		.radius = radius,
		.strength = strength,
		.mode = static_cast<NvFlexExtForceMode>(mode),
		.linearFalloff = linearFalloff
	};

	const auto forcefieldHandle = scene->AddForcefield(forcefield);
	LUA->PushNumber(forcefieldHandle);
	CATCH_GELLY_EXCEPTIONS();

	return 1;
}

LUA_FUNCTION(gelly_UpdateForcefieldPosition) {
	START_GELLY_EXCEPTIONS();
	LUA->CheckType(1, GarrysMod::Lua::Type::Number);  // Handle
	LUA->CheckType(2, GarrysMod::Lua::Type::Vector);  // Position

	scene->UpdateForcefieldPosition(
		static_cast<ObjectHandle>(LUA->GetNumber(1)), LUA->GetVector(2)
	);

	CATCH_GELLY_EXCEPTIONS();
	return 0;
}

LUA_FUNCTION(gelly_RemoveForcefieldObject) {
	START_GELLY_EXCEPTIONS();
	LUA->CheckType(1, GarrysMod::Lua::Type::Number);  // Handle
	scene->RemoveForcefield(static_cast<ObjectHandle>(LUA->GetNumber(1)));
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

#define GET_LUA_TABLE_BOOL(name) \
	LUA->GetField(-1, #name);    \
	name##_b = LUA->GetBool(-1); \
	LUA->Pop();

#define GET_LUA_TABLE_MEMBER(type, name)               \
	bool name##_b;                                     \
	float name;                                        \
	Vector name##_v;                                   \
	if constexpr (std::is_same_v<type, Vector>) {      \
		GET_LUA_TABLE_VECTOR(name);                    \
	} else if constexpr (std::is_same_v<type, bool>) { \
		GET_LUA_TABLE_BOOL(name);                      \
	} else {                                           \
		GET_LUA_TABLE_NUMBER(name);                    \
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
	GET_LUA_TABLE_MEMBER(float, RestDistanceRatio);

	SetFluidProperties props = {};
	props.viscosity = Viscosity;
	props.cohesion = Cohesion;
	props.surfaceTension = SurfaceTension;
	props.vorticityConfinement = VorticityConfinement;
	props.adhesion = Adhesion;
	props.dynamicFriction = DynamicFriction;
	props.restDistanceRatio = RestDistanceRatio;

	scene->SetFluidProperties(props);
	CATCH_GELLY_EXCEPTIONS();

	return 0;
}

LUA_FUNCTION(gelly_SetFluidMaterial) {
	START_GELLY_EXCEPTIONS();
	LUA->CheckType(1, GarrysMod::Lua::Type::Table);

	GET_LUA_TABLE_MEMBER(float, Roughness);
	GET_LUA_TABLE_MEMBER(bool, IsSpecularTransmission);
	GET_LUA_TABLE_MEMBER(float, RefractiveIndex);
	GET_LUA_TABLE_MEMBER(Vector, DiffuseColor);

	PipelineFluidMaterial material = {};
	material.roughness = Roughness;
	material.specularTransmission =
		IsSpecularTransmission_b
			? 1.f
			: 0.f;	// generally easier on the GPU-side
					// to use a float as a boolean (bool registers have issues)
	material.refractiveIndex = RefractiveIndex;

	material.diffuseColor[0] = DiffuseColor_v.x;
	material.diffuseColor[1] = DiffuseColor_v.y;
	material.diffuseColor[2] = DiffuseColor_v.z;

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
	GET_LUA_TABLE_MEMBER(float, RefractionStrength);

	config.refractionStrength = RefractionStrength;

	compositor->SetConfig(config);
	CATCH_GELLY_EXCEPTIONS();
	return 0;
}

LUA_FUNCTION(gelly_SetDiffuseScale) {
	START_GELLY_EXCEPTIONS();
	LUA->CheckType(1, GarrysMod::Lua::Type::Number);

	auto scale = static_cast<float>(LUA->GetNumber(1));
	auto config = compositor->GetConfig();
	config.diffuseScale = scale;
	compositor->SetConfig(config);

	CATCH_GELLY_EXCEPTIONS();
	return 0;
}

LUA_FUNCTION(gelly_SetDiffuseMotionBlur) {
	START_GELLY_EXCEPTIONS();

	LUA->CheckType(1, GarrysMod::Lua::Type::Number);

	auto blur = static_cast<float>(LUA->GetNumber(1));
	auto config = compositor->GetConfig();
	config.diffuseMotionBlur = blur;
	compositor->SetConfig(config);

	CATCH_GELLY_EXCEPTIONS();
	return 0;
}

LUA_FUNCTION(gelly_SetTimeStepMultiplier) {
	START_GELLY_EXCEPTIONS();
	LUA->CheckType(1, GarrysMod::Lua::Type::Number);

	auto multiplier = static_cast<float>(LUA->GetNumber(1));
	scene->SetTimeStepMultiplier(multiplier);
	CATCH_GELLY_EXCEPTIONS();
	return 0;
}

LUA_FUNCTION(gelly_ChangeMaxParticles) {
	START_GELLY_EXCEPTIONS();
	LUA->CheckType(1, GarrysMod::Lua::Type::Number);

	const auto newMax = static_cast<int>(LUA->GetNumber(1));
	if (newMax > MAXIMUM_PARTICLES) {
		LUA->ThrowError("Cannot set max particles above 1,000,000!");
	}

	// For safe measure we'll honestly just need to remove the sim and scene,
	// although the sim context should be fine
	unsigned int originalWidth = compositor->GetWidth();
	unsigned int originalHeight = compositor->GetHeight();
	float originalScale = compositor->GetScale();

	sim.reset();
	sim = MakeFluidSimulation(simContext.get());
	scene.reset();
	compositor.reset();
	scene = std::make_shared<Scene>(simContext, sim, newMax);
	compositor = std::make_shared<GModCompositor>(
		PipelineType::STANDARD,
		scene->GetSimData(),
		rendererDevice,
		originalWidth,
		originalHeight,
		newMax,
		originalScale
	);

	scene->SetAbsorptionModifier(compositor->GetAbsorptionModifier());
	scene->Initialize();

	LUA->Pop();

	// then restore the map
	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
	LUA->GetField(-1, "game");
	LUA->GetField(-1, "GetMap");
	LUA->Call(0, 1);

	const auto *map = LUA->GetString(-1);
	const std::string fileName = "maps/" + std::string(map) + ".bsp";

	scene->LoadMap(assetCache, fileName);

	// then pop off the special table and the game table
	LUA->Pop(2);
	CATCH_GELLY_EXCEPTIONS();
	return 0;
}

LUA_FUNCTION(gelly_GetVersion) {
	START_GELLY_EXCEPTIONS();
	LUA->PushString(GELLY_VERSION);
	CATCH_GELLY_EXCEPTIONS();
	return 1;
}

LUA_FUNCTION(gelly_SetGellySettings) {
	START_GELLY_EXCEPTIONS();
	LUA->CheckType(1, GarrysMod::Lua::Type::Table);	 // settings

	GET_LUA_TABLE_MEMBER(float, FilterIterations);
	GET_LUA_TABLE_MEMBER(bool, EnableGPUSynchronization);
	GET_LUA_TABLE_MEMBER(bool, EnableGPUTiming);

	int filterIterations = static_cast<int>(FilterIterations);
	auto currentSettings = compositor->GetGellySettings();
	currentSettings.filterIterations = filterIterations;
	currentSettings.enableGPUSynchronization = EnableGPUSynchronization_b;
	currentSettings.enableGPUTiming = EnableGPUTiming_b;

	compositor->UpdateGellySettings(currentSettings);
	CATCH_GELLY_EXCEPTIONS();
	return 0;
}

LUA_FUNCTION(gelly_GetGellySettings) {
	START_GELLY_EXCEPTIONS();
	auto currentSettings = compositor->GetGellySettings();

	LUA->CreateTable();
	LUA->PushNumber(currentSettings.filterIterations);
	LUA->SetField(-2, "FilterIterations");
	LUA->PushBool(currentSettings.enableGPUSynchronization);
	LUA->SetField(-2, "EnableGPUSynchronization");
	LUA->PushBool(currentSettings.enableGPUTiming);
	LUA->SetField(-2, "EnableGPUTiming");

	CATCH_GELLY_EXCEPTIONS();
	return 1;
}

LUA_FUNCTION(gelly_GetGellyTimings) {
	START_GELLY_EXCEPTIONS();
	auto timings = compositor->FetchGellyTimings();

	LUA->CreateTable();
	LUA->PushNumber(timings.albedoDownsampling);
	LUA->SetField(-2, "AlbedoDownsampling");
	LUA->PushNumber(timings.ellipsoidSplatting);
	LUA->SetField(-2, "EllipsoidSplatting");
	LUA->PushNumber(timings.surfaceFiltering);
	LUA->SetField(-2, "SurfaceFiltering");
	LUA->PushNumber(timings.rawNormalEstimation);
	LUA->SetField(-2, "RawNormalEstimation");
	LUA->PushBool(timings.isDisjoint);
	LUA->SetField(-2, "IsDisjoint");

	CATCH_GELLY_EXCEPTIONS();
	return 1;
}

LUA_FUNCTION(gelly_ConfigureSim) {
	START_GELLY_EXCEPTIONS();
	LUA->CheckType(1, GarrysMod::Lua::Type::Table);

	GET_LUA_TABLE_MEMBER(float, Substeps);
	GET_LUA_TABLE_MEMBER(float, Iterations);
	GET_LUA_TABLE_MEMBER(float, RelaxationFactor);
	GET_LUA_TABLE_MEMBER(float, CollisionDistance);
	GET_LUA_TABLE_MEMBER(float, Gravity);

	int substeps = static_cast<int>(Substeps);
	int iterations = static_cast<int>(Iterations);

	scene->Configure(
		{.substeps = substeps,
		 .iterations = iterations,
		 .relaxationFactor = RelaxationFactor,
		 .collisionDistance = CollisionDistance,
		 .gravity = Gravity}
	);

	CATCH_GELLY_EXCEPTIONS();

	return 0;
}

// We split these into free functions since these will likely be called very
// often
LUA_FUNCTION(gelly_SetSunDirection) {
	START_GELLY_EXCEPTIONS();
	LUA->CheckType(1, GarrysMod::Lua::Type::Vector);

	const auto sunDirection = LUA->GetVector(1);
	auto config = compositor->GetConfig();

	config.sunDirection[0] = sunDirection.x;
	config.sunDirection[1] = sunDirection.y;
	config.sunDirection[2] = sunDirection.z;

	compositor->SetConfig(config);
	CATCH_GELLY_EXCEPTIONS();
	return 0;
}

LUA_FUNCTION(gelly_SetSunEnabled) {
	START_GELLY_EXCEPTIONS();
	LUA->CheckType(1, GarrysMod::Lua::Type::Bool);

	const auto enabled = LUA->GetBool(1);
	auto config = compositor->GetConfig();
	config.sunEnabled = enabled ? 1.f : 0.f;

	compositor->SetConfig(config);
	CATCH_GELLY_EXCEPTIONS();
	return 0;
}

LUA_FUNCTION(gelly_IsRWDIBuild) {
	START_GELLY_EXCEPTIONS();
#ifdef GELLY_ENABLE_RENDERDOC_CAPTURES
	LUA->PushBool(true);
#else
	LUA->PushBool(false);
#endif
	CATCH_GELLY_EXCEPTIONS();
	return 1;
}

#ifdef GELLY_ENABLE_RENDERDOC_CAPTURES
LUA_FUNCTION(gelly_ReloadAllShaders) {
	START_GELLY_EXCEPTIONS();
	compositor->ReloadAllShaders();
	CATCH_GELLY_EXCEPTIONS();
	return 0;
}
#endif

LUA_FUNCTION(gelly_ChangeResolution) {
	START_GELLY_EXCEPTIONS();
	float width = static_cast<float>(LUA->GetNumber(1));
	float height = static_cast<float>(LUA->GetNumber(2));
	float scale = static_cast<float>(LUA->GetNumber(3)
	);	// used when the scale is changed but not resolution

	compositor->ChangeResolution(width, height, scale);

	CATCH_GELLY_EXCEPTIONS();
	return 0;
}

extern "C" __declspec(dllexport) int gmod13_open(lua_State *L) {
	GarrysMod::Lua::ILuaBase *LUA = L->luabase;
#ifndef PRODUCTION_BUILD
	logging::StartDevConsoleLogging();
#endif

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

	START_GELLY_EXCEPTIONS()

	LOG_INFO("Hello, world!");
	LOG_INFO("Grabbing initial information...");
#ifndef _MSC_VER
	LOG_WARNING("This copy of Gelly was built with a non-MSVC compiler!");
	LOG_WARNING("The first load may throw a 'Module not found' error.")
#endif
	LOG_INFO("Creating temporary binaries...");
	for (const auto &binary : gbp::packedBinaries) {
		const auto binaryPath =
			std::filesystem::current_path() / binary.moduleName;

		if (exists(binaryPath)) {
			LOG_WARNING(
				"Found conflicting binary '%s', deleting...",
				binaryPath.string().c_str()
			);

			std::error_code ec;
			bool successfullyRemoved = std::filesystem::remove(binaryPath, ec);
			if (!successfullyRemoved) {
				LOG_WARNING(
					"Could not remove binary '%s'. Typically, this happens "
					"when Windoes does not want to let go of the DLL, implying "
					"usage (GWater2)",
					binaryPath.string().c_str()
				);

				// don't even continue with writing the new binary
				continue;
			}
		}

		std::ofstream binaryFile(binaryPath, std::ios::binary);
		binaryFile.write(
			reinterpret_cast<const char *>(binary.data), binary.dataSize
		);
		binaryFile.close();

		LOG_INFO(
			"Wrote binary '%s' to disk for loading...",
			binaryPath.string().c_str()
		);
	}

	CViewSetup currentView = {};
	GetClientViewSetup(currentView);

	if (!GetD3DDevice()) {
		LUA->ThrowError(
			"Gelly has detected that the current GMod instance is running "
			"in "
			"DirectX 9 mode. Gelly requires Direct3D9Ex to function. Check "
			"if "
			"you have any launch options that force DirectX 9 mode--such "
			"as "
			"'-nod3d9ex' "
			"Certain optimization configs/mods might also force DirectX 9 "
			"mode. "
		);
	}

	assetCache = std::make_shared<AssetCache>();
	rendererDevice = std::make_shared<gelly::renderer::Device>();

	simContext = MakeSimContext(
		rendererDevice->GetRawDevice().Get(),
		rendererDevice->GetRawDeviceContext().Get()
	);

	sim = MakeFluidSimulation(simContext.get());

	scene = std::make_shared<Scene>(simContext, sim, DEFAULT_MAX_PARTICLES);

	compositor = std::make_shared<GModCompositor>(
		PipelineType::STANDARD,
		scene->GetSimData(),
		rendererDevice,
		currentView.width,
		currentView.height,
		DEFAULT_MAX_PARTICLES,
		1.f
	);

	scene->SetAbsorptionModifier(compositor->GetAbsorptionModifier());
	scene->Initialize();

	DumpLuaStack("Getting global table", LUA);
	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
	// lets check sig version and this version first
	LUA->GetField(-1, "VERSION");
	DumpLuaStack("Getting version", LUA);
	int version = static_cast<int>(LUA->GetNumber(-1));
	LUA->Pop();
	DumpLuaStack("Popping version", LUA);

	if (version != sigs::VERSION) {
		LOG_WARNING(
			"Sig mismatch! Expected %d, got %d", sigs::VERSION, version
		);
	}

	LOG_INFO("Disabling the queued material system...");
	DisableMaterialSystemThreading();

	DumpLuaStack("Preparing gelly table", LUA);
	LUA->CreateTable();
	DumpLuaStack("Creating gelly table", LUA);
	DEFINE_LUA_FUNC(gelly, Render);
	DEFINE_LUA_FUNC(gelly, Composite);
	DEFINE_LUA_FUNC(gelly, Simulate);
	DEFINE_LUA_FUNC(gelly, GetStatus);
	DEFINE_LUA_FUNC(gelly, AddParticles);
	DEFINE_LUA_FUNC(gelly, LoadMap);
	DEFINE_LUA_FUNC(gelly, AddObject);
	DEFINE_LUA_FUNC(gelly, AddPlayerObject);
	DEFINE_LUA_FUNC(gelly, RemoveObject);
	DEFINE_LUA_FUNC(gelly, AddForcefieldObject);
	DEFINE_LUA_FUNC(gelly, UpdateForcefieldPosition);
	DEFINE_LUA_FUNC(gelly, RemoveForcefieldObject);
	DEFINE_LUA_FUNC(gelly, SetObjectPosition);
	DEFINE_LUA_FUNC(gelly, SetObjectRotation);
	DEFINE_LUA_FUNC(gelly, SetFluidProperties);
	DEFINE_LUA_FUNC(gelly, SetFluidMaterial);
	DEFINE_LUA_FUNC(gelly, ChangeParticleRadius);
	DEFINE_LUA_FUNC(gelly, Reset);
	DEFINE_LUA_FUNC(gelly, ChangeThresholdRatio);
	DEFINE_LUA_FUNC(gelly, SetRenderSettings);
	DEFINE_LUA_FUNC(gelly, SetDiffuseScale);
	DEFINE_LUA_FUNC(gelly, SetDiffuseMotionBlur);
	DEFINE_LUA_FUNC(gelly, SetTimeStepMultiplier);
	DEFINE_LUA_FUNC(gelly, ChangeMaxParticles);
	DEFINE_LUA_FUNC(gelly, GetVersion);
	DEFINE_LUA_FUNC(gelly, SetGellySettings);
	DEFINE_LUA_FUNC(gelly, GetGellySettings);
	DEFINE_LUA_FUNC(gelly, GetGellyTimings);
	DEFINE_LUA_FUNC(gelly, ConfigureSim);
	DEFINE_LUA_FUNC(gelly, SetSunDirection);
	DEFINE_LUA_FUNC(gelly, SetSunEnabled);
	DEFINE_LUA_FUNC(gelly, IsRWDIBuild);
	DEFINE_LUA_FUNC(gelly, ChangeResolution);
#ifdef GELLY_ENABLE_RENDERDOC_CAPTURES
	DEFINE_LUA_FUNC(gelly, ReloadAllShaders);
#endif
	DumpLuaStack("After defining functions", LUA);
	LUA->SetField(-2, "gelly");
	DumpLuaStack("Setting gelly table", LUA);
	LUA->Pop();
	DumpLuaStack("Popping global table", LUA);

	CATCH_GELLY_EXCEPTIONS();

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

	assetCache.reset();
	compositor.reset();
	scene.reset();
	sim.reset();
	rendererDevice.reset();
	simContext.reset();

	LOG_SAVE_TO_FILE();
#ifndef PRODUCTION_BUILD
	logging::StopDevConsoleLogging();
#endif
	return 0;
}
