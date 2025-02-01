#ifndef GELLY_SCENE_H
#define GELLY_SCENE_H
#include <memory>

#include "handlers/forcefield-handler.h"
#include "handlers/shape-handler.h"
#include "object-handler.h"

namespace gelly::simulation {
class Scene {
public:
	explicit Scene(ObjectHandlerContext ctx);
	Scene();
	~Scene() = default;

	[[nodiscard]] std::shared_ptr<ShapeHandler> GetShapeHandler() const;
	[[nodiscard]] std::shared_ptr<ForcefieldHandler> GetForcefieldHandler(
	) const;

	void Update();

private:
	using HandlerPtr = std::shared_ptr<ObjectHandler>;

	ObjectHandlerContext ctx{};
	std::shared_ptr<MonotonicCounter> sharedCounter;

	std::shared_ptr<ShapeHandler> shapeHandler;
	std::shared_ptr<ForcefieldHandler> forcefieldHandler;

	std::vector<HandlerPtr> handlers;
};
}  // namespace gelly::simulation

#endif	// GELLY_SCENE_H
