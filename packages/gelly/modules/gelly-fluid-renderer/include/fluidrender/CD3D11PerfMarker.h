#ifndef CD3D11PERFMARKER_H
#define CD3D11PERFMARKER_H

#include <d3d11_1.h>
#include <wrl.h>

#include "GellyInterfaceRef.h"
#include "IPerfMarker.h"
#include "IRenderContext.h"

using namespace Microsoft::WRL;

class CD3D11PerfMarker : public IPerfMarker {
private:
	ComPtr<ID3DUserDefinedAnnotation> perfMarker;

public:
	~CD3D11PerfMarker() override = default;
	CD3D11PerfMarker(GellyInterfaceVal<IRenderContext> context);

	void BeginEvent(const char *name) override;
	void EndEvent() override;
};

#endif	// CD3D11PERFMARKER_H
