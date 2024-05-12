#ifndef PARTICLES_H
#define PARTICLES_H
#include <memory>
#include <vector>

#include "GarrysMod/Lua/SourceCompat.h"
#include "fluidrender/IFluidRenderer.h"
#include "fluidsim/IFluidSimulation.h"
#include "fluidsim/ISimCommandList.h"

class ParticleListBuilder {
	friend class Particles;

protected:
	std::vector<AddParticle> particles;
	float absorption[3];
	bool absorptionSet = false;

public:
	ParticleListBuilder();
	~ParticleListBuilder() = default;

	ParticleListBuilder AddParticle(
		const Vector &position, const Vector &velocity
	);

	ParticleListBuilder SetAbsorption(float r, float g, float b);
};

class Particles {
private:
	std::shared_ptr<IFluidRenderer> renderer;
	std::shared_ptr<IFluidSimulation> sim;

	[[nodiscard]] ISimCommandList *CreateCommandListFromBuilder(
		const ParticleListBuilder &builder
	) const;

	void SubmitPerParticleAbsorption(const ParticleListBuilder &builder) const;

	void PullAbsorptionData(const ParticleListBuilder &builder) const;
	void PushAbsorptionData(const ParticleListBuilder &builder) const;

public:
	Particles(
		const std::shared_ptr<IFluidRenderer> &renderer,
		const std::shared_ptr<IFluidSimulation> &sim
	);
	~Particles() = default;

	static ParticleListBuilder CreateParticleList();
	void AddParticles(const ParticleListBuilder &builder) const;
	void ClearParticles() const;
};

#endif	// PARTICLES_H
