#ifndef SIGNATURES_H
#define SIGNATURES_H

namespace sigs {
/**
 * \brief GMod version when the signatures were generated.
 */
constexpr int VERSION = 240202;
constexpr const char *const CMaterialSystem_GetLocalCubemap =
	// Note: Only redo this sig if something huge happens--those wildcards are
	// really important because they mask out the relative LEA offsets which
	// pretty much makes it update-proof.
	"40 53 48 83 ec 20 48 8b d9 48 8d 0d f4 2a 0c 00 ff 15 ?? ?? 08 00 48 8b "
	"c8 48 85 c0 75 07 48 8d 8b f0 24 00 00";
constexpr const char *const CMaterialSystem_AllowThreading =
	"48 89 5c 24 08 48 89 6c 24 10 48 89 74 24 18 57 48 83 ec 20 41 8b e8 0f "
	"b6 da 48 8b f9 ff 15 25 0a 09 00 41 b8 02 00 00 00 48 8d 15 f8 47 09 00 "
	"48 8b c8 4c 8b 08 41 ff 51 40 0f b6 b7 99 30 00 02 33 c9 83 f8 02 0f 4c "
	"d9 ff 15 31 0b 09 00 80 78 05 02 72 7f 8b 87 58 2f 00 02 88 9f 99 30 00 "
	"02 84 db";

constexpr const char *const CShaderAPIDX8_GetD3DTexture =
	"48 85 d2 75 03 33 c0 c3 80 7a 1a 01 48 8b 42 58 74 08 0f b6 4a 1b 48 8b "
	"04 c8";
constexpr const char *const CTexture_GetTextureHandle =
	"48 89 5c 24 08 48 89 74 24 10 57 48 83 ec 20 33 ff 8b da 48 8b f1 85 d2";
}  // namespace sigs

#endif	// SIGNATURES_H
