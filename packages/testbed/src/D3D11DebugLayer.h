#ifndef D3D11DEBUGLAYER_H
#define D3D11DEBUGLAYER_H

#include <d3d11.h>

#include "ILogger.h"

namespace testbed {
struct DebugLayer {
	ID3D11Debug *debug;
	ID3D11InfoQueue *infoQueue;
};

DebugLayer InitializeRendererDebugLayer(
	ILogger *newLogger, ID3D11Device *device
);
void LogRenderDebugMessages(const DebugLayer &debugLayer);
}  // namespace testbed
#endif	// D3D11DEBUGLAYER_H
