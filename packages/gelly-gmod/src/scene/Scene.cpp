#include "Scene.h"

Scene::Scene(const std::shared_ptr<gelly::simulation::Simulation> &sim) :
	sim(sim), ents(), particles(sim) {}

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

void Scene::UpdateEntityRotation(
	EntIndex entIndex, DirectX::XMFLOAT4 rotation
) {
	ents->UpdateEntityRotation(entIndex, rotation);
}

void Scene::UpdateEntityScale(EntIndex entIndex, Vector scale) {
	ents->UpdateEntityScale(entIndex, scale);
}

void Scene::LoadMap(
	const std::shared_ptr<AssetCache> &assetCache, const std::string &mapPath
) {
	map.emplace(assetCache, sim->GetSolver().GetUnownedScene(), mapPath);
}

void Scene::AddParticles(const ParticleListBuilder &builder) const {
	particles.AddParticles(builder, absorptionModifier);
}

void Scene::ClearParticles() const { particles.ClearParticles(); }

void Scene::Initialize() {
	ents.emplace(sim->GetSolver().GetUnownedScene());
	SetTimeStepMultiplier(DEFAULT_TIMESTEP_MULTIPLIER);
}
