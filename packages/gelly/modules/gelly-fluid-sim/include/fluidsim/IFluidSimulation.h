#ifndef GELLY_IFLUIDSIMULATION_H
#define GELLY_IFLUIDSIMULATION_H

#include "GellyInterface.h"
#include "GellyObserverPtr.h"
#include "ISimCommandList.h"
#include "ISimContext.h"
#include "ISimData.h"
#include "ISimScene.h"

gelly_interface IFluidSimulation {
public:
	virtual ~IFluidSimulation() = default;

	virtual void SetMaxParticles(int maxParticles) = 0;
	/**
	 * \brief Signals to the underlying simulation that it should initialize.
	 * This is called for you by the simulation context, usually. However, if
	 * a buffer resource or something similar gets destroyed due to some error,
	 * this should be called again to reinitialize the simulation.
	 * \note This is not a way to clear particles, it will completely
	 * reinitialize the simulation to default values.
	 */
	virtual void Initialize() = 0;

	virtual ISimData *GetSimulationData() = 0;
	/**
	 * \brief Returns the virtual scene where the particles are contained.
	 * \note If the returned pointer is null, the simulation is not attached to
	 * any scene and thus does not support collision.
	 * \return Pointer to the scene object.
	 */
	virtual ISimScene *GetScene() = 0;
	virtual SimContextAPI GetComputeAPI() = 0;
	virtual void AttachToContext(GellyObserverPtr<ISimContext> context) = 0;

	/**
	 * \brief Creates a new command list for the simulation.
	 * \note This pointer is owned by the simulation, and will
	 * point to nothing after the simulation is destroyed.
	 * \return A new empty command list.
	 */
	virtual ISimCommandList *CreateCommandList() = 0;
	/**
	 * \brief Recommended, but each command list will be destroyed when the
	 * simulation is destroyed.
	 * \param commandList The command list to destroy.
	 */
	virtual void DestroyCommandList(ISimCommandList * commandList) = 0;
	virtual void ExecuteCommandList(ISimCommandList * commandList) = 0;

	/**
	 * \brief Will throw if the simulation data buffers are not linked.
	 * \param deltaTime Time since last update in seconds.
	 */
	virtual void Update(float deltaTime) = 0;

	virtual const char* GetComputeDeviceName() = 0;
};

#endif	// GELLY_IFLUIDSIMULATION_H
