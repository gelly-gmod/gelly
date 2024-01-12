#ifndef SIGNATURES_H
#define SIGNATURES_H

namespace sigs {
/**
 * \brief GMod version when the signatures were generated.
 */
constexpr int VERSION = 240111;
constexpr const char *const CMaterialSystem_GetLocalCubemap =
	// Note: Only redo this sig if something huge happens--those wildcards are
	// really important because they mask out the relative LEA offsets which
	// pretty much makes it update-proof.
	"40 53 48 83 ec 20 48 8b d9 48 8d 0d 44 2b 0c 00 ff 15 ?? ?? 08 00 48 8b "
	"c8 48 85 c0 75 07 48 8d 8b f0 24 00 00";
constexpr const char *const CShaderAPIDX8_GetD3DTexture =
	"48 85 d2 75 03 33 c0 c3 80 7a 1a 01 48 8b 42 58 74 08 0f b6 4a 1b 48 8b "
	"04 c8";
constexpr const char *const CTexture_GetTextureHandle =
	"48 89 5c 24 08 48 89 74 24 10 57 48 83 ec 20 33 ff 8b da 48 8b f1 85 d2";
}  // namespace sigs

#endif	// SIGNATURES_H
