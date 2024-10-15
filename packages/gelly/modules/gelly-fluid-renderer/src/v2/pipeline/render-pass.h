#ifndef RENDER_PASS_H
#define RENDER_PASS_H

#include <d3d11.h>
#include <helpers/comptr.h>

#include <memory>
#include <optional>

#include "device.h"
#include "resources/depth-buffer.h"

namespace gelly {
namespace renderer {

/**
 * Like the Vulkan render pass, this class encapsulates the necessary state
 * setup for a frame. In vulkan you'd typically set up stuff like layout
 * transitions and subpasses, but here we set up depth stencil state, blend
 * state and viewport. The rest is handled by the pipeline which explicitly
 * creates a frame with inputs and outputs (which are then translated into the
 * input assembler, output merger, ps/vs/gs, etc).
 *
 * \note It does not actually configure the immediate
 * command buffer, for that you need to create an actual Pipeline object.
 */
class RenderPass {
public:
	struct DepthStencilState {
		bool depthTestEnabled;
		bool depthWriteEnabled;
		D3D11_COMPARISON_FUNC depthComparisonFunc;
	};

	struct ViewportState {
		float topLeftX;
		float topLeftY;
		float width;
		float height;
		float minDepth;
		float maxDepth;
	};

	struct RasterizerState {
		D3D11_FILL_MODE fillMode;
		D3D11_CULL_MODE cullMode;
	};

	struct BlendState {
		bool independentBlendEnable;
		D3D11_RENDER_TARGET_BLEND_DESC renderTarget[8];
	};

	struct PassInfo {
		const std::shared_ptr<Device> device;
		DepthStencilState depthStencilState;
		ViewportState viewportState;
		RasterizerState rasterizerState;
		std::optional<BlendState> blendState = std::nullopt;
		bool enableMipRegeneration = true;
		/**
		 * This scales all of the output of this render pass by this factor.
		 * So, if you have a 1920x1080 pass and you set this to 0.5, the output
		 * is 960x540.
		 *
		 * It is your responsibility to let the shader know about this scale,
		 * you may retrieve the factor with GetScaleOutput() in the render pass
		 * object.
		 */
		float outputScale = 1.0f;
	};

	RenderPass(const PassInfo &passInfo);
	~RenderPass() = default;

	/**
	 * sets up all the state for this render pass
	 */
	auto Apply() -> void;

	auto IsMipRegenerationEnabled() const -> bool {
		return passInfo.enableMipRegeneration;
	}

	auto GetOutputScale() const -> float { return passInfo.outputScale; }

	auto GetScaledWidth() const -> float {
		return floorf(passInfo.viewportState.width * passInfo.outputScale);
	}

	auto GetScaledHeight() const -> float {
		return floorf(passInfo.viewportState.height * passInfo.outputScale);
	}

	// For some use-cases, we might want to disable mip regeneration or enable
	// it on the fly
	auto SetMipRegenerationEnabled(bool enabled) -> void {
		passInfo.enableMipRegeneration = enabled;
	}

private:
	PassInfo passInfo;
	ComPtr<ID3D11DepthStencilState> depthStencilState;
	ComPtr<ID3D11RasterizerState> rasterizerState;
	ComPtr<ID3D11BlendState> blendState;

	auto CreateDepthStencilState() -> ComPtr<ID3D11DepthStencilState>;
	auto CreateRasterizerState() -> ComPtr<ID3D11RasterizerState>;
	auto CreateBlendState() -> ComPtr<ID3D11BlendState>;
	auto CreateViewport() -> D3D11_VIEWPORT;
};

}  // namespace renderer
}  // namespace gelly

#endif	// RENDER_PASS_H
