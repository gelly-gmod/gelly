#ifndef IPERFMARKER_H
#define IPERFMARKER_H
#include "GellyInterface.h"

// High-level interface for performance markers, modelled pretty much 1:1 with
// the D3D11 interface, although the internals of that is abstracted away
// because the D3D11 perf markers have a lot of boilerplate code that isn't
// necessary for the high-level interface.
gelly_interface IPerfMarker {
public:
	virtual ~IPerfMarker() = default;

	virtual void BeginEvent(const char *name) = 0;
	virtual void EndEvent() = 0;
};

#endif	// IPERFMARKER_H
