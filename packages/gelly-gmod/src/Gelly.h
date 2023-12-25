#ifndef GELLY_H
#define GELLY_H

#include <GellyFluidRender.h>
#include <GellyFluidSim.h>

#include <memory>
#include <thread>

/**
 * \brief Holds together the fluid simulation and rendering, and provides an
 * interface to them as they are in another thread.
 */
class GellyIntegration {
private:
	IFluidRenderer *renderer;
	IFluidSimulation *simulation;
	IRenderContext *renderContext;
	ISimContext *simContext;
public:
	GellyIntegration(uint16_t width, uint16_t height);
	~GellyIntegration();

	void Render();
	void Simulate(float dt);
};

#endif //GELLY_H
