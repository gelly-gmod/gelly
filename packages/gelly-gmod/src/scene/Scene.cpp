#include "Scene.h"

Scene::Scene(
	const std::shared_ptr<IFluidRenderer> &connectedRenderer,
	const std::shared_ptr<ISimContext> &simContext,
	const std::shared_ptr<IFluidSimulation> &sim,
	int maxParticles
)
	: simContext(simContext),
	  sim(sim),
	  connectedRenderer(connectedRenderer),
	  ents(),
	  particles(connectedRenderer, sim),
	  config(sim) {
	// link the simulation to the renderer
	sim->SetMaxParticles(maxParticles);
	connectedRenderer->SetSimData(sim->GetSimulationData());
	sim->Initialize();

	ents.emplace(sim->GetScene());
}

void Scene::AddEntity(EntIndex entIndex, std::vector<Vector> vertices) {
	ents->AddEntity(entIndex, vertices);
}

void Scene::AddPlayerObject(EntIndex entIndex, float radius, float halfHeight) {
	ents->AddPlayerObject(entIndex, radius, halfHeight);
}

void Scene::RemoveEntity(EntIndex entIndex) { ents->RemoveEntity(entIndex); }

void Scene::UpdateEntityPosition(EntIndex entIndex, Vector position) {
	ents->UpdateEntityPosition(entIndex, position);
}

void Scene::UpdateEntityRotation(EntIndex entIndex, XMFLOAT4 rotation) {
	ents->UpdateEntityRotation(entIndex, rotation);
}

void Scene::LoadMap(const std::string &mapPath) {
	map.emplace(sim->GetScene(), mapPath);
}

void Scene::AddParticles(const ParticleListBuilder &builder) const {
	particles.AddParticles(builder);
}

void Scene::ClearParticles() const { particles.ClearParticles(); }

void Scene::SetFluidProperties(const ::SetFluidProperties &props) const {
	config.SetFluidProperties(props);
}

void Scene::ChangeRadius(float radius) const { config.ChangeRadius(radius); }