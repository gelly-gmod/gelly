#include "Scene.h"

Scene::Scene(ID3D11Device *device, ID3D11DeviceContext *context)
	: simContext(CreateD3D11SimContext(device, context)),
	  sim(CreateD3D11FlexFluidSimulation(simContext.get())),
	  ents(Entities(sim->GetScene())) {}

void Scene::AddEntity(EntIndex entIndex, std::vector<Vector> vertices) {
	ents.AddEntity(entIndex, vertices);
}

void Scene::AddPlayerObject(EntIndex entIndex, float radius, float halfHeight) {
	ents.AddPlayerObject(entIndex, radius, halfHeight);
}

void Scene::RemoveEntity(EntIndex entIndex) { ents.RemoveEntity(entIndex); }

void Scene::UpdateEntityPosition(EntIndex entIndex, Vector position) {
	ents.UpdateEntityPosition(entIndex, position);
}

void Scene::UpdateEntityRotation(EntIndex entIndex, XMFLOAT4 rotation) {
	ents.UpdateEntityRotation(entIndex, rotation);
}
