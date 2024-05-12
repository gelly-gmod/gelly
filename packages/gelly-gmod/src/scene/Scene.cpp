#include "Scene.h"

Scene::Scene(
	const std::shared_ptr<IFluidRenderer> &connectedRenderer,
	int maxParticles,
	ID3D11Device *device,
	ID3D11DeviceContext *context
)
	: simContext(CreateD3D11SimContext(device, context)),
	  sim(CreateD3D11FlexFluidSimulation(simContext.get())),
	  connectedRenderer(connectedRenderer),
	  ents(sim->GetScene()),
	  particles(connectedRenderer, sim),
	  config(sim) {
	// link the simulation to the renderer
	sim->SetMaxParticles(maxParticles);
	connectedRenderer->SetSimData(sim->GetSimulationData());
	sim->Initialize();
}

void Scene::AddEntity(EntIndex entIndex, std::vector<Vector> vertices) {
	ents.AddEntity(entIndex, vertices);
}

void Scene::AddPlayerObject(EntIndex entIndex, float radius, float halfHeight) {
	ents.AddPlayerObject(entIndex, radius, halfHeight);
}

void Scene::RemoveEntity(EntIndex entIndex) { ents.RemoveEntity(entIndex); }

void Scene::UpdateEntityPosition(EntIndex entIndex, Vector position) {
	ents.UpdateEntityPosition(entIndex, position);
}

void Scene::UpdateEntityRotation(EntIndex entIndex, XMFLOAT4 rotation) {
	ents.UpdateEntityRotation(entIndex, rotation);
}

void Scene::LoadMap(const std::string &mapPath) {
	map = Map(sim->GetScene(), mapPath);
}

void Scene::AddParticles(const ParticleListBuilder &builder) const {
	particles.AddParticles(builder);
}

void Scene::ClearParticles() const { particles.ClearParticles(); }

void Scene::SetFluidProperties(const ::SetFluidProperties &props) const {
	config.SetFluidProperties(props);
}
