#include <GarrysMod/Lua/Interface.h>
#include "LoggingMacros.h"
#include <cstdio>
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <GMFS.h>
#include <MinHook.h>
#include <Windows.h>

#include "Gelly.h"
#include "source/D3DDeviceWrapper.h"
#include "source/IBaseClientDLL.h"

#define DEFINE_LUA_FUNC(namespace, name) \
	LUA->PushCFunction(namespace##_##name); \
	LUA->SetField(-2, #name);

static GellyIntegration *gelly = nullptr;

void InjectConsoleWindow() {
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
}

LUA_FUNCTION(gelly_Render) {
	gelly->Render();
	return 0;
}

LUA_FUNCTION(gelly_Simulate) {
	LUA->CheckType(1, GarrysMod::Lua::Type::Number); // Delta time
	auto dt = static_cast<float>(LUA->GetNumber(1));

	gelly->Simulate(dt);
	return 0;
}

LUA_FUNCTION(gelly_LoadMap) {
	LUA->CheckType(1, GarrysMod::Lua::Type::String); // Map name
	gelly->LoadMap(LUA->GetString(1));
	return 0;
}

LUA_FUNCTION(gelly_AddObject) {
	// The lua side will pass through the triangle mesh
	LUA->CheckType(1, GarrysMod::Lua::Type::Table); // Mesh

	uint32_t vertexCount = LUA->ObjLen(1);
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
	uint32_t* indices = new uint32_t[vertexCount];

	for (uint32_t i = 0; i < vertexCount; i+=3) {
		indices[i + 2] = i;
		indices[i + 1] = i + 1;
		indices[i] = i + 2;
	}

	ObjectCreationParams params = {};
	params.shape = ObjectShape::TRIANGLE_MESH;

	ObjectCreationParams::TriangleMesh meshParams = {};
	meshParams.vertices = reinterpret_cast<const float*>(vertices);
	meshParams.vertexCount = vertexCount;
	meshParams.indices32 = indices;
	meshParams.indexCount = vertexCount;
	meshParams.indexType = ObjectCreationParams::TriangleMesh::IndexType::UINT32;

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

	return 1;
}

LUA_FUNCTION(gelly_RemoveObject) {
	LUA->CheckType(1, GarrysMod::Lua::Type::Number); // Handle
	gelly->GetSimulation()->GetScene()->RemoveObject(static_cast<ObjectHandle>(LUA->GetNumber(1)));
	return 0;
}

LUA_FUNCTION(gelly_SetObjectPosition) {
	LUA->CheckType(1, GarrysMod::Lua::Type::Number); // Handle
	LUA->CheckType(2, GarrysMod::Lua::Type::Vector); // Position

	gelly->GetSimulation()->GetScene()->SetObjectPosition(
		static_cast<ObjectHandle>(LUA->GetNumber(1)),
		LUA->GetVector(2).x,
		LUA->GetVector(2).y,
		LUA->GetVector(2).z
	);
	return 0;
}

LUA_FUNCTION(gelly_SetObjectRotation) {
	LUA->CheckType(1, GarrysMod::Lua::Type::Number); // Handle
	LUA->CheckType(2, GarrysMod::Lua::Type::Angle); // Rotation
	// we need to convert it from an angle to a quaternion
	QAngle angle = LUA->GetAngle(2);

	XMVECTOR quat = XMQuaternionRotationRollPitchYaw(
		// We need to convert to radians, but also re-order since Source uses a right handed Z-offset euler angle system
		XMConvertToRadians(-angle.x),
		XMConvertToRadians(angle.z),
		XMConvertToRadians(angle.y)
	);

	gelly->GetSimulation()->GetScene()->SetObjectQuaternion(
		static_cast<ObjectHandle>(LUA->GetNumber(1)),
		XMVectorGetX(quat),
		XMVectorGetY(quat),
		XMVectorGetZ(quat),
		XMVectorGetW(quat)
	);

	return 0;
}

LUA_FUNCTION(gelly_AddParticles) {
	LUA->CheckType(1, GarrysMod::Lua::Type::Table); // Particles

	const uint32_t particleCount = LUA->ObjLen(1);
	auto* cmdList = gelly->GetSimulation()->CreateCommandList();

	for (uint32_t i = 0; i < particleCount; i+=2) {
		LUA->PushNumber(i + 1);
		LUA->GetTable(-2);
		const auto position = LUA->GetVector(-1);
		LUA->PushNumber(i + 2);
		LUA->GetTable(-3);
		const auto velocity = LUA->GetVector(-1);

		cmdList->AddCommand({ADD_PARTICLE, AddParticle{position.x, position.y, position.z, velocity.x, velocity.y, velocity.z}});

		LUA->Pop(3);
	}

	gelly->GetSimulation()->ExecuteCommandList(cmdList);
	gelly->GetSimulation()->DestroyCommandList(cmdList);
	return 0;
}

LUA_FUNCTION(gelly_GetStatus) {
	// Current status table:
	//	- ComputeDeviceName: string
	//	- ActiveParticles: number
	//	- MaxParticles: number

	LUA->CreateTable();
	LUA->PushString(gelly->GetComputeDeviceName());
	LUA->SetField(-2, "ComputeDeviceName");
	LUA->PushNumber(gelly->GetSimulation()->GetSimulationData()->GetActiveParticles());
	LUA->SetField(-2, "ActiveParticles");
	LUA->PushNumber(gelly->GetSimulation()->GetSimulationData()->GetMaxParticles());
	LUA->SetField(-2, "MaxParticles");

	return 1;
}

#define GET_LUA_TABLE_NUMBER(name) \
	LUA->GetField(-1, #name); \
	name = static_cast<float>(LUA->GetNumber(-1)); \
	LUA->Pop();

#define GET_LUA_TABLE_VECTOR(name) \
	LUA->GetField(-1, #name); \
	name##_v = LUA->GetVector(-1); \
	LUA->Pop();

#define GET_LUA_TABLE_MEMBER(type, name) \
	float name; \
	Vector name##_v; \
	if constexpr (std::is_same_v<type, Vector>) { \
		GET_LUA_TABLE_VECTOR(name); \
	} else { \
		GET_LUA_TABLE_NUMBER(name); \
	}

LUA_FUNCTION(gelly_SetFluidProperties) {
	LUA->CheckType(1, GarrysMod::Lua::Type::Table);

	GET_LUA_TABLE_MEMBER(float, Viscosity);
	GET_LUA_TABLE_MEMBER(float, Cohesion);
	GET_LUA_TABLE_MEMBER(float, SurfaceTension);
	GET_LUA_TABLE_MEMBER(float, VorticityConfinement);
	GET_LUA_TABLE_MEMBER(float, Adhesion);

	SetFluidProperties props = {};
	props.viscosity = Viscosity;
	props.cohesion = Cohesion;
	props.surfaceTension = SurfaceTension;
	props.vorticityConfinement = VorticityConfinement;
	props.adhesion = Adhesion;

	auto* cmdList = gelly->GetSimulation()->CreateCommandList();
	cmdList->AddCommand({SET_FLUID_PROPERTIES, props});
	gelly->GetSimulation()->ExecuteCommandList(cmdList);
	gelly->GetSimulation()->DestroyCommandList(cmdList);

	return 0;
}

LUA_FUNCTION(gelly_SetFluidVisualParams) {
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

	return 0;
}

GMOD_MODULE_OPEN() {
	InjectConsoleWindow();
	if (const auto status = FileSystem::LoadFileSystem(); status != FILESYSTEM_STATUS::OK) {
		LOG_ERROR("Failed to load file system: %d", status);
		return 0;
	}

	if (const auto status = MH_Initialize(); status != MH_OK) {
		LOG_ERROR("Failed to initialize MinHook: %s", MH_StatusToString(status));
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

	auto* d3dDevice = GetD3DDevice();
	LOG_INFO("D3D9Ex device: %p", d3dDevice);

	D3DCAPS9 caps = {};
	d3dDevice->GetDeviceCaps(&caps);
	LOG_INFO("D3D9Ex device caps:");
	LOG_INFO("  Max texture width: %lu", caps.MaxTextureWidth);
	LOG_INFO("  Max texture height: %lu", caps.MaxTextureHeight);
	LOG_INFO("  Max texture aspect ratio: %lu", caps.MaxTextureAspectRatio);
	LOG_INFO("  Max texture repeat: %lu", caps.MaxTextureRepeat);
	LOG_INFO("  Texture filter caps: %lu", caps.TextureFilterCaps);

	gelly = new GellyIntegration(currentView.width, currentView.height, d3dDevice);

	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
	LUA->CreateTable();
	DEFINE_LUA_FUNC(gelly, Render);
	DEFINE_LUA_FUNC(gelly, Simulate);
	DEFINE_LUA_FUNC(gelly, GetStatus);
	DEFINE_LUA_FUNC(gelly, AddParticles);
	DEFINE_LUA_FUNC(gelly, LoadMap);
	DEFINE_LUA_FUNC(gelly, AddObject);
	DEFINE_LUA_FUNC(gelly, RemoveObject);
	DEFINE_LUA_FUNC(gelly, SetObjectPosition);
	DEFINE_LUA_FUNC(gelly, SetObjectRotation);
	DEFINE_LUA_FUNC(gelly, SetFluidProperties);
	DEFINE_LUA_FUNC(gelly, SetFluidVisualParams);
	LUA->SetField(-2, "gelly");
	LUA->Pop();

	return 0;
}

GMOD_MODULE_CLOSE() {
	LOG_INFO("Goodbye, world!");
	if (const auto status = MH_Uninitialize(); status != MH_OK) {
		LOG_ERROR("Failed to uninitialize MinHook: %s", MH_StatusToString(status));
		return 0;
	}
	delete gelly;
	return 0;
}
