# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.22.6] - 2024-09-29

### Fixed

- Fixed disc emitter leaking particles
- Fixed rest distance ratio being in the range of [0, 1] instead of [0.5, 0.7]

## [1.22.5] - 2024-09-29

### Added

- Added color overrides to disc emitters, which allows you to change the color of the fluid emitted by any disc emitter.

## [1.22.4] - 2024-09-29

### Changed

- Adjusted albedo/thickness scaling up to a quarter
- Implemented new cross filtering technique to reduce thickness artifacts

## [1.22.3] - 2024-09-28

### Changed

- Adjusted the mip curve such that waves and other details should still be preserved

### Fixed

- Fixed the pixelation of the fluid when the camera is close to the fluid.
- Fixed fluid mixing being virtually non-existent.
- **Note**: Fluid mixing is still not perfect, but it should be better than before. It **will not** be accurate, but it
  should be less jarring.

## [1.22.2] - 2024-09-27

### Fixed

- Fixed Glunk being pitch black

## [1.22.1] - 2024-09-27

### Fixed

- Fixed normals propagating through the mip chain, leading to a blurry appearance.
- Fixed the version popup not bolding the text properly.

## [1.22.0] - 2024-09-25

### Added

- Added the customization menu, a brand-new menu that allows you to customize almost every aspect of the fluid
  simulation and renderer.
- **Note**: By default, you can press **M** to open the menu.
- Adds console variables for most customization options.
- Adds a new console command, **gelly_toggle_customization**, to open and close the customization menu.
- **Note**: You may bind this to any key and Gelly will remove the M keybind for the customization menu.
- New version popup now has bolded text.

### Changed

- The 'max particles' setting is now a slider in the customization menu and a console variable.
- The range for the 'max particles' setting is now 10,000 to 1,500,000.

### Removed

- Removed legacy preset creator
- Removed even more legacy settings menu

## [1.21.5] - 2024-09-09

### Fixed

- Fixed Gelly breaking when it was loaded with the September 9th update of Garry's Mod.

## [1.21.4] - 2024-08-30

### Fixed

- Fixed previously simulated particles flickering when the simulation was reset.
- Fixed renderer resources not being marked properly for synchronization.
- This may or may not improve the square-shaped corruption bug.

## [1.21.3] - 2024-08-26

### Added

- Added a console command to clear particles while not having the Gelly Gun equipped.

### Changed

- Map cleanup now clears all particles.

## [1.21.2] - 2024-08-25

### Fixed

- Fixed the fluid sometimes flickering to random colors when the camera is moved.
- This may or may not fix the square flickering issue.

## [1.21.1] - 2024-08-25

### Fixed

- Certain maps that use displacements (TF2 Harvest, most notably) are now supported.
- Note that the performance of these maps may be worse than usual due to the nature of the fix.
- These maps are now marked as "bad" and will use the legacy loader to ensure correct simulation.

## [1.21.0] - 2024-08-24

### Added

- Added the disc emitter, a new small disc which emits the current fluid. Configure it with the context menu.
- Added support for high-radius fluids. Use the "gelly_preset_radius_scale" console command for now.

### Changed

- Linearized the mip regression function with respect to the radius of the fluid. This will make surfaces smoother even
  at high radius.
- Water preset is now less spiky, it should be more like water now.
- Piss preset is the same as the Water preset, but its color was strengthened.
- Gell-O preset was reworked from scratch, it should now be more like jelly.
- Glunk preset was smoothed out, it should be very creamy now.
- Blood preset was tuned to act more like blood, it should be more opaque now as well.

### Removed

- All the legacy emitters were removed, they were replaced by the disc emitter.

### Fixed

- Fixed changelogs being corrupted when they had multiple lines.
- Fixed FleX instability at high radius
- Fixed surface filter not working properly with high-radius fluids

## [1.20.4] - 2024-08-20

### Changed

- Reduced the visibility of individual particles in the thickness
- Thickness is now feathered, which should make it less apparent

## [1.20.3] - 2024-08-20

### Fixed

- Fixed refraction being diagonal, it now properly magnifies the background.

## [1.20.2] - 2024-08-20

### Changed

- Lowered filter iterations
- The new filter will account for bumpier surfaces, there shouldn't be a noticeable difference in quality, but a boost
  in performance.

## [1.20.1] - 2024-08-20

### Changed

- Significantly rewrote most of the splatting pass to use low-level optimizations
- In general, this update provides performance improvements.

## [1.20.0] - 2024-08-19

### Added

- Backported the old additive thickness model. This should alleviate the problem of seeing fluids across walls.

### Changed

- Piss radius was decreased.
- Piss friction was corrected to be the same as water.

### Removed

- Old physically-based thickness model

## [1.19.1] - 2024-07-22

### Fixed

- Fixed the new binary packing causing an exception when Gelly is loaded multiple times in the same session.
- It also fixes binary packing being incompatible with alternative mods, such as GWater2.

## [1.19.0] - 2024-07-22

### Added

- New welcome screen for first-time users

### Removed

- Removed the old welcome popup
- Old popups still appear in case of a fatal error

## [1.18.0] - 2024-07-21

### Changed

- External binaries no longer ship with Gelly, they are now embedded into the binary module.
- That means, installation should be less flaky.
- All one needs to do is drag and drop the *single* folder into the GMod root directory.
- README instructions were changed to be more audience-friendly.
- Besides that, this release is equivalent to the last one. You don't need to update if you're on 1.17.0.

## [1.17.0] - 2024-07-13

### Changed

- Added pixel-size invariant rendering, which means the fluid will remain smooth no matter how close or far away you are
  from it.

### Fixed

- Fixed normals becoming a little biased in the last surface filter update.

## [1.16.0] - 2024-07-12

### Added

- New surface filter, which allows for generally smoother and less glitchy fluid rendering.

## [1.15.0] - 2024-07-09

### Added

- Added PHYParser to Gelly
- Added the asset cache, an in-memory cache for asset vertex data which should speed up instantiation of props

### Fixed

- Fixed the mod addition restriction system failing in some cases

### Changed

- Prop models now go through the asset cache and exclusively use PHY collision models.
- Maps now go through PHYParser if possible, falling back on render models provided by BSPParser.
- Diffuse rendering has been fixed and now appears correctly.

## [1.14.0] - 2024-07-04

### Added

- A new console variable, "gelly_preset_radius_scale," to adjust the radius of the fluid presets.
- Radius scaling is now applied to all presets, including the default ones.

### Fixed

- Fixed Gelly attempting to add entities with no physical representation to the simulation.

## [1.13.1] - 2024-07-03

### Fixed

- Brush models are now supported
- Map entities are now visible to Gelly and can be interacted with
- Gelly now works with maps such as gm_gwater.

## [1.13.0] - 2024-07-02

### Added

- Forcefield support in Gelly
- New forcefield functions in Gelly API
- A forcefield SENT and library in GellyX
- Forcefields can be created with the Gelly Gun by holding the middle mouse button and pressing E.

### Changed

- The simulation scene now performs a dirty update when an object is removed.
- Water preset is a tad more green

### Fixed

- Rendering/composition turns off when Gelly as a whole is inactive

## [1.12.2] - 2024-07-02

### Changed

- Changes the water preset to be more like the ocean, with a deep blue color.
- Removes the thickness limit, which may appear a little weird but ultimately makes for far more realistic lighting.

## [1.12.1] - 2024-06-29

### Fixed

- In FleX, props would spawn in place of another, causing some particles to shoot out of the prop. This has been fixed.
- Fixed `gmod_wheel` objects not being picked up by the simulation.

### Changes

- Runs FleX with less margin and substeps to improve performance
- To account for the above change, the relaxation factor has decreased to 0.25 and now uses global relaxation to
  encourage faster convergence.
- Gelly Gun now uses jittering to ensure that particles do not bug out when in proximity to the gun's spawn point.
- Back depth filtering is now disabled by default, as it was causing performance losses with no real visual gains.

## [1.12.0] - 2024-06-28

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
- HDR fix is now configurable via gelly_hdr_fix and disabled on unknown maps.
- HDR fix is on forcibly if a known-bad map is loaded to ensure the best experience.
- Filter threshold ratio has been adjusted, which eliminates "halos" around some fluids.

### Fixed

- Fixed the preset creator having wacky colors
- Fixed frustrum culling occasionally glitching and rendering all fluids when viewed perpendicularly (or, sideways)
- Frustrum culling now works properly with the new renderer and properly culls any fluids that are not in view

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

> > > > > > > master

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
