# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.12.0] - 2024-06-27

### Added

- Entirely new renderer codebase
- New thickness model (not a new absorption model, that has remained the same)
- Basic underwater rendering (not perfect, but it's a start)
- New GPU synchronization option in the developer UI (can be toggled on and off for performance/debugging)

### Changed

- Every single preset has had their color tweaked.
- Water is significantly more dyed with blue.
- Piss is now more yellow.
- Blood is a saturated red and is more opaque.
- Gell-O has a deeper red color.
- Glunk now appears dark if you go into the fluid.
- Depth filter has been tweaked and therefore should have better performance.
- Normal fluid rendering is now disengaged once the camera is underwater and switched to underwater rendering.
- Frame rendering has been tweaked to more or less synchronize with the GPU, which means you may or may not see a
  performance increase.

### Fixed

- Fixed the preset creator having wacky colors

## [1.11.0] - 2024-06-18

### Added

- New absorption model which is more physically accurate.
- The new rule of thumb is to use hammer units when describing how fast each color channel is absorbed.

### Fixed

- Fixed the normal aliasing bug where viewing a fluid from a distance would cause it to turn black.

### Changes

- The renderer has been optimized, thickness should significantly be crisper.
- All presets have been updated to reflect the new absorption model.
- Thickness is now blurred several more times.

## [1.10.2] - 2024-06-17

### Fixed

- Fixed the nil transform bug, which tended to happen if an object was deleted in close succession to another object

## [1.10.1] - 2024-06-17

### Fixed

- Fixed the sandbox mod not being the default mod
- If you still have trouble, run: "gelly_mod select sandbox-mod" in the GMod console

## [1.10.0] - 2024-06-16

### Fixed

- Props no longer bleed when shot with the blood mod enabled.

### Changes

- The blood mod has been improved with different blood colors and a reworked bleeding system.

## [1.9.0] - 2024-06-15

### Added

- Added a new mod manager, which allows developers to create mods for Gelly. It also allows users to easily manage their
  installed mods.
- Added a command line interface, gelly_mod, to manage mods through the GMod console (only way to manage mods for now)
- Added a new mod, "Sandbox Mod," which is the default mod for Gelly. It enables the Gelly Gun and Gelly Liquifier.
- Added a clientside entity deletion function, particularly for internal use but exposed to the mod API

### Fixed

- Fixed the new version dialog not showing a multi-line message correctly
- Fixed the `.luarc.json` not resolving Gelly's modules correctly.

### Changes

- The blood mod is now disabled by default. It can be enabled with "gelly_mod select blood-mod" in the GMod console.
- The Gelly Gun and Gelly Liquifier are now part of the Sandbox Mod.

## [1.8.0] - 2024-06-15

### Added

- Added a material override system. Developer can now create interesting effects by overriding the material of the
  particles, for example, it's now possible to make rainbow fluid emitters.

## [1.7.0] - 2024-06-14

### Added

- Tiny particles which contribute less to the overall mass of the fluid are now filtered. Simulations should now have a
  generally more "compact" look.

## [1.6.1] - 2023-06-13

### Fixed

- Fixed the new version dialog showing up on the latest version

## [1.6.0] - 2023-06-13

### Added

- Added a new dialog for when a new version is available.

### Fixed

- Fixed the preset creator not loading properly

## [1.5.1] - 2023-06-12

### Fixed

- Colliders no longer have a 90 degree offset depending on the prop spawned

## [1.5.0] - 2023-06-11

### Added

- Entirely new CI/CD pipeline
- Clang/GCC support

### Changed

- Releases now use a static release folder at `./release`.
- The Gelly addon no longer requires the bootstrapper addon, it is now a standalone filesystem
  addon: `garrysmod/addons/gelly/gelly.gma`.

### Fixed

- Fixed the preset creator not responding to inputs

## [1.4.0] - 2023-06-10

### Changed

- Added changelogs and a version number
- Exposed version number in the binary module.
