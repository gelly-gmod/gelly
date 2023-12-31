#include "fluidsim/CFlexSimRigids.h"

#include <NvFlexExt.h>

#include <stdexcept>

static ISimRigids::RigidHandle g_nextHandle = 0;

CFlexSimRigids::CFlexSimRigids(int maxParticles, NvFlexLibrary *library, NvFlexSolver *solver) :
	m_rigidAssets(),
	m_rigids(),
	m_container(nullptr) {
	m_container = NvFlexExtCreateContainer(library, solver, maxParticles);
}

CFlexSimRigids::~CFlexSimRigids() {
	for (auto& asset : m_rigidAssets) {
		NvFlexExtDestroyAsset(asset.second);
	}
	NvFlexExtDestroyContainer(m_container);
}

void CFlexSimRigids::AddRigidModel(RigidModelName name, cref<RigidModelCreationParams> params) {
	NvFlexExtAsset* asset = NvFlexExtCreateRigidFromMesh(
		reinterpret_cast<const float*>(params.vertices.get()),
		params.numVertices,
		params.indices.get(),
		params.numIndices,
		params.radius,
		params.expansion
	);

	m_rigidAssets[name] = asset;
}

ISimRigids::RigidHandle CFlexSimRigids::CreateRigid(RigidModelName name) {
	const auto it = m_rigidAssets.find(name);
	if (it == m_rigidAssets.end()) {
		throw std::runtime_error("Could not find the requested rigid model");
	}

	const auto& asset = it->second;
	XMFLOAT4X4 identity = {};
	XMStoreFloat4x4(&identity, XMMatrixIdentity());

	// transpose to column major
	XMStoreFloat4x4(&identity, XMMatrixTranspose(XMLoadFloat4x4(&identity)));

	NvFlexExtParticleData mappedParticles = NvFlexExtMapParticleData(m_container);
	NvFlexExtInstance* instance = NvFlexExtCreateInstance(
		m_container,
		&mappedParticles,
		asset,
		reinterpret_cast<const float*>(identity.m),
		0.f,
		0.f,
		0.f,
		NvFlexMakePhase(0, eNvFlexPhaseSelfCollide),
		1.f
	);

	InternalRigidData data = {};
	data.instance = instance;
	data.position = XMFLOAT3(0.f, 0.f, 0.f);
	data.rotation = XMFLOAT4(0.f, 0.f, 0.f, 1.f);

	const RigidHandle handle = g_nextHandle++;
	m_rigids[handle] = data;

	return handle;
}

void CFlexSimRigids::DestroyRigid(RigidHandle handle) {
	const auto it = m_rigids.find(handle);
	if (it == m_rigids.end()) {
		throw std::runtime_error("Could not find the requested rigid");
	}

	const auto& data = it->second;
	NvFlexExtDestroyInstance(m_container, data.instance);
	m_rigids.erase(it);
}

void CFlexSimRigids::Update() {
	NvFlexExtPushToDevice(m_container);
}

void CFlexSimRigids::ComputeNewPositions() {
	NvFlexExtPullFromDevice(m_container);
	RecomputeRigidTransforms();
}