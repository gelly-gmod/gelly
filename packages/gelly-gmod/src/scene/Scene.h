#ifndef SCENE_H
#define SCENE_H
// clang-format off
#include "Map.h"
// clang-format on

#include <optional>

#include "EntIndex.h"
#include "EntityManager.h"
#include "GarrysMod/Lua/SourceCompat.h"
#include "ParticleManager.h"
#include "logging/global-macros.h"
#include "renderers/splatting/splatting-renderer.h"
#include "v2/simulation.h"

class Scene {
	const float DEFAULT_TIMESTEP_MULTIPLIER = 10.0f;

private:
	std::shared_ptr<gelly::simulation::Simulation> sim;
	std::shared_ptr<gelly::renderer::splatting::AbsorptionModifier>
		absorptionModifier;

	std::optional<EntityManager> ents;
	std::optional<gelly::gmod::Map> map;
	ParticleManager particles;

public:
	Scene(const std::shared_ptr<gelly::simulation::Simulation> &sim);

	Scene(const Scene &) = delete;
	Scene &operator=(const Scene &) = delete;
	Scene(Scene &&) = delete;
	Scene &operator=(Scene &&) = delete;

	~Scene() { LOG_INFO("Scene destructor called") };

	void AddEntity(
		EntIndex entIndex,
		const std::shared_ptr<AssetCache> &cache,
		const char *assetName
	);
	void AddPlayerObject(EntIndex entIndex, float radius, float halfHeight);
	void RemoveEntity(EntIndex entIndex);
	void UpdateEntityPosition(
		EntIndex entIndex, Vector position, size_t boneIndex = 0
	);
	void UpdateEntityRotation(
		EntIndex entIndex, XMFLOAT4 rotation, size_t boneIndex = 0
	);
	void UpdateEntityScale(
		EntIndex entIndex, Vector scale, size_t boneIndex = 0
	);

	void LoadMap(
		const std::shared_ptr<AssetCache> &assetCache,
		const std::string &mapPath
	);

	void AddParticles(const ParticleListBuilder &builder) const;
	void ClearParticles() const;

	[[nodiscard]] ObjectID AddForcefield(
		const ForcefieldCreationInfo &forcefield
	) {
		return sim->GetSolver()
			.GetScene()
			.GetForcefieldHandler()
			->MakeForcefield(forcefield);
	}

	void UpdateForcefieldPosition(ObjectID handle, const Vector &position) {
		sim->GetSolver().GetScene().GetForcefieldHandler()->UpdateForcefield(
			handle,
			[&](ForcefieldObject &object) {
				object.SetPosition(position.x, position.y, position.z);
			}
		);
	}

	void RemoveForcefield(ObjectID handle) {
		sim->GetSolver().GetScene().GetForcefieldHandler()->RemoveForcefield(
			handle
		);
	}

	[[nodiscard]] int GetActiveParticles() const {
		return sim->GetSolver().GetActiveParticleCount();
	}

	[[nodiscard]] int GetMaxParticles() const {
		return sim->GetSolver().GetMaxParticles();
	}

	[[nodiscard]] const char *GetComputeDevice() const {
		return sim->GetComputeDeviceName();
	}

	void BeginTick(float dt) { sim->GetSolver().BeginTick(dt); }
	void EndTick() { sim->GetSolver().EndTick(); }

	void SetTimeStepMultiplier(float timeStepMultiplier) {
		sim->GetSolver().SetTimeStepMultiplier(
			fmaxf(timeStepMultiplier, 0.0001f)
		);
	}

	void UpdateSolver(gelly::simulation::Solver::UpdateSolverInfo &&info) {
		sim->GetSolver().Update(info);
	}

	void SetAbsorptionModifier(
		const std::shared_ptr<gelly::renderer::splatting::AbsorptionModifier>
			&modifier
	) {
		absorptionModifier = modifier;
	}

	void Initialize();
};

#endif	// SCENE_H
