#ifndef GELLY_TECHNIQUE_H
#define GELLY_TECHNIQUE_H

#include <d3d11.h>
#include <wrl.h>

using namespace Microsoft::WRL;

/**
 * This struct contains various render targets that can be used as outputs AND
 * inputs for a technique.
 */
struct TechniqueRTs {
	ComPtr<ID3D11RenderTargetView> depth;
	ComPtr<ID3D11RenderTargetView> normal;
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
		ID3D11DeviceContext *context, TechniqueRTs *rts, const Camera &camera
	) = 0;
};

#endif	// GELLY_TECHNIQUE_H
