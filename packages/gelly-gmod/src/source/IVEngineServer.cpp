#include "IVEngineServer.h"
#include "Interface.h"

static IVEngineServer* engineServer = nullptr;

void EnsureEngineServerFound() {
	if (engineServer == nullptr) {
		engineServer = GetInterface<IVEngineServer>("engine.dll", INTERFACEVERSION_VENGINESERVER);
	}
}

IVEngineServer* GetEngineServer() {
	EnsureEngineServerFound();
	return engineServer;
}