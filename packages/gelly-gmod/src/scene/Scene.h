#ifndef SCENE_H
#define SCENE_H
// clang-format off
#include "Map.h"
// clang-format on

#include <GellyFluidSim.h>
#include <fluidsim/IFluidSimulation.h>

#include "Config.h"
#include "EntIndex.h"
#include "EntityManager.h"
#include "GarrysMod/Lua/SourceCompat.h"
#include "ParticleManager.h"
#include "logging/global-macros.h"
#include "renderers/splatting/splatting-renderer.h"

class Scene {
	const float DEFAULT_TIMESTEP_MULTIPLIER = 10.0f;

private:
	std::shared_ptr<ISimContext> simContext;
	std::shared_ptr<IFluidSimulation> sim;
	std::shared_ptr<gelly::renderer::splatting::AbsorptionModifier>
		absorptionModifier;

	std::optional<EntityManager> ents;
	std::optional<Map> map;
	ParticleManager particles;
	Config config;

public:
	Scene(
		const std::shared_ptr<ISimContext> &simContext,
		const std::shared_ptr<IFluidSimulation> &sim,
		int maxParticles
	);

	Scene(const Scene &) = delete;
	Scene &operator=(const Scene &) = delete;
	Scene(Scene &&) = delete;
	Scene &operator=(Scene &&) = delete;

	~Scene(){LOG_INFO("Scene destructor called")};

	void AddEntity(
		EntIndex entIndex,
		const std::shared_ptr<AssetCache> &cache,
		const char *assetName
	);
	void AddPlayerObject(EntIndex entIndex, float radius, float halfHeight);
	void RemoveEntity(EntIndex entIndex);
	void UpdateEntityPosition(EntIndex entIndex, Vector position);
	void UpdateEntityRotation(EntIndex entIndex, XMFLOAT4 rotation);

	void LoadMap(
		const std::shared_ptr<AssetCache> &assetCache,
		const std::string &mapPath
	);

	void AddParticles(const ParticleListBuilder &builder) const;
	void ClearParticles() const;

	void SetFluidProperties(const SetFluidProperties &props) const;
	void ChangeRadius(float radius) const;

	[[nodiscard]] ObjectHandle AddForcefield(
		const ObjectCreationParams &forcefield
	) {
		return sim->GetScene()->CreateObject(forcefield);
	}

	void UpdateForcefieldPosition(ObjectHandle handle, const Vector &position) {
		sim->GetScene()->SetObjectPosition(
			handle, position.x, position.y, position.z
		);
	}

	void RemoveForcefield(ObjectHandle handle) {
		sim->GetScene()->RemoveObject(handle);
	}

	[[nodiscard]] int GetActiveParticles() const {
		return sim->GetSimulationData()->GetActiveParticles();
	}

	[[nodiscard]] int GetMaxParticles() const {
		return sim->GetSimulationData()->GetMaxParticles();
	}

	[[nodiscard]] const char *GetComputeDevice() const {
		return sim->GetComputeDeviceName();
	}

	void Simulate(float dt) { sim->Update(dt); }

	void SetTimeStepMultiplier(float timeStepMultiplier) {
		sim->SetTimeStepMultiplier(fmaxf(timeStepMultiplier, 0.0001f));
	}

	[[nodiscard]] GellyInterfaceVal<ISimData> GetSimData() const {
		return sim->GetSimulationData();
	}

	void Configure(::Configure &&config) {
		const auto commandList = sim->CreateCommandList();
		commandList->AddCommand({CONFIGURE, config});
		sim->ExecuteCommandList(commandList);
		sim->DestroyCommandList(commandList);
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
