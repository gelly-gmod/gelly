#include "VTable.h"

#include <MinHook.h>

#include <cassert>

#define DEBUG_ASSERT_MH(status)                    \
	OutputDebugStringA(MH_StatusToString(status)); \
	assert(status == MH_OK);

void VTable::Init(void ***classInstance, int vtableSize) {
#ifdef _DEBUG
	assert(classInstance != nullptr);
	assert(vtableSize > 0);
#endif

	vtableCopy = static_cast<void **>(malloc(vtableSize * sizeof(void *)));
	memcpy(vtableCopy, *classInstance, vtableSize * sizeof(void *));

	size = vtableSize;
}

VTable::VTable(void ***classInstance, int size)
	: vtableCopy(nullptr), size(size) {
	Init(classInstance, size);
}

VTable::VTable() : vtableCopy(nullptr), size(0) {}

VTable::~VTable() {
	if (vtableCopy != nullptr) {
		free(vtableCopy);
		vtableCopy = nullptr;
	}
}

void **VTable::GetVTable() const { return vtableCopy; }

void VTable::Hook(int index, void *hook, void **original) {
#ifdef _DEBUG
	assert(index >= 0);
	assert(index < size);
	assert(hook != nullptr);
	assert(original != nullptr);
	assert(vtableCopy != nullptr);
#endif

	void **vtable = GetVTable();
	void *target = vtable[index];

	MH_STATUS status = MH_CreateHook(target, hook, original);
#ifdef _DEBUG
	DEBUG_ASSERT_MH(status);
#endif
}

void VTable::EnableHook(int index) const {
#ifdef _DEBUG
	assert(index >= 0);
#endif

	void **vtable = GetVTable();
	void *original = vtable[index];

	MH_STATUS status = MH_EnableHook(original);
#ifdef _DEBUG
	DEBUG_ASSERT_MH(status);
#endif
}

void VTable::DisableHook(int index) const {
#ifdef _DEBUG
	assert(index >= 0);
#endif

	void **vtable = GetVTable();
	void *original = vtable[index];

	MH_STATUS status = MH_DisableHook(original);
#ifdef _DEBUG
	DEBUG_ASSERT_MH(status);
#endif
}

void VTable::Unhook(int index) const {
#ifdef _DEBUG
	assert(index >= 0);
#endif

	void **vtable = GetVTable();
	void *original = vtable[index];

	MH_STATUS status = MH_RemoveHook(original);
#ifdef _DEBUG
	DEBUG_ASSERT_MH(status);
#endif
}