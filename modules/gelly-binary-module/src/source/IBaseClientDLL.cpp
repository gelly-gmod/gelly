#include "IBaseClientDLL.h"

#include "Interface.h"

/**
 * lazy initialization of clientDLL
 */
static IBaseClientDLL *clientDLL = nullptr;

static void EnsureClientDLL() {
	if (!clientDLL) {
		clientDLL =
			GetInterface<IBaseClientDLL>("client.dll", CLIENT_DLL_VERSION);
	}
}

void GetClientViewSetup(CViewSetup &viewSetup) {
	EnsureClientDLL();
	clientDLL->GetPlayerView(viewSetup);
}