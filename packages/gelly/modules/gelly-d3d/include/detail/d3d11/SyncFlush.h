#ifndef GELLY_SYNCFLUSH_H
#define GELLY_SYNCFLUSH_H

#include <d3d11.h>

namespace d3d11 {
/**
 * Synchronously flushes the current command buffer.
 * @param device
 * @param context
 */
void SyncFlush(ID3D11Device *device, ID3D11DeviceContext *context);
}  // namespace d3d11

#endif	// GELLY_SYNCFLUSH_H
