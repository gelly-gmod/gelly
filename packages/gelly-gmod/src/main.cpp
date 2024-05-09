#include <GarrysMod/Lua/Interface.h>

#include <cstdio>

#include "LoggingMacros.h"
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <GMFS.h>
#include <MinHook.h>
#include <Windows.h>

#include <map>

#include "Gelly.h"
#include "source/D3DDeviceWrapper.h"
#include "source/GetCubemap.h"
#include "source/IBaseClientDLL.h"
#include "source/IServerGameEnts.h"
#include "source/IVEngineServer.h"
#include "source/IVRenderView.h"
#include "tracy/Tracy.hpp"

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

static GellyIntegration *gelly = nullptr;
static std::map<ObjectHandle, int> handleToEntIndexMap;

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

LUA_FUNCTION(gelly_Render) {
	START_GELLY_EXCEPTIONS()
	gelly->Render();
	CATCH_GELLY_EXCEPTIONS()
	return 0;
}

LUA_FUNCTION(gelly_Composite) {
	START_GELLY_EXCEPTIONS()
	gelly->Composite();
	CATCH_GELLY_EXCEPTIONS()
	return 0;
}

LUA_FUNCTION(gelly_Simulate) {
	START_GELLY_EXCEPTIONS()
	LUA->CheckType(1, GarrysMod::Lua::Type::Number);  // Delta time
	auto dt = static_cast<float>(LUA->GetNumber(1));

	gelly->Simulate(dt);
	CATCH_GELLY_EXCEPTIONS()
	return 0;
}

LUA_FUNCTION(gelly_LoadMap) {
	START_GELLY_EXCEPTIONS()
	if (!gelly->IsInteractive()) {
		return 0;
	}

	LUA->CheckType(1, GarrysMod::Lua::Type::String);  // Map name
	gelly->LoadMap(LUA->GetString(1));
	CATCH_GELLY_EXCEPTIONS()
	return 0;
}

LUA_FUNCTION(gelly_AddObject) {
	START_GELLY_EXCEPTIONS()
	if (!gelly->IsInteractive()) {
		LUA->PushNumber(INVALID_OBJECT_HANDLE);
		return 1;
	}

	// The lua side will pass through the triangle mesh
	LUA->CheckType(1, GarrysMod::Lua::Type::Table);	  // Mesh
	LUA->CheckType(2, GarrysMod::Lua::Type::Number);  // Ent index
	int entIndex = static_cast<int>(LUA->GetNumber(2));
	LUA->Pop();	 // to not interfere with the loop

	const uint32_t vertexCount = LUA->ObjLen(1);
	if (vertexCount <= 0) {
		LUA->ThrowError("Cannot create object with no vertices!");
	}

	auto *vertices = new Vector[vertexCount];

	for (uint32_t i = 0; i < vertexCount; i++) {
		LUA->PushNumber(i + 1);
		LUA->GetTable(-2);
		const auto vertex = LUA->GetVector(-1);
		vertices[i] = vertex;
		LUA->Pop();
	}

	// now we just make some useless indices
	uint32_t *indices = new uint32_t[vertexCount];

	for (uint32_t i = 0; i < vertexCount; i += 3) {
		indices[i + 2] = i;
		indices[i + 1] = i + 1;
		indices[i] = i + 2;
	}

	ObjectCreationParams params = {};
	params.shape = ObjectShape::TRIANGLE_MESH;

	ObjectCreationParams::TriangleMesh meshParams = {};
	meshParams.vertices = reinterpret_cast<const float *>(vertices);
	meshParams.vertexCount = vertexCount;
	meshParams.indices32 = indices;
	meshParams.indexCount = vertexCount;
	meshParams.indexType =
		ObjectCreationParams::TriangleMesh::IndexType::UINT32;

	meshParams.scale[0] = 1.f;
	meshParams.scale[1] = 1.f;
	meshParams.scale[2] = 1.f;

	params.shapeData = meshParams;

	LOG_INFO("Creating object with %d vertices", vertexCount);
	auto handle = gelly->GetSimulation()->GetScene()->CreateObject(params);
	gelly->GetSimulation()->GetScene()->Update();
	LOG_INFO("Created object with handle %d", handle);

	LUA->PushNumber(static_cast<double>(handle));

	delete[] vertices;
	delete[] indices;

	handleToEntIndexMap[handle] = entIndex;
	CATCH_GELLY_EXCEPTIONS();
	return 1;
}

LUA_FUNCTION(gelly_AddPlayerObject) {
	START_GELLY_EXCEPTIONS();
	if (!gelly->IsInteractive()) {
		LUA->PushNumber(INVALID_OBJECT_HANDLE);
		return 1;
	}

	LUA->CheckType(1, GarrysMod::Lua::Type::Number);  // Radius
	LUA->CheckType(2, GarrysMod::Lua::Type::Number);  // Half height
	LUA->CheckType(3, GarrysMod::Lua::Type::Number);  // Ent index

	const auto radius = static_cast<float>(LUA->GetNumber(1));
	const auto halfHeight = static_cast<float>(LUA->GetNumber(2));
	const auto entIndex = static_cast<int>(LUA->GetNumber(3));

	ObjectCreationParams params = {};
	params.shape = ObjectShape::CAPSULE;
	ObjectCreationParams::Capsule capsuleParams = {};
	capsuleParams.radius = radius;
	capsuleParams.halfHeight = halfHeight;

	params.shapeData = capsuleParams;

	LOG_INFO(
		"Creating player object with radius %f and half height %f",
		radius,
		halfHeight
	);
	auto handle = gelly->GetSimulation()->GetScene()->CreateObject(params);
	gelly->GetSimulation()->GetScene()->Update();

	handleToEntIndexMap[handle] = entIndex;
	LOG_INFO("Created player object with handle %d", handle);

	LUA->PushNumber(static_cast<double>(handle));
	CATCH_GELLY_EXCEPTIONS();
	return 1;
}

LUA_FUNCTION(gelly_RemoveObject) {
	START_GELLY_EXCEPTIONS();
	if (!gelly->IsInteractive()) {
		return 0;
	}

	LUA->CheckType(1, GarrysMod::Lua::Type::Number);  // Handle
	gelly->GetSimulation()->GetScene()->RemoveObject(
		static_cast<ObjectHandle>(LUA->GetNumber(1))
	);
	CATCH_GELLY_EXCEPTIONS();
	return 0;
}

LUA_FUNCTION(gelly_SetObjectPosition) {
	START_GELLY_EXCEPTIONS();
	if (!gelly->IsInteractive()) {
		return 0;
	}

	LUA->CheckType(1, GarrysMod::Lua::Type::Number);  // Handle
	LUA->CheckType(2, GarrysMod::Lua::Type::Vector);  // Position

	gelly->GetSimulation()->GetScene()->SetObjectPosition(
		static_cast<ObjectHandle>(LUA->GetNumber(1)),
		LUA->GetVector(2).x,
		LUA->GetVector(2).y,
		LUA->GetVector(2).z
	);
	CATCH_GELLY_EXCEPTIONS();
	return 0;
}

inline float rad(float deg) { return deg * (XM_PI / 180.0); }

LUA_FUNCTION(gelly_SetObjectRotation) {
	START_GELLY_EXCEPTIONS();
	if (!gelly->IsInteractive()) {
		return 0;
	}

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

	gelly->GetSimulation()->GetScene()->SetObjectQuaternion(
		static_cast<ObjectHandle>(LUA->GetNumber(1)),
		quat.y,
		quat.z,
		quat.w,
		quat.x
	);

	CATCH_GELLY_EXCEPTIONS();
	return 0;
}

LUA_FUNCTION(gelly_AddParticles) {
	START_GELLY_EXCEPTIONS();
	if (!gelly->IsInteractive()) {
		return 0;
	}

	LUA->CheckType(1, GarrysMod::Lua::Type::Table);	 // Particles

	const uint32_t particleCount = LUA->ObjLen(1);
	auto *cmdList = gelly->GetSimulation()->CreateCommandList();

	if (gelly->IsPerParticleAbsorptionSupported()) {
		gelly->GetRenderer()->PullPerParticleData();
	}

	const uint32_t startParticleIndex =
		gelly->GetSimulation()->GetSimulationData()->GetActiveParticles();

	uint32_t currentParticleIndex = 0;

	for (uint32_t i = 0; i < particleCount; i += 2) {
		LUA->PushNumber(i + 1);
		LUA->GetTable(-2);
		const auto position = LUA->GetVector(-1);
		LUA->PushNumber(i + 2);
		LUA->GetTable(-3);
		const auto velocity = LUA->GetVector(-1);

		cmdList->AddCommand(
			{ADD_PARTICLE,
			 AddParticle{
				 position.x,
				 position.y,
				 position.z,
				 velocity.x,
				 velocity.y,
				 velocity.z
			 }}
		);

		currentParticleIndex++;

		if (gelly->IsPerParticleAbsorptionSupported()) {
			const auto absorption = gelly->GetCurrentAbsorption();
			gelly->GetRenderer()->SetPerParticleAbsorption(
				startParticleIndex + currentParticleIndex,
				reinterpret_cast<const float *>(&absorption)
			);
		}

		LUA->Pop(2);
	}

	if (gelly->IsPerParticleAbsorptionSupported()) {
		gelly->GetRenderer()->PushPerParticleData();
	}

	gelly->GetSimulation()->ExecuteCommandList(cmdList);
	gelly->GetSimulation()->DestroyCommandList(cmdList);
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
	LUA->PushString(gelly->GetComputeDeviceName());
	LUA->SetField(-2, "ComputeDeviceName");
	LUA->PushNumber(
		gelly->GetSimulation()->GetSimulationData()->GetActiveParticles()
	);
	LUA->SetField(-2, "ActiveParticles");
	LUA->PushNumber(
		gelly->GetSimulation()->GetSimulationData()->GetMaxParticles()
	);
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
	if (!gelly->IsInteractive()) {
		return 0;
	}

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

	auto *cmdList = gelly->GetSimulation()->CreateCommandList();
	cmdList->AddCommand({SET_FLUID_PROPERTIES, props});
	gelly->GetSimulation()->ExecuteCommandList(cmdList);
	gelly->GetSimulation()->DestroyCommandList(cmdList);
	CATCH_GELLY_EXCEPTIONS();

	return 0;
}

LUA_FUNCTION(gelly_SetFluidVisualParams) {
	START_GELLY_EXCEPTIONS();
	LUA->CheckType(1, GarrysMod::Lua::Type::Table);

	GET_LUA_TABLE_MEMBER(float, Shininess);
	GET_LUA_TABLE_MEMBER(Vector, Absorption);
	GET_LUA_TABLE_MEMBER(float, RefractionStrength);

	FluidVisualParams params = {};
	params.absorption[0] = Absorption_v.x;
	params.absorption[1] = Absorption_v.y;
	params.absorption[2] = Absorption_v.z;
	params.absorption[3] = Shininess;
	params.refractionStrength = RefractionStrength;

	gelly->SetFluidParams(params);
	CATCH_GELLY_EXCEPTIONS();
	return 0;
}

LUA_FUNCTION(gelly_ChangeParticleRadius) {
	START_GELLY_EXCEPTIONS();
	if (!gelly->IsInteractive()) {
		return 0;
	}

	LUA->CheckType(1, GarrysMod::Lua::Type::Number);

	const auto newRadius = static_cast<float>(LUA->GetNumber(1));
	gelly->ChangeParticleRadius(newRadius);
	CATCH_GELLY_EXCEPTIONS();
	return 0;
}

LUA_FUNCTION(gelly_Reset) {
	START_GELLY_EXCEPTIONS();
	if (!gelly->IsInteractive()) {
		return 0;
	}

	auto *cmdList = gelly->GetSimulation()->CreateCommandList();
	cmdList->AddCommand({RESET, Reset{}});
	gelly->GetSimulation()->ExecuteCommandList(cmdList);
	gelly->GetSimulation()->DestroyCommandList(cmdList);
	CATCH_GELLY_EXCEPTIONS();
	return 0;
}

LUA_FUNCTION(gelly_IsEntityCollidingWithParticles) {
	START_GELLY_EXCEPTIONS();
	if (!gelly->IsInteractive() || !gelly->IsEntityCollisionSupported()) {
		return 0;
	}

	LUA->CheckType(1, GarrysMod::Lua::Type::Number);  // Ent index
	int targetEntIndex = static_cast<int>(LUA->GetNumber(1));
	bool isColliding = false;

	constexpr auto contactVisitor = [&](const XMFLOAT3 &_,
										ObjectHandle handle) {
		const auto entIndex = handleToEntIndexMap[handle];
		if (entIndex == targetEntIndex) {
			isColliding = true;
			return true;
		}

		return false;
	};

	gelly->GetSimulation()->VisitLatestContactPlanes(contactVisitor);

	LUA->PushBool(isColliding);
	CATCH_GELLY_EXCEPTIONS();
	return 1;
}

LUA_FUNCTION(gelly_ChangeThresholdRatio) {
	START_GELLY_EXCEPTIONS();
	LUA->CheckType(1, GarrysMod::Lua::Type::Number);  // Ratio

	gelly->ChangeThresholdRatio(static_cast<float>(LUA->GetNumber(1)));
	CATCH_GELLY_EXCEPTIONS();
	return 0;
}

LUA_FUNCTION(gelly_SetRenderSettings) {
	START_GELLY_EXCEPTIONS();
	LUA->CheckType(1, GarrysMod::Lua::Type::Table);	 // new render settings

	FluidRenderSettings newSettings;
	GET_LUA_TABLE_MEMBER(int, SmoothingIterations);
	GET_LUA_TABLE_MEMBER(int, ThicknessIterations);

	newSettings.filterIterations = static_cast<int>(SmoothingIterations);
	newSettings.thicknessFilterIterations =
		static_cast<int>(ThicknessIterations);

	gelly->SetRenderSettings(newSettings);
	CATCH_GELLY_EXCEPTIONS();
	return 0;
}

typedef HRESULT(WINAPI *SetDepthStencilSurface_t)(
	IDirect3DDevice9Ex *device, IDirect3DSurface9 *surface
);

typedef HRESULT(WINAPI *Clear_t)(
	IDirect3DDevice9Ex *device,
	DWORD Count,
	const D3DRECT *pRects,
	DWORD Flags,
	D3DCOLOR Color,
	float Z,
	DWORD Stencil
);

SetDepthStencilSurface_t originalSetDepthStencilSurface = nullptr;
SetDepthStencilSurface_t hookedSetDepthStencilSurface = nullptr;

Clear_t originalClear = nullptr;
Clear_t hookedClear = nullptr;

static bool captureDepth = false;

HRESULT WINAPI HookedSetDepthStencilSurface(
	IDirect3DDevice9Ex *device, IDirect3DSurface9 *surface
) {
	if (gelly) {
		// Don't allow it as we have our own depth buffer
		return originalSetDepthStencilSurface(
			device, gelly->RetrieveCustomDepthSurface()
		);
	}

	return originalSetDepthStencilSurface(device, surface);
}

HRESULT WINAPI HookedClear(
	IDirect3DDevice9Ex *device,
	DWORD Count,
	const D3DRECT *pRects,
	DWORD Flags,
	D3DCOLOR Color,
	float Z,
	DWORD Stencil
) {
	if (gelly && captureDepth) {
		if (Flags & D3DCLEAR_ZBUFFER) {
			return D3D_OK;
		}
	}
	return originalClear(device, Count, pRects, Flags, Color, Z, Stencil);
}

LUA_FUNCTION(gelly_CaptureDepth) {
	captureDepth = true;
	return 0;
}

LUA_FUNCTION(gelly_DisableCapturingDepth) {
	captureDepth = false;
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

	LOG_INFO("Screen resolution: %dx%d", currentView.width, currentView.height);
	LOG_INFO("Field of view: %f", currentView.fov);
	LOG_INFO("Near clip: %f", currentView.zNear);
	LOG_INFO("Far clip: %f", currentView.zFar);

	auto *d3dDevice = GetD3DDevice();
	LOG_INFO("D3D9Ex device: %p", d3dDevice);

	// If we didnt get an Ex version, bail--there really isnt a point to running
	// 16-bit Gelly. Its awful, theres so many limitations, and it just doesnt
	// work well.
	if (!d3dDevice) {
		LUA->ThrowError(
			"Gelly has detected that the current GMod instance is running in "
			"DirectX 9 mode. Gelly requires Direct3D9Ex to function. Check if "
			"you have any launch options that force DirectX 9 mode--such as "
			"'-nod3d9ex' "
			"Certain optimization configs/mods might also force DirectX 9 "
			"mode. "
		);
	}

	D3DCAPS9 caps = {};
	d3dDevice->GetDeviceCaps(&caps);
	LOG_INFO("D3D9Ex device caps:");
	LOG_INFO("  Max texture width: %lu", caps.MaxTextureWidth);
	LOG_INFO("  Max texture height: %lu", caps.MaxTextureHeight);
	LOG_INFO("  Max texture aspect ratio: %lu", caps.MaxTextureAspectRatio);
	LOG_INFO("  Max texture repeat: %lu", caps.MaxTextureRepeat);
	LOG_INFO("  Texture filter caps: %lu", caps.TextureFilterCaps);

	void **vtable = *reinterpret_cast<void ***>(d3dDevice);
	void *setDepthStencilSurface = vtable[39];
	void *clear = vtable[43];

	LOG_INFO(
		"IDirect3DDevice9Ex::SetDepthStencilSurface: %p", setDepthStencilSurface
	);

	LOG_INFO("IDirect3DDevice9Ex::Clear: %p", clear);

	hookedSetDepthStencilSurface =
		reinterpret_cast<SetDepthStencilSurface_t>(setDepthStencilSurface);

	hookedClear = reinterpret_cast<Clear_t>(clear);

	if (MH_CreateHook(
			setDepthStencilSurface,
			&HookedSetDepthStencilSurface,
			reinterpret_cast<LPVOID *>(&originalSetDepthStencilSurface)
		) != MH_OK) {
		LOG_ERROR("Failed to hook IDirect3DDevice9Ex::SetDepthStencilSurface");
		return 0;
	}

	if (MH_CreateHook(
			clear, &HookedClear, reinterpret_cast<LPVOID *>(&originalClear)
		) != MH_OK) {
		LOG_ERROR("Failed to hook IDirect3DDevice9Ex::Clear");
		return 0;
	}

	MH_EnableHook(setDepthStencilSurface);
	MH_EnableHook(clear);

	gelly =
		new GellyIntegration(currentView.width, currentView.height, d3dDevice);

	SetupWorldRenderingHooks(d3dDevice, gelly->RetrieveCustomDepthSurface());
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
	DEFINE_LUA_FUNC(gelly, Render);
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
	DEFINE_LUA_FUNC(gelly, IsEntityCollidingWithParticles);
	DEFINE_LUA_FUNC(gelly, ChangeThresholdRatio);
	DEFINE_LUA_FUNC(gelly, SetRenderSettings);
	DEFINE_LUA_FUNC(gelly, CaptureDepth);
	DEFINE_LUA_FUNC(gelly, DisableCapturingDepth);
	LUA->SetField(-2, "gelly");
	LUA->Pop();

	return 0;
}

GMOD_MODULE_CLOSE() {
	LOG_INFO("Goodbye, world!");
	MH_RemoveHook(reinterpret_cast<void *>(originalSetDepthStencilSurface));
	MH_RemoveHook(reinterpret_cast<void *>(originalClear));

	TeardownWorldRenderingHooks();

	if (const auto status = MH_Uninitialize(); status != MH_OK) {
		LOG_ERROR(
			"Failed to uninitialize MinHook: %s", MH_StatusToString(status)
		);
		return 0;
	}
	delete gelly;
	gelly = nullptr;
	LOG_INFO("Shut down tracy");
	RemoveConsoleWindow();
	return 0;
}
