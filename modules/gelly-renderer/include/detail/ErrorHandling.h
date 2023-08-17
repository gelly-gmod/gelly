#ifndef GELLY_ERRORHANDLING_H
#define GELLY_ERRORHANDLING_H

#include <Windows.h>
#include <cstdio>

#define DX(prettyMessage, x) if (FAILED(x)) { \
    printf("D3D11 HRESULT: %lx", x);                                          \
    MessageBox(nullptr, prettyMessage, "Gelly Renderer Fatal Error", MB_OK); \
    exit(1); \
}


#endif //GELLY_ERRORHANDLING_H
