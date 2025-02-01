#ifndef OBJECT_H
#define OBJECT_H
#include <cstdint>
#include <memory>

namespace Gelly {
using ObjectID = uint32_t;

constexpr ObjectID WORLD_ID = 0;
constexpr ObjectID INVALID_ID = 0xFFFFFFFF;

}  // namespace Gelly

#endif	// OBJECT_H
