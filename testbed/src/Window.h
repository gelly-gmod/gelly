#ifndef GELLY_WINDOW_H
#define GELLY_WINDOW_H

#include <windows.h>

namespace testbed {
extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;

void MakeTestbedWindow();
bool HandleWindowMessages();
HWND GetTestbedWindowHandle();
}  // namespace testbed

#endif	// GELLY_WINDOW_H
