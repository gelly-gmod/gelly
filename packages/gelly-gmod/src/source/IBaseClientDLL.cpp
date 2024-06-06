#include "IBaseClientDLL.h"

#include "Interface.h"
#include "logging/global-macros.h"

static IBaseClientDLL *clientDLL = nullptr;

static void EnsureClientDLL() {
	if (!clientDLL) {
		LOG_INFO("client.dll has not been initialized, loading...");
		clientDLL =
			GetInterface<IBaseClientDLL>("client.dll", CLIENT_DLL_VERSION);
		LOG_INFO("Loaded IBaseClientDLL at %p", clientDLL);
	}
}

void GetClientViewSetup(CViewSetup &viewSetup) {
	EnsureClientDLL();
	clientDLL->GetPlayerView(viewSetup);
}
