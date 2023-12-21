#ifndef CSIMPLESIMCOMMANDLIST_H
#define CSIMPLESIMCOMMANDLIST_H

#include "ISimCommandList.h"

/**
 * \brief A vector-backed implementation of ISimCommandList.
 * \note Customizable by the simulation, specify which commands are supported in
 * the constructor.
 */
class CSimpleSimCommandList : public ISimCommandList {
private:
	SimCommandType supportedCommands;
	CommandVec commands;

public:
	CSimpleSimCommandList(SimCommandType supportedCommands);
	~CSimpleSimCommandList() override = default;

	void AddCommand(const SimCommand &command) override;
	void Reserve(size_t commandCount) override;
	void ClearCommands() override;

	std::pair<CommandVecIterator, CommandVecIterator> GetCommands() override;
};

#endif	// CSIMPLESIMCOMMANDLIST_H
