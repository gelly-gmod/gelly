#include <GMFS.h>
#include <GarrysMod/Lua/Interface.h>

#include <stdexcept>
#include <thread>

#include "Gelly.h"
#include "TextureOverride.h"
#include "source/IRenderView.h"

#define IMAGE_FORMAT_RGBA16161616F 24
#define TEXTUREFLAGS_RENDERTARGET 32768

#define SET_C_FUNC(parent, name)         \
	LUA->PushCFunction(parent##_##name); \
	LUA->SetField(-2, #name);

#define LUA_RUNTIME_ERROR_START() try {
#define LUA_RUNTIME_ERROR_END()           \
	}                                     \
	catch (const std::runtime_error &e) { \
		LUA->ThrowError(e.what());        \
	}

#define CREATE_SOURCE_TEXTURE(texType)                       \
	LUA_RUNTIME_ERROR_START()                                \
	TextureOverride::Enable(TextureOverrideTarget::texType); \
	LUA_RUNTIME_ERROR_END()                                  \
	LUA->PushSpecial(SPECIAL_GLOB);                          \
	LUA->GetField(-1, "GetRenderTargetEx");                  \
	LUA->PushString("__Gelly_" #texType "RT");               \
	LUA->PushNumber(params.width);                           \
	LUA->PushNumber(params.height);                          \
	LUA->PushNumber(0);                                      \
	LUA->PushNumber(0);                                      \
	LUA->PushNumber(TEXTUREFLAGS_RENDERTARGET);              \
	LUA->PushNumber(0);                                      \
	LUA->PushNumber(IMAGE_FORMAT_RGBA16161616F);             \
	LUA->Call(8, 1);                                         \
	LUA->Remove(-2);                                         \
	LUA_RUNTIME_ERROR_START()                                \
	TextureOverride::Disable();                              \
	LUA_RUNTIME_ERROR_END()

using namespace GarrysMod::Lua;
static int Gelly_id = 0;
static Gelly *gellyInstance = nullptr;
static IVRenderView_SceneBeginFn originalSceneBegin = nullptr;

void __fastcall IVRenderView_SceneBeginHook(
	IVRenderView *thisptr,
	void *edx,
	void *worldLists,
	unsigned long flags,
	float waterZAdjust
) {
	printf("DrawWorld\n");
	originalSceneBegin(thisptr, edx, worldLists, flags, waterZAdjust);

	if (gellyInstance) {
		gellyInstance->Render();
		gellyInstance->compositor.Composite();
	}
}

LUA_FUNCTION(Gelly_gc) {
	LUA->CheckType(1, Gelly_id);
	auto *gelly = *LUA->GetUserType<Gelly *>(1, Gelly_id);

	delete gelly;

	return 0;
}

LUA_FUNCTION(Gelly_SetDebugZValue) {
	LUA->CheckType(1, Gelly_id);
	LUA->CheckType(2, Type::Number);
	auto *gelly = *LUA->GetUserType<Gelly *>(1, Gelly_id);

	gelly->compositor.debugConstants.zValue =
		static_cast<float>(LUA->GetNumber(2));

	return 0;
}

LUA_FUNCTION(Gelly_LoadMap) {
	LUA->CheckType(1, Gelly_id);
	LUA->CheckType(2, Type::String);
	auto *gelly = *LUA->GetUserType<Gelly *>(1, Gelly_id);

	GellyMessage loadMapMessage{};
	loadMapMessage.type = GellyMessage::LoadMap;
	loadMapMessage.loadMap.mapName = LUA->GetString(2);
	gelly->SendGellyMessage(loadMapMessage);

	return 0;
}

LUA_FUNCTION(Gelly_Clear) {
	LUA->CheckType(1, Gelly_id);
	auto *gelly = *LUA->GetUserType<Gelly *>(1, Gelly_id);

	GellyMessage clearMessage{};
	clearMessage.type = GellyMessage::Clear;
	gelly->SendGellyMessage(clearMessage);

	return 0;
}

LUA_FUNCTION(Gelly_Composite) {
	LUA->CheckType(1, Gelly_id);
	auto *gelly = *LUA->GetUserType<Gelly *>(1, Gelly_id);

	gelly->compositor.Composite();

	return 0;
}

LUA_FUNCTION(Gelly_Update) {
	LUA->CheckType(1, Gelly_id);
	LUA->CheckType(2, Type::Number);
	auto *gelly = *LUA->GetUserType<Gelly *>(1, Gelly_id);

	GellyMessage updateMessage{};
	updateMessage.type = GellyMessage::Update;
	updateMessage.update.deltaTime = static_cast<float>(LUA->GetNumber(2));
	gelly->SendGellyMessage(updateMessage);

	return 0;
}

LUA_FUNCTION(Gelly_AddParticle) {
	LUA->CheckType(1, Gelly_id);
	LUA->CheckType(2, Type::Vector);
	LUA->CheckType(3, Type::Vector);

	auto *gelly = *LUA->GetUserType<Gelly *>(1, Gelly_id);
	auto position = LUA->GetVector(2);
	auto velocity = LUA->GetVector(3);

	GellyMessage addParticleMessage{};
	addParticleMessage.type = GellyMessage::AddParticle;
	addParticleMessage.addParticle.position = {
		position.x, position.y, position.z, 1.f};
	addParticleMessage.addParticle.velocity = {
		velocity.x, velocity.y, velocity.z};
	gelly->SendGellyMessage(addParticleMessage);

	return 0;
}

LUA_FUNCTION(Gelly_SetupCamera) {
	LUA->CheckType(1, Gelly_id);
	LUA->CheckType(2, Type::Number);
	LUA->CheckType(3, Type::Number);
	LUA->CheckType(4, Type::Number);
	LUA->CheckType(5, Type::Number);
	LUA->CheckType(6, Type::Number);

	auto *gelly = *LUA->GetUserType<Gelly *>(1, Gelly_id);

	GellyMessage setupCameraMessage{};
	setupCameraMessage.type = GellyMessage::SetupCamera;
	setupCameraMessage.setupCamera.fov = static_cast<float>(LUA->GetNumber(2));
	setupCameraMessage.setupCamera.width =
		static_cast<float>(LUA->GetNumber(3));
	setupCameraMessage.setupCamera.height =
		static_cast<float>(LUA->GetNumber(4));
	setupCameraMessage.setupCamera.nearZ =
		static_cast<float>(LUA->GetNumber(5));
	setupCameraMessage.setupCamera.farZ = static_cast<float>(LUA->GetNumber(6));

	gelly->SendGellyMessage(setupCameraMessage);

	return 0;
}

LUA_FUNCTION(Gelly_SyncCamera) {
	LUA->CheckType(1, Gelly_id);
	LUA->CheckType(2, Type::Vector);
	LUA->CheckType(3, Type::Vector);

	auto *gelly = *LUA->GetUserType<Gelly *>(1, Gelly_id);
	auto position = LUA->GetVector(2);
	auto direction = LUA->GetVector(3);

	GellyMessage syncCameraMessage{};
	syncCameraMessage.type = GellyMessage::SyncCamera;
	syncCameraMessage.syncCamera.position = {
		position.x, position.y, position.z};
	syncCameraMessage.syncCamera.direction = {
		direction.x, direction.y, direction.z};
	gelly->SendGellyMessage(syncCameraMessage);

	return 0;
}

LUA_FUNCTION(Gelly_Render) {
	LUA->CheckType(1, Gelly_id);
	auto *gelly = *LUA->GetUserType<Gelly *>(1, Gelly_id);

	GellyMessage renderMessage{};
	renderMessage.type = GellyMessage::Render;
	gelly->SendGellyMessage(renderMessage);

	return 0;
}

LUA_FUNCTION(gelly_Create) {
	LUA->CheckType(1, Type::Number);  // maxParticles
	LUA->CheckType(2, Type::Number);  // maxColliders
	LUA->CheckType(3, Type::Number);  // width
	LUA->CheckType(4, Type::Number);  // height

	GellyInitParams params{};
	params.maxParticles = static_cast<int>(LUA->GetNumber(1));
	params.maxColliders = static_cast<int>(LUA->GetNumber(2));
	params.width = static_cast<int>(LUA->GetNumber(3));
	params.height = static_cast<int>(LUA->GetNumber(4));

	// We're going to manually create the textures and apply our override

	CREATE_SOURCE_TEXTURE(Normal);
	CREATE_SOURCE_TEXTURE(Depth);

	params.sharedTextures = TextureOverride::sharedTextures;

	if (params.sharedTextures.normal == nullptr ||
		params.sharedTextures.depth == nullptr) {
		LUA->ThrowError("Failed to create shared textures");
	}

	if (!TextureOverride::device) {
		LUA->ThrowError("Failed to create device");
	}

	// Create a Gelly
	auto *gelly = new Gelly(params, TextureOverride::device);
	LUA->PushUserType_Value(gelly, Gelly_id);

	gellyInstance = gelly;

	//	try {
	//		HookSceneBegin(IVRenderView_SceneBeginHook, &originalSceneBegin);
	//	} catch (const std::runtime_error &e) {
	//		LUA->ThrowError(e.what());
	//	}

	return 3;
}

GMOD_MODULE_OPEN() {
	AllocConsole();
	freopen("CONOUT$", "w", stdout);

	try {
		TextureOverride::Initialize();
		FileSystem::LoadFileSystem();
	} catch (const std::runtime_error &e) {
		LUA->ThrowError(e.what());
	}

	LUA->PushSpecial(SPECIAL_GLOB);
	LUA->CreateTable();
	SET_C_FUNC(gelly, Create);
	LUA->SetField(-2, "gelly");

	Gelly_id = LUA->CreateMetaTable("Gelly");
	LUA->Push(-1);
	LUA->SetField(-2, "__index");
	LUA->PushCFunction(Gelly_gc);
	LUA->SetField(-2, "__gc");
	SET_C_FUNC(Gelly, LoadMap);
	SET_C_FUNC(Gelly, Composite);
	SET_C_FUNC(Gelly, Update);
	SET_C_FUNC(Gelly, Render);
	SET_C_FUNC(Gelly, AddParticle);
	SET_C_FUNC(Gelly, SetupCamera);
	SET_C_FUNC(Gelly, SyncCamera);
	SET_C_FUNC(Gelly, Clear);
	SET_C_FUNC(Gelly, SetDebugZValue);
	LUA->Pop();

	return 0;
}

GMOD_MODULE_CLOSE() {
	TextureOverride::Shutdown();
	return 0;
}