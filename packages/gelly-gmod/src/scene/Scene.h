#ifndef SCENE_H
#define SCENE_H

#include <GellyFluidSim.h>
#include <fluidsim/IFluidSimulation.h>

#include "Config.h"
#include "EntIndex.h"
#include "Entities.h"
#include "GarrysMod/Lua/SourceCompat.h"
#include "Map.h"
#include "Particles.h"
#include "fluidrender/IFluidRenderer.h"

class Scene {
private:
	std::shared_ptr<ISimContext> simContext;
	std::shared_ptr<IFluidSimulation> sim;
	std::shared_ptr<IFluidRenderer> connectedRenderer;

	Entities ents;
	std::optional<Map> map;
	Particles particles;
	Config config;

public:
	Scene(
		const std::shared_ptr<IFluidRenderer> &connectedRenderer,
		int maxParticles,
		ID3D11Device *device,
		ID3D11DeviceContext *deviceContext
	);

	~Scene() = default;

	void AddEntity(EntIndex entIndex, std::vector<Vector> vertices);
	void AddPlayerObject(EntIndex entIndex, float radius, float halfHeight);
	void RemoveEntity(EntIndex entIndex);
	void UpdateEntityPosition(EntIndex entIndex, Vector position);
	void UpdateEntityRotation(EntIndex entIndex, XMFLOAT4 rotation);

	void LoadMap(const std::string &mapPath);

	void AddParticles(const ParticleListBuilder &builder) const;
	void ClearParticles() const;

	void SetFluidProperties(const SetFluidProperties &props) const;
};

#endif	// SCENE_H
