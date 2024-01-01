#ifndef IMAPPEDBUFFERVIEW_H
#define IMAPPEDBUFFERVIEW_H

#include <GellyInterface.h>
#include <GellyInterfaceRef.h>

#include <stdexcept>
#include <type_traits>

#include "GellyDataTypes.h"
#include "IManagedBuffer.h"

using namespace Gelly::DataTypes;

/**
 * \brief A buffer view that can map GPU buffers to the CPU, although they have
 * to be dynamic. This class is perfect for implementing batched
 * modifications--but if you want something fire and forget, you can use the
 * original Modify function on the buffer itself.
 */
gelly_interface IMappedBufferView {
public:
	virtual ~IMappedBufferView() = default;

	virtual void AttachToContext(GellyInterfaceRef<IRenderContext> context) = 0;
	virtual void View(GellyInterfaceRef<IManagedBuffer> buffer) = 0;

	virtual void* GetBufferStart() = 0;
	virtual size_t GetBufferSize() = 0;
	/**
	 * \brief Really a convenience function since you can get this info from the
	 * original buffer.
	 * \return The stride of the buffer, in bytes.
	 */
	virtual size_t GetElementSize() = 0;

	template<typename T>
	T* GetBufferStart() {
		return static_cast<T*>(GetBufferStart());
	}

	template<typename T>
	void Write(constexpr uint index, const T& data) {
		static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");
		static_assert(std::is_standard_layout_v<T>, "T must be standard layout");

		if (constexpr uint byteIndex = index * sizeof(T); byteIndex >= GetBufferSize()) {
			throw std::out_of_range("IMappedBufferView::Write: index out of range");
		}

		GetBufferStart<T>()[index] = data;
	}
};

#endif //IMAPPEDBUFFERVIEW_H
