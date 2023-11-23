#ifndef GELLY_D3D11_ERRORHANDLING_H
#define GELLY_D3D11_ERRORHANDLING_H

#include <Windows.h>

#include <cstdio>
#include <stdexcept>

#define DX(prettyMessage, x)                                      \
	if (FAILED(x)) {                                              \
		char error[256];                                          \
		sprintf_s(error, "%s\nHRESULT: 0x%lx", prettyMessage, x); \
		throw std::runtime_error(error);                          \
	}

#endif	// GELLY_D3D11_ERRORHANDLING_H
