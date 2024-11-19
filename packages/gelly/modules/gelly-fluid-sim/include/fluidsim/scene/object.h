#ifndef OBJECT_H
#define OBJECT_H
#include <cstdint>
#include <memory>

namespace Gelly {
enum class ObjectType : uint8_t { SHAPE, FORCEFIELD };

using ObjectID = uint32_t;

template <typename Metadata>
struct Object {
public:
	ObjectType type;
	std::unique_ptr<Metadata> metadata;
};
}  // namespace Gelly

#endif	// OBJECT_H
