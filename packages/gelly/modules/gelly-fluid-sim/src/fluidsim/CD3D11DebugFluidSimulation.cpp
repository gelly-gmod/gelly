#include "fluidsim/CD3D11DebugFluidSimulation.h"

#include <stdexcept>

CD3D11DebugFluidSimulation::CD3D11DebugFluidSimulation()
	: simData(new CD3D11CPUSimData()),
	  positionBuffer(nullptr),
	  maxParticles(0) {}

CD3D11DebugFluidSimulation::~CD3D11DebugFluidSimulation() {
	delete simData;

	if (positionBuffer != nullptr) {
		positionBuffer->Release();
	}
}

void CD3D11DebugFluidSimulation::CreateBuffers() {
	D3D11_BUFFER_DESC positionBufferDesc = {};
	positionBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	positionBufferDesc.ByteWidth = sizeof(SimFloat4) * maxParticles;
	positionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	positionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	auto *device = static_cast<ID3D11Device *>(
		context->GetAPIHandle(SimContextHandle::D3D11_DEVICE)
	);

	if (const auto result =
			device->CreateBuffer(&positionBufferDesc, nullptr, &positionBuffer);
		FAILED(result)) {
		throw std::runtime_error(
			"CD3D11DebugFluidSimulation::CreateBuffers: Failed to create "
			"position buffer."
		);
	}
}

static float t = 0.0f;
void CD3D11DebugFluidSimulation::GenerateRandomParticles() {
	auto *deviceContext = static_cast<ID3D11DeviceContext *>(
		context->GetAPIHandle(SimContextHandle::D3D11_DEVICE_CONTEXT)
	);

	D3D11_MAPPED_SUBRESOURCE mappedSubresource = {};
	if (const auto result = deviceContext->Map(
			positionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource
		);
		FAILED(result)) {
		throw std::runtime_error(
			"CD3D11DebugFluidSimulation::GenerateRandomParticles: Failed to "
			"map the position buffer."
		);
	}

	auto *positionData = static_cast<SimFloat4 *>(mappedSubresource.pData);
	t += 0.01f;
	for (int i = 0; i < maxParticles; i++) {
		// A simple cube
		positionData[i].x = (rand() % 1000) / 1000.0f;
		positionData[i].y = (rand() % 1000) / 1000.0f;
		positionData[i].z = (rand() % 1000) / 1000.0f;
		positionData[i].w = 1.0f;
	}

	deviceContext->Unmap(positionBuffer, 0);

	// Copy to the specified buffer in the simulation data.
	if (!simData->IsBufferLinked(SimBufferType::POSITION)) {
		throw std::runtime_error(
			"CD3D11DebugFluidSimulation::GenerateRandomParticles: Position "
			"buffer is not linked."
		);
	}

	auto *linkedBuffer = static_cast<ID3D11Buffer *>(
		simData->GetLinkedBuffer(SimBufferType::POSITION)
	);

	ID3D11Resource *positionResource = nullptr;
	if (const auto result = positionBuffer->QueryInterface(
			__uuidof(ID3D11Resource),
			reinterpret_cast<void **>(&positionResource)
		);
		FAILED(result)) {
		throw std::runtime_error(
			"CD3D11DebugFluidSimulation::GenerateRandomParticles: Failed to "
			"get position buffer resource."
		);
	}

	ID3D11Resource *linkedResource = nullptr;
	if (const auto result = linkedBuffer->QueryInterface(
			__uuidof(ID3D11Resource), reinterpret_cast<void **>(&linkedResource)
		);
		FAILED(result)) {
		throw std::runtime_error(
			"CD3D11DebugFluidSimulation::GenerateRandomParticles: Failed to "
			"get linked buffer resource."
		);
	}

	deviceContext->CopyResource(linkedResource, positionResource);
	deviceContext->Flush();	 // send off the copy command
}

void CD3D11DebugFluidSimulation::SetMaxParticles(int maxParticles) {
	if (maxParticles <= 0) {
		throw std::invalid_argument(
			"CD3D11DebugFluidSimulation::SetMaxParticles: maxParticles must be "
			"greater than 0."
		);
	}

	this->maxParticles = maxParticles;
	simData->SetMaxParticles(maxParticles);
	simData->SetActiveParticles(maxParticles);
}

void CD3D11DebugFluidSimulation::Initialize() {
	CreateBuffers();
	GenerateRandomParticles();
}

ISimData *CD3D11DebugFluidSimulation::GetSimulationData() { return simData; }
ISimScene *CD3D11DebugFluidSimulation::GetScene() { return nullptr; }

SimContextAPI CD3D11DebugFluidSimulation::GetComputeAPI() {
	return SimContextAPI::D3D11;
}

ISimCommandList *CD3D11DebugFluidSimulation::CreateCommandList() {
	auto *commandList = new CSimpleSimCommandList(supportedCommands);
	commandLists.push_back(commandList);
	return commandList;
}

void CD3D11DebugFluidSimulation::DestroyCommandList(ISimCommandList *commandList
) {
	// delete on nullptr is a no-op so it's fine to call this after the
	// simulation is destroyed.
	delete commandList;
}

void CD3D11DebugFluidSimulation::ExecuteCommandList(ISimCommandList *commandList
) {
	if (commandList == nullptr) {
		throw std::invalid_argument(
			"CD3D11DebugFluidSimulation::ExecuteCommandList: commandList must "
			"not be null."
		);
	}

	const auto iterators = commandList->GetCommands();

	for (auto it = iterators.first; it != iterators.second; ++it) {
		auto &command = *it;
		std::visit(
			[&](auto &&arg) {
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, Reset>) {
					GenerateRandomParticles();
				} else {
					throw std::runtime_error(
						"CD3D11DebugFluidSimulation::ExecuteCommandList: "
						"Unsupported command type."
					);
				}
			},
			command.data
		);
	}
}

void CD3D11DebugFluidSimulation::AttachToContext(
	const GellyObserverPtr<ISimContext> context
) {
	this->context = context;
}

void CD3D11DebugFluidSimulation::Update(const float deltaTime) {
	// Do nothing.
}

const char *CD3D11DebugFluidSimulation::GetComputeDeviceName() {
	return "CPU";
}

// Aside from the inherent base featureset, this simulation has nothing.
bool CD3D11DebugFluidSimulation::CheckFeatureSupport(const GELLY_FEATURE feature) {
	return false;
}