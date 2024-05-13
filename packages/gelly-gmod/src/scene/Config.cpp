#include "Config.h"

Config::Config(const std::shared_ptr<IFluidSimulation> &sim) : sim(sim) {}

void Config::SetFluidProperties(const ::SetFluidProperties &props) const {
	auto *cmdList = sim->CreateCommandList();
	cmdList->AddCommand(SimCommand{SET_FLUID_PROPERTIES, props});
	sim->ExecuteCommandList(cmdList);
	sim->DestroyCommandList(cmdList);
}

void Config::ChangeRadius(float radius) const {
	auto *cmdList = sim->CreateCommandList();
	cmdList->AddCommand(SimCommand{CHANGE_RADIUS, ::ChangeRadius{radius}});
	sim->ExecuteCommandList(cmdList);
	sim->DestroyCommandList(cmdList);
}
