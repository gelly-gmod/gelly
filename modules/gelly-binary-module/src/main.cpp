#include <GarrysMod/Lua/Interface.h>
#include <stdexcept>
#include <thread>
#include "TextureOverride.h"
#include "Gelly.h"

static std::thread gelly_thread;
static void gelly_threadmain(GellyInitParams params) {
    Gelly gelly(params);
    while (true) {
//        gelly.Update(1.0f / 60.0f);
        gelly.Render();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

#define IMAGE_FORMAT_RGBA16161616F 24
#define TEXTUREFLAGS_RENDERTARGET 32768

#define SET_C_FUNC(parent, name) \
    LUA->PushCFunction(parent##_##name); \
    LUA->SetField(-2, #name);

#define CREATE_SOURCE_TEXTURE(texType) \
    TextureOverride::Enable(TextureOverrideTarget::texType); \
    LUA->PushSpecial(SPECIAL_GLOB); \
    LUA->GetField(-1, "GetRenderTargetEx"); \
    LUA->PushString("__Gelly_" #texType "RT"); \
    LUA->PushNumber(params.width); \
    LUA->PushNumber(params.height); \
    LUA->PushNumber(0); \
    LUA->PushNumber(0); \
    LUA->PushNumber(TEXTUREFLAGS_RENDERTARGET); \
    LUA->PushNumber(0); \
    LUA->PushNumber(IMAGE_FORMAT_RGBA16161616F); \
    LUA->Call(8, 1); \
    LUA->Remove(-2); \
    TextureOverride::Disable();

using namespace GarrysMod::Lua;
static int Gelly_id = 0;

LUA_FUNCTION(Gelly_gc) {
    LUA->CheckType(1, Gelly_id);
    auto* gelly = *LUA->GetUserType<Gelly*>(1, Gelly_id);

    delete gelly;
    return 0;
}

LUA_FUNCTION(Gelly_LoadMap) {
    LUA->CheckType(1, Gelly_id);
    LUA->CheckType(2, Type::String);
    auto* gelly = *LUA->GetUserType<Gelly*>(1, Gelly_id);
    gelly->LoadMap(LUA->GetString(2));
    return 0;
}

LUA_FUNCTION(Gelly_Update) {
    LUA->CheckType(1, Gelly_id);
    LUA->CheckType(2, Type::Number);
    auto* gelly = *LUA->GetUserType<Gelly*>(1, Gelly_id);
    gelly->Update(static_cast<float>(LUA->GetNumber(2)));
    return 0;
}

LUA_FUNCTION(Gelly_Render) {
    LUA->CheckType(1, Gelly_id);
    auto* gelly = *LUA->GetUserType<Gelly*>(1, Gelly_id);
    gelly->Render();
    return 0;
}

LUA_FUNCTION(gelly_Create) {
    LUA->CheckType(1, Type::Number); // maxParticles
    LUA->CheckType(2, Type::Number); // maxColliders
    LUA->CheckType(3, Type::Number); // width
    LUA->CheckType(4, Type::Number); // height

    GellyInitParams params{};
    params.maxParticles = static_cast<int>(LUA->GetNumber(1));
    params.maxColliders = static_cast<int>(LUA->GetNumber(2));
    params.width = static_cast<int>(LUA->GetNumber(3));
    params.height = static_cast<int>(LUA->GetNumber(4));

    // We're going to manually create the textures and apply our override

    CREATE_SOURCE_TEXTURE(Normal);

    params.sharedTextures = TextureOverride::sharedTextures;
    gelly_thread = std::thread(gelly_threadmain, params);
    gelly_thread.detach();

    // Give the caller the RTs
    return 1;
}

GMOD_MODULE_OPEN() {
    try {
        TextureOverride::Initialize();
    } catch (const std::runtime_error& e) {
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
    SET_C_FUNC(Gelly, Update);
    SET_C_FUNC(Gelly, Render);
    LUA->Pop();

    return 0;
}

GMOD_MODULE_CLOSE() {
    TextureOverride::Shutdown();
    return 0;
}