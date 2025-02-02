#include "scene.h"

using namespace gelly::simulation;

Scene::Scene(ObjectHandlerContext ctx) : ctx(ctx) {
	sharedCounter = std::make_shared<MonotonicCounter>();
	shapeHandler = std::make_shared<ShapeHandler>(ctx, sharedCounter);
	forcefieldHandler = std::make_shared<ForcefieldHandler>(ctx, sharedCounter);

	handlers.push_back(shapeHandler);
	handlers.push_back(forcefieldHandler);
}

Scene::Scene() : ctx({}) {}

std::shared_ptr<ShapeHandler> Scene::GetShapeHandler() const {
	return shapeHandler;
}

std::shared_ptr<ForcefieldHandler> Scene::GetForcefieldHandler() const {
	return forcefieldHandler;
}

void Scene::Update() {
	for (const auto &handler : handlers) {
		handler->Update();
	}
}