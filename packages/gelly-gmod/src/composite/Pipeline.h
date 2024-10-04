#ifndef PIPELINE_H
#define PIPELINE_H

#include "GellyResources.h"
#include "PipelineConfig.h"
#include "UnownedResources.h"

struct PipelineFluidMaterial {
	// The current material system is physically based for the most part,
	// with the trade-off of physically inaccurate lobe shapes for faster
	// and simpler evaluation on the GPU. For example, roughness is implemented
	// as a linear interpolation of the diffuse and delta specular lobes.

	float roughness = 0.f;
	// should be either zero or one, no in-between
	float specularTransmission = 0.f;
	float refractiveIndex = 1.5f;
	float padding = 0.f;

	float diffuseColor[3] = {0.8f, 0.8f, 0.8f};
	float padding1 = 0.f;
};

static_assert(
	sizeof(PipelineFluidMaterial) % 16 == 0,
	"PipelineFluidMaterial must be 16-byte aligned"
);

/**
 * A D3D9 pipeline interface responsible for creating a state block to
 * draw the Gelly composite.
 *
 * It is heavily encouraged to use ComPtr for managing resources,
 * this is one of the most mission-critical parts of the gelly-gmod
 * module and it's important to leave the game's memory cleaner than
 * when it was found.
 *
 * At any given time, the game may restart and reload the module, so
 * any left over hooks or resources will cause a crash. Therefore, it's
 * important that any subclass of this interface cleans up after itself.
 *
 * Unless the subclass is doing hooking, the destructor should be default.
 */
class Pipeline {
public:
	virtual ~Pipeline() = default;

	/**
	 * Called once it's appropiate for the pipeline to create its local D3D9
	 * resources.
	 *
	 * Some examples of what to do here: create a recovery state block,
	 * create a vertex buffer for rendering a full screen quad, load
	 * shaders, etc.
	 *
	 * Throw an exception if the resources cannot be created.
	 *
	 * @throws std::runtime_error Thrown if the resources cannot be created
	 */
	virtual gelly::renderer::splatting::InputSharedHandles
	CreatePipelineLocalResources(
		const GellyResources &gelly, const UnownedResources &gmod
	) = 0;

	virtual void UpdateGellyResources(const GellyResources &newResources) = 0;

	virtual void SetConfig(const PipelineConfig &config) = 0;
	[[nodiscard]] virtual PipelineConfig GetConfig() const = 0;

	virtual void SetFluidMaterial(const PipelineFluidMaterial &material) = 0;

	/**
	 * Called during opaque rendering to composite and shade the Gelly frame.
	 */
	virtual void Composite() = 0;

	/**
	 * Called when the pipeline should render the Gelly frame.
	 */
	virtual void Render() = 0;

#ifdef GELLY_ENABLE_RENDERDOC_CAPTURES
	/**
	 * Called when the pipeline should reload all shaders.
	 */
	virtual void ReloadAllShaders() = 0;
#endif
};

#endif	// PIPELINE_H
