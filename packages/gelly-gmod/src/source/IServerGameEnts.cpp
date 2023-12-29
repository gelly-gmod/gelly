#include "IServerGameEnts.h"

#include <stdexcept>

#include "Interface.h"

static IServerGameEnts* serverGameEnts = nullptr;

void EnsureServerGameEntsFound() {
	if (serverGameEnts == nullptr) {
		serverGameEnts = GetInterface<IServerGameEnts>("server.dll", INTERFACEVERSION_SERVERGAMEENTS);
		if (serverGameEnts == nullptr) {
			throw std::runtime_error("Failed to find IServerGameEnts interface");
		}
	}
}

IServerGameEnts* GetServerGameEnts() {
	EnsureServerGameEntsFound();
	return serverGameEnts;
}