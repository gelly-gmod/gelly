#ifndef GELLY_IRENDERCONTEXT_H
#define GELLY_IRENDERCONTEXT_H

#include <GellyInterface.h>

// Distinction between handles and resources since this context will abstract
// away rendering resources.
enum class RenderAPIResource {
	D3D11Device,
	D3D11DeviceContext,
};

gelly_interface IRenderContext {
public:
	// All subclasses must destroy their resources here, but also in the event
	// of a device reset.
	virtual ~IRenderContext() = 0;

	virtual void *GetRenderAPIResource(RenderAPIResource resource) = 0;
};

#endif	// GELLY_IRENDERCONTEXT_H
