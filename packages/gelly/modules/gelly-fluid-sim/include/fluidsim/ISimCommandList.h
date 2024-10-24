#ifndef ISIMCOMMANDLIST_H
#define ISIMCOMMANDLIST_H

#include <GellyInterface.h>

#include <variant>
#include <vector>

namespace Gelly {
namespace SimCommands {
struct AddParticle {
	float x, y, z;
	float vx, vy, vz;
};

struct ChangeRadius {
	float radius;
};

struct SetFluidProperties {
	float viscosity;
	float cohesion;
	float surfaceTension;
	float vorticityConfinement;
	float adhesion;
	float dynamicFriction;
	/**
	 * Ratio of the rest distance to the radius of the particles. (typically
	 * 0.73)
	 */
	float restDistanceRatio = 0.73f;
};

struct SetDiffuseProperties {
	float ballisticCount = 0.f;
	float kineticThreshold = 0.f;
	float buoyancy = 0.f;
	float drag = 0.f;
	float lifetime = 0.f;
};

struct Reset {};

struct Configure {
	int substeps;
	int iterations;
	float relaxationFactor = 1.f;
	float collisionDistance = 0.6f;
	float gravity = -4.f;
};

enum SimCommandType {
	ADD_PARTICLE = 0b000001,
	CHANGE_RADIUS = 0b000010,
	RESET = 0b000100,
	SET_FLUID_PROPERTIES = 0b001000,
	CONFIGURE = 0b010000,
	SET_DIFFUSE_PROPERTIES = 0b100000,
};

struct SimCommand {
	SimCommandType type;
	std::variant<
		AddParticle,
		Reset,
		ChangeRadius,
		SetFluidProperties,
		SetDiffuseProperties,
		Configure>
		data;
};
}  // namespace SimCommands
}  // namespace Gelly

using namespace Gelly::SimCommands;

/**
 * This interface represents a list of commands that can be executed on the
 * simulation fast. This is used for making things such as particle emitters,
 * particle attractors, and other things that need to be updated every frame.
 * \note It's recommended to keep a single command list for each type of
 * action you want to perform rather than to make one every frame.
 */
gelly_interface ISimCommandList {
public:
	using CommandVec = std::vector<SimCommand>;
	using CommandVecIterator = CommandVec::const_iterator;

	virtual ~ISimCommandList() = default;

	/**
	 * \brief Adds a command to the list.
	 * \note Depending on the implementation, this may or may not throw a
	 * runtime error exception if the command is supported or not.
	 * \param command Command to add.
	 */
	virtual void AddCommand(const SimCommand &command) = 0;
	virtual void Reserve(size_t commandCount) = 0;
	virtual void ClearCommands() = 0;

	/**
	 * \brief Allows the user to iterate over the commands in the list.
	 * \return Returns a pair of iterators, respectively the beginning and end
	 */
	virtual std::pair<CommandVecIterator, CommandVecIterator> GetCommands() = 0;
};

#endif	// ISIMCOMMANDLIST_H
