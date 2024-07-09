#include "Scene.h"

Scene::Scene(
	const std::shared_ptr<ISimContext> &simContext,
	const std::shared_ptr<IFluidSimulation> &sim,
	int maxParticles
) :
	simContext(simContext), sim(sim), ents(), particles(sim), config(sim) {
	sim->SetMaxParticles(maxParticles);
}

void Scene::AddEntity(
	EntIndex entIndex,
	const std::shared_ptr<AssetCache> &cache,
	const char *assetName
) {
	ents->AddEntity(entIndex, cache, assetName);
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
	particles.AddParticles(builder, absorptionModifier);
}

void Scene::ClearParticles() const { particles.ClearParticles(); }

void Scene::SetFluidProperties(const ::SetFluidProperties &props) const {
	config.SetFluidProperties(props);
}

void Scene::ChangeRadius(float radius) const { config.ChangeRadius(radius); }

void Scene::Initialize() {
	sim->Initialize();
	ents.emplace(sim->GetScene());

	SetTimeStepMultiplier(DEFAULT_TIMESTEP_MULTIPLIER);
}
