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

	std::thread managerThread;

	/**
	 * \brief Manages the simulation and rendering, also initializes them. It
	 * also handles communication between the main thread and itself.
	 */
	void ManagerThread();
public:
	GellyIntegration();
	~GellyIntegration();

	void Render();
	void Simulate(float dt);
};

#endif //GELLY_H
