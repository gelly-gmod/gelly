#ifndef GELLY_TEST_ERRORHANDLING_H
#define GELLY_TEST_ERRORHANDLING_H

#include <windows.h>
#include <cstdio>

#define DX(prettyMessage, x) {HRESULT hr = x; if (FAILED(hr)) { \
    printf("D3D9 HRESULT: %x\n", hr);          \
    LPTSTR errorText = nullptr;               \
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&errorText, 0, nullptr); \
    if (errorText != NULL) {                  \
        printf("errorText: %s\n", errorText); \
        LocalFree(errorText); \
    }                                          \
    MessageBox(nullptr, prettyMessage, "Gelly Test D3D9 Fatal Error", MB_OK); \
    exit(1); }\
}

#endif //GELLY_TEST_ERRORHANDLING_H
