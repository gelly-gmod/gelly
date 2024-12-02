#ifndef HANDLER_H
#define HANDLER_H

#include <GellyInterface.h>

#include "NvFlex.h"

namespace Gelly {
struct ObjectHandlerContext {
	NvFlexLibrary *lib;
	NvFlexSolver *solver;
};
}  // namespace Gelly

using namespace Gelly;
gelly_interface ObjectHandler {
public:
	virtual ~ObjectHandler() = default;
	virtual void Update() = 0;
};

#endif	// HANDLER_H
