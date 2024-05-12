#include "Config.h"

Config::Config(const std::shared_ptr<IFluidSimulation> &sim) : sim(sim) {}

void Config::SetFluidProperties(const ::SetFluidProperties &props) const {
	auto *cmdList = sim->CreateCommandList();
	cmdList->AddCommand(SimCommand{SET_FLUID_PROPERTIES, props});
	sim->ExecuteCommandList(cmdList);
	sim->DestroyCommandList(cmdList);
}
