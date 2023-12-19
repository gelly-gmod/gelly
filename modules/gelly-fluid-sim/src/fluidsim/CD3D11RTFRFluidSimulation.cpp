// This fluid sim isn't actually doing anything, sort of like the debug fluid
// sim. All its doing is outputting the particle positions from frame files.

#include "fluidsim/CD3D11RTFRFluidSimulation.h"

#include <stdexcept>

CD3D11RTFRFluidSimulation::CD3D11RTFRFluidSimulation()
	: simData(new CD3D11CPUSimData()),
	  positionBuffer(nullptr),
	  maxParticles(0),
	  activeParticles(0),
	  datasetInfo({}),
	  dataset({}),
	  currentFrameIndex(0) {}

CD3D11RTFRFluidSimulation::~CD3D11RTFRFluidSimulation() {
	delete simData;

	if (positionBuffer != nullptr) {
		positionBuffer->Release();
	}
}

void CD3D11RTFRFluidSimulation::CreateBuffers() {
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
			"CD3D11RTFRFluidSimulation::CreateBuffers: Failed "
			"to create "
			"position buffer."
		);
	}
}

void CD3D11RTFRFluidSimulation::LoadFrameIntoBuffers() {
	if (!dataset.IsLoaded()) {
		// nothing to load! but its also not necessarily an error
		return;
	}
	simData->SetActiveParticles(dataset.GetParticleCount(currentFrameIndex));

	auto *deviceContext = static_cast<ID3D11DeviceContext *>(
		context->GetAPIHandle(SimContextHandle::D3D11_DEVICE_CONTEXT)
	);

	D3D11_MAPPED_SUBRESOURCE mappedSubresource = {};
	if (const auto result = deviceContext->Map(
			positionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource
		);
		FAILED(result)) {
		throw std::runtime_error(
			"CD3D11RTFRFluidSimulation::LoadFrameIntoBuffers: Failed to "
			"map the position buffer."
		);
	}

	auto *positionData = static_cast<SimFloat4 *>(mappedSubresource.pData);
	for (int i = 0; i < simData->GetActiveParticles(); i++) {
		const float *positionVec = dataset.GetParticle(i, currentFrameIndex);
		positionData[i] = {
			positionVec[0], positionVec[1], positionVec[2], 1.0f
		};
	}

	deviceContext->Unmap(positionBuffer, 0);

	// Copy to the specified buffer in the simulation data.
	if (!simData->IsBufferLinked(SimBufferType::POSITION)) {
		throw std::runtime_error(
			"CD3D11RTFRFluidSimulation::LoadFrameIntoBuffers: Position "
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
			"CD3D11RTFRFluidSimulation::LoadFrameIntoBuffers: Failed to "
			"get position buffer resource."
		);
	}

	ID3D11Resource *linkedResource = nullptr;
	if (const auto result = linkedBuffer->QueryInterface(
			__uuidof(ID3D11Resource), reinterpret_cast<void **>(&linkedResource)
		);
		FAILED(result)) {
		throw std::runtime_error(
			"CD3D11RTFRFluidSimulation::LoadFrameIntoBuffers: Failed to "
			"get linked buffer resource."
		);
	}

	deviceContext->CopyResource(linkedResource, positionResource);
	deviceContext->Flush();	 // send off the copy command
}

void CD3D11RTFRFluidSimulation::SetMaxParticles(int maxParticles) {
	if (maxParticles <= 0) {
		throw std::invalid_argument(
			"CD3D11RTFRFluidSimulation::SetMaxParticles: maxParticles must be "
			"greater than 0."
		);
	}

	this->maxParticles = maxParticles;
	simData->SetMaxParticles(maxParticles);
}

void CD3D11RTFRFluidSimulation::Initialize() {
	CreateBuffers();
	currentFrameIndex = 0;
	LoadFrameIntoBuffers();
}

void CD3D11RTFRFluidSimulation::LoadDatasetFromFolder(
	const std::filesystem::path &folderPath
) {
	datasetInfo = rtfr::LoadDatasetInfo(folderPath);
	dataset.LoadDataset(datasetInfo);

	SetMaxParticles(datasetInfo.particleCount);
}

ISimData *CD3D11RTFRFluidSimulation::GetSimulationData() { return simData; }

SimContextAPI CD3D11RTFRFluidSimulation::GetComputeAPI() {
	return SimContextAPI::D3D11;
}

void CD3D11RTFRFluidSimulation::AttachToContext(
	const GellyObserverPtr<ISimContext> context
) {
	this->context = context;
}

static float t = 0.f;
constexpr float DELAY_TIME = 1 / 60.f;
void CD3D11RTFRFluidSimulation::Update(const float deltaTime) {
	t += deltaTime;
	// quick n dirty

	if (t >= DELAY_TIME) {
		t = 0.f;
		currentFrameIndex += 1;
		if (currentFrameIndex >= datasetInfo.frameCount) {
			currentFrameIndex = 0;
		}
	}

	LoadFrameIntoBuffers();
}