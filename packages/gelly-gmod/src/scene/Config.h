#ifndef CONFIG_H
#define CONFIG_H
#include <memory>

#include "fluidsim/IFluidSimulation.h"

class Config {
private:
	std::shared_ptr<IFluidSimulation> sim;

public:
	Config(const std::shared_ptr<IFluidSimulation> &sim);
	~Config() = default;

	void SetFluidProperties(const SetFluidProperties &props) const;
	void ChangeRadius(float radius) const;
};

#endif	// CONFIG_H
