#ifndef SCENE_H
#define SCENE_H

#include <GellyFluidSim.h>
#include <fluidsim/IFluidSimulation.h>

#include "EntIndex.h"
#include "Entities.h"
#include "GarrysMod/Lua/SourceCompat.h"

class Scene {
private:
	std::shared_ptr<ISimContext> simContext;
	std::shared_ptr<IFluidSimulation> sim;

	Entities ents;

public:
	Scene(ID3D11Device *device, ID3D11DeviceContext *deviceContext);
	~Scene() = default;

	void AddEntity(EntIndex entIndex, std::vector<Vector> vertices);
	void AddPlayerObject(EntIndex entIndex, float radius, float halfHeight);
	void RemoveEntity(EntIndex entIndex);
	void UpdateEntityPosition(EntIndex entIndex, Vector position);
	void UpdateEntityRotation(EntIndex entIndex, XMFLOAT4 rotation);
};

#endif	// SCENE_H
