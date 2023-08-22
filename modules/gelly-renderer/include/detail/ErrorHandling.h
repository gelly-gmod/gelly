#ifndef GELLY_ERRORHANDLING_H
#define GELLY_ERRORHANDLING_H

#include <Windows.h>
#include <cstdio>

#define DX(prettyMessage, x) if (FAILED(x)) { \
    char error[256];                          \
    sprintf_s(error, "%s\nHRESULT: 0x%lx", prettyMessage, x); \
    MessageBox(nullptr, error, "Gelly Renderer Fatal Error", MB_OK); \
}


#endif //GELLY_ERRORHANDLING_H
