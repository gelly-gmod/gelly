#include "D3D11DebugLayer.h"

#include <d3d11.h>
#include <stdio.h>

using namespace testbed;

static ILogger *logger = nullptr;

static const char *MessageCategoryToString(D3D11_MESSAGE_CATEGORY category) {
	switch (category) {
		case D3D11_MESSAGE_CATEGORY_APPLICATION_DEFINED:
			return "Application defined";
		case D3D11_MESSAGE_CATEGORY_MISCELLANEOUS:
			return "Miscellaneous";
		case D3D11_MESSAGE_CATEGORY_INITIALIZATION:
			return "Initialization";
		case D3D11_MESSAGE_CATEGORY_CLEANUP:
			return "Cleanup";
		case D3D11_MESSAGE_CATEGORY_COMPILATION:
			return "Compilation";
		case D3D11_MESSAGE_CATEGORY_STATE_CREATION:
			return "State creation";
		case D3D11_MESSAGE_CATEGORY_STATE_SETTING:
			return "State setting";
		case D3D11_MESSAGE_CATEGORY_STATE_GETTING:
			return "State getting";
		case D3D11_MESSAGE_CATEGORY_RESOURCE_MANIPULATION:
			return "Resource manipulation";
		case D3D11_MESSAGE_CATEGORY_EXECUTION:
			return "Execution";
		case D3D11_MESSAGE_CATEGORY_SHADER:
			return "Shader";
	}
}

static const char *MessageSeverityToString(D3D11_MESSAGE_SEVERITY severity) {
	switch (severity) {
		case D3D11_MESSAGE_SEVERITY_CORRUPTION:
			return "Corruption";
		case D3D11_MESSAGE_SEVERITY_ERROR:
			return "Error";
		case D3D11_MESSAGE_SEVERITY_WARNING:
			return "Warning";
		case D3D11_MESSAGE_SEVERITY_INFO:
			return "Info";
		case D3D11_MESSAGE_SEVERITY_MESSAGE:
			return "Message";
	}
}

DebugLayer testbed::InitializeRendererDebugLayer(
	ILogger *newLogger, ID3D11Device *device
) {
	logger = newLogger;
	ID3D11Debug *debugLayer = nullptr;
	if (FAILED(device->QueryInterface(
			__uuidof(ID3D11Debug), reinterpret_cast<void **>(&debugLayer)
		))) {
		logger->Error("Failed to initialize debug layer");
		return {};
	}

	ID3D11InfoQueue *infoQueue = nullptr;
	if (FAILED(debugLayer->QueryInterface(
			__uuidof(ID3D11InfoQueue), reinterpret_cast<void **>(&infoQueue)
		))) {
		logger->Error("Failed to initialize debug info queue");
		return {};
	}

	return {debugLayer, infoQueue};
}

void testbed::LogRenderDebugMessages(const DebugLayer &debugLayer) {
	const UINT64 messageCount = debugLayer.infoQueue->GetNumStoredMessages();
	for (UINT64 i = 0; i < messageCount; ++i) {
		SIZE_T messageLength = 0;
		HRESULT result =
			debugLayer.infoQueue->GetMessage(i, nullptr, &messageLength);
		if (FAILED(result)) {
			logger->Error("Failed to get debug message length");
			break;
		}
		auto *message = static_cast<D3D11_MESSAGE *>(malloc(messageLength));
		result = debugLayer.infoQueue->GetMessage(i, message, &messageLength);
		if (FAILED(result)) {
			logger->Error("Failed to get debug message");
			break;
		}

		auto *severityString = MessageSeverityToString(message->Severity);
		auto *categoryString = MessageCategoryToString(message->Category);

		char formattedMessage[2048];
		sprintf_s(
			formattedMessage,
			"[D3D11 DEBUG] - [%s | %s]: %s",
			severityString,
			categoryString,
			message->pDescription
		);

		if (message->Severity == D3D11_MESSAGE_SEVERITY_ERROR ||
			message->Severity == D3D11_MESSAGE_SEVERITY_CORRUPTION) {
			logger->Error(formattedMessage);
		} else {
			logger->Debug(formattedMessage);
		}

		free(message);
	}
}
