#include "CSimpleSimCommandList.h"

#include "ISimCommandList.h"

CSimpleSimCommandList::CSimpleSimCommandList(SimCommandType supportedCommands)
	: supportedCommands(supportedCommands) {}

void CSimpleSimCommandList::AddCommand(const SimCommand &command) {
	if (command.type & supportedCommands) {
		commands.push_back(command);
	}
}

void CSimpleSimCommandList::Reserve(size_t commandCount) {
	commands.reserve(commandCount);
}

void CSimpleSimCommandList::ClearCommands() { commands.clear(); }

std::pair<
	ISimCommandList::CommandVecIterator,
	ISimCommandList::CommandVecIterator>
CSimpleSimCommandList::GetCommands() {
	return std::make_pair(commands.cbegin(), commands.cend());
}