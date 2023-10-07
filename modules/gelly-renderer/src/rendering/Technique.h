#ifndef GELLY_TECHNIQUE_H
#define GELLY_TECHNIQUE_H

#include <GellyD3D.h>
#include <d3d11.h>
#include <wrl.h>

#include "detail/Camera.h"
#include "detail/GBuffer.h"
#include "techniques/PerFrameCBuffer.h"

using namespace Microsoft::WRL;

/**
 * This struct contains resources that are shared between all techniques.
 */
struct TechniqueResources {
	d3d11::ConstantBuffer<PerFrameCBuffer> *perFrameCB;
	Camera *camera;
	GBuffer *gbuffer;
	ComPtr<ID3D11DepthStencilView> dsv;
};

/**
 * A technique is a collection of shaders and other resources that are used to
 * render the fluid.
 * @note Techniques are sequential, so the order in which they are added to the
 * renderer matters. Certain techniques may rely on the output of other ones.
 */
class Technique {
public:
	virtual ~Technique() = default;

	/**
	 * Runs the technique for a single frame. You can expect this method to
	 * flush the pipeline or make a draw call.
	 * @param context
	 * @param rts
	 */
	virtual void RunForFrame(
		ID3D11DeviceContext *context, TechniqueResources *resources
	) = 0;
};

#endif	// GELLY_TECHNIQUE_H
