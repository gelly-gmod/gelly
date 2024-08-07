#ifndef GELLY_SCENE_H
#define GELLY_SCENE_H

#define NOMINMAX
#include "ILogger.h"
#include "Memory.h"
#include "fluidsim/IFluidSimulation.h"

/**
 * Since testbed is not a game engine, most of our stuff is static, and the
 * "world" of the game is analogous to the "scene" of the testbed. This is where
 * we load completely static world data from a gltf file.
 */

namespace testbed {
struct SceneMetadata {
	const char *filepath{};
	/**
	 * Auto-generated when loaded.
	 */
	int triangles = 0;
};

void InitializeSceneSystem(ILogger *newLogger);
void LoadScene(const SceneMetadata &metadata);
/**
 * Pushes a render list to the renderer, all of the actual render data
 * construction is done as soon as LoadScene is called.
 *
 * @note Call during a frame! This does not start/end a frame.
 */
void RenderScene();
SceneMetadata GetCurrentSceneMetadata();

/**
 * \brief Allows for the fluid simulation to "see" and collide particles with
 * the currently loaded scene.
 * \param sim The simulation to register the scene to.
 */
void RegisterSceneToGellySim(IFluidSimulation *sim);
void UnregisterSceneFromGellySim(IFluidSimulation *sim);
void UpdateGellySimScene();

}  // namespace testbed

#endif	// GELLY_SCENE_H
