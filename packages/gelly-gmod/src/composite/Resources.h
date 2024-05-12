#ifndef RESOURCES_H
#define RESOURCES_H

#include "UnownedResources.h"

/**
 * Allows access to GMod's resources
 */
class Resources {
public:
	Resources() = default;
	~Resources() = default;

	static UnownedResources FindGModResources();
};

#endif	// RESOURCES_H
