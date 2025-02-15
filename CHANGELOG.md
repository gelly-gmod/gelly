# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.36.2] - 2025-02-15

### Fixed

- Fixed blood preset being wrong
- Fixed performance debugger not working properly

## [1.36.0] - 2025-02-14

### Added

- Explosions now affect fluid! You can now see fluid react to explosions, which should look pretty cool. Explosions are
  scaled by the power of the explosion, so an RPG missile will cause more fluid to be displaced than a grenade or SMG
  nade.

### Changed

- Tuned blood preset to be less spiky
- The gelly gun now adjusts density in relation to the radius, so large radius fluids dont explode as much anymore.

## [1.35.0] - 2025-02-13

### Added

- Bullets now affect fluid! You can now shoot fluid with bullets and see the fluid react to the bullet. The strength is
  related to the power of your weapon, so shotguns will cause more fluid to be displaced than pistols for example.

### Fixed

- Player collision no longer disappears after ragdolling / dying.

## [1.34.4] - 2025-02-12

### Fixed

- Fixed particle culling randomly deactivating when changing parameters.

## [1.34.3] - 2025-02-12

### Added

- Added a "Particle Culling" toggle to the menu, which you can use to remove the filter for small particles.
- Added the "Particle Mass" slider to the menu, which you can use to change the mass of the particles. You can use this
  to mimic oil on water.
- Added the "Anisotropy Min" and "Anisotropy Max" sliders to the menu, which you can use to change the anisotropy of the
  particles. You can use this to make the fluid look more spherical or spiky.

### Changed

- Improved the filter for small particles, which should make the fluid look smoother from any distance.
- Made the simulation tick at a constant delta time, so high sim rates may appear "sped-up" but the actual behavior
  should no longer be affected like before.
- Gallium is now blobbier and slippier.
- When smoothing iterations is set to zero in the developer menu, you can now see the raw normals of the fluid.

### Fixed

- Fixed the Allow Threading signature and the addon now loads properly on the February 12th update of Garry's Mod.

## [1.34.2] - 2025-02-11

### Changed

- Made gallium slide more like how it should.
- Made gallium much more tense, so it globs up more.
- Removed viscosity from gallium, which means it may "shake" more than before.

## [1.34.1] - 2025-02-11

### Fixed

- Fixed fluid disappearing into pixely bits when viewed afar. Holes may still appear, but they shouldn't be as bad as
  before this update.
- Fluid should remain consistent from all distances now. It is significantly harder to assure this for low-radius
  fluids, but it is apparent for high-radius fluids.

## [1.34.0] - 2025-02-11

### Added

- Added a new visual mod menu in the customization menu. Its under the Mods tab and enables you to select mods to use
  with Gelly.

### Changed

- The blood mod now allows you to remain normal gameplay with Gelly, so you can use the Gelly Gun or Liquifier. Once
  blood is emitted, however, it will clear all fluid and replace it with blood.
- The sandbox mod is now default and is no longer customizable. The visual mod menu is now the way to customize other
  mods.

### Fixed

- Patched any possibility of the no preset bug happening again.

## [1.33.1] - 2025-02-09

### Fixed

- Fixed a potential deadlock when you disable all mods.
- Fixed the welcome screen's logo not appearing.

## [1.33.0] - 2025-02-01

### Added

- Added a new preset menu, found in the same customization menu (open with M, by default). This menu allows you to
  quickly switch between presets without needing to use the Gelly Gun.
- Added a new preset, Gelq, a viscous but slippery purple-pink plastic fluid.
- Added a new preset, Gallium, a fully metallic fluid that reflects light like a metal.
- Added a new preset, Milk. which acts like Gell-O but is more opaque and white. :)
- Added scattering! You can now select "Scatter" in the Presets tab to have the fluid scatter light, which gives a more
  soft and jelly-like appearance.
- Added metals! You can now select "Metal" in the Presets tab to have the fluid reflect light like a metal.
- Added support for player collisions. This can be customized in the customization menu
  under Simulation.
- Added support for NPC collisions.
- Added support for ragdoll collisions.
- Added an entirely new foam system, which should look significantly better than the old one. It has less flickering and
  smoother transitions.

### Changed

- The prior method for collisions was removed and updated in this version, so hopefully you can expect more accurate and
  better compatibility with other models beyond the basic GMod props.
- Tuned blood preset to be more globby, but not that globby.
- Blood now squirts from both sides of a gunshot wound in the blood mod.
- Optimized map loading, so phys-heavy maps such as gm_gwater_remastered should load faster.
- Rewrote the entire simulation codebase. What that means is you can expect generally faster load times and performance
  improvements.
- Optimized the update loop. This should result in a consistent 60 FPS simulation, which means while it could look
  choppy, your FPS should be higher and performance should be better.
- Added a slider in Simulation named Max Diffuse Particles, which controls how many foam particles can spawn.

### Fixed

- Fixed fluid exploding when alt-tabbed or when the game is minimized.

## [1.29.6] - 2025-01-29

### Fixed

- Fixed Gelly not loading on the Jan 29th update of GMod.

## [1.29.5] - 2025-01-09

### Changed

- Updated blood mod to actually function properly
- Tuned blood preset to be less globby

## [1.29.4] - 2025-01-08

### Changed

- Changed all branding to the new Gelly logo
- That's it! Feel free to not update if you don't care about the logo.

## [1.29.3] - 2025-01-06

### Added

- Exposed surface tension! You can now mess with it on a scale from 0-1. It is formulated as a ratio of radius to reduce
  chances of simulation instability.

### Changed

- Multiple fluid presets now have surface tension to some degree, which should make them look more realistic.

## [1.29.2] - 2025-01-03

### Added

- Added new "Whitewater Strength" slider to control how much discoloration is applied to rapidly moving fluid.

### Changed

- Tweaked how refraction looks for smaller particles, it should look a bit more realistic now.
- Adjusted the density of the liquifier to reduce stuttering and lag.

## [1.29.1] - 2024-12-14

### Fixed

- Fixed the random and odd particle clusters or spiky particles that would appear in the fluid. (caught by @meetric1)
- Fluid should look much smoother now, particularly in turbulent areas.

## [1.29.0] - 2024-12-13

### Added

- Added the ability to reset every single setting in Gelly through the customization menu (hit M)

### Fixed

- Fixed a longstanding bug where an error would be thrown if the hex color was invalid

## [1.28.6] - 2024-12-3

### Fixed

- Fixed an error being thrown when Prop Resizer isn't installed

## [1.28.5] - 2024-11-25

### Fixed

- Fixed the cubemap balls sometimes being visible during gameplay

## [1.28.4] - 2024-11-24

### Fixed

- Actually fix the teleportation bug this time, for real.

## [1.28.3] - 2024-11-23

### Added

- Added a new scene system, there may be a slight performance increase with this change.
- Added prop resizer support

### Fixed

- Fixed the iconic teleportation bug (for sure this time) that caused the fluid to teleport randomly when props were
  spawned.

## [1.28.2] - 2024-11-16

### Fixed

- Fixed whitewater suddenly activating once the resolution scale was changed.

## [1.28.1] - 2024-10-29

### Changed

- Disc emitters now toggle their emission when used (when you press E and look at one)

## [1.28.0] - 2024-10-26

### Added

- Spray! You can now enjoy seeing light and foamy particles spray out of turbulent fluids.
- Foam! You can now see your particles begin to scatter light and foam up when they're turbulent.
- Bubbles... technically! You can now see white particles in the fluid when it's turbulent, which is a placeholder for
  bubbles.
- A new checkbox in the Simulation tab, "Whitewater Enabled," which enables the new spray and foam effects. You may
  disable this to completely disable the new effects and thus save on performance.
- A new per-preset checkbox, "Use Whitewater," which allows you to enable or disable whitewater on a per-preset basis,
  such that the effects aren't illogically enabled, such as Glunk or Blood.

### Changed

- Improved how thickness is calculated, which means thin presets like Blood should still look mostly uniform across
  their surface.

### Fixed

- Fixed a long-standing but particularly rare bug where fluid behind the camera, but not necessarily "out-of-frame"
  would appear as
  a silhouette in the thickness buffer.

### Removed

- Removed the old foam system, which was a bit of a mess and didn't look very good.

## [1.27.1] - 2024-10-26

### Added

- Added door collisions

## [1.27.0] - 2024-10-14

### Added

- Added a new slider, "Resolution Scale" in Graphics. This slider allows you to scale the resolution of the fluid
  simulation, which can be useful for performance tuning.
- Added a display for the version in the menu, it's in the top right corner.
- Added a new switch to Graphics for disabling the Glunk Lighting Fix, which *may* conflict with other mods.
- The Glunk Lighting Fix, a feature which makes Glunk look better by default in most maps.

### Changed

- Massive optimizations across the board, the fluid should be significantly faster now.
- Reduced default resolution scale to 0.75
- Optimize the surface filter to be more efficient particularly on low-bandwidth systems

### Fixed

- Fixed the Gelly Gun's grabber mode not working properly after 1.25.0
- Fixed resolution changes breaking the fluid simulation
- Fixed the UI not matching up to the resolution when changed.

### Removed

- Removed old legacy textures, the only reason this is mentioned is that it may particularly benefit low-VRAM systems.

## [1.26.0] - 2024-10-10

### Added

- Added three new sliders to the customization menu.
- Relaxation: controls fluid simulation convergence speed (dont mess with this unless you know what you're doing)
- Gravity: self-explanatory! have fun with it
- Collision Distance: controls how much of an offset the fluid has from the surface it's colliding with

### Changed

- Made blood significantly less bad looking, it should now be more like blood.
- Improved the liquifier
- Changed up a lot of the internal simulation details, so expect different behavior

### Fixed

- Fixed a sort of rare bug where the iterations/substeps would not apply on game start unless you modified a setting.

## [1.25.3] - 2024-10-09

### Added

- Added all missing icons for every entity or weapon in the addon
- Added a new icon for the spawnmenu tab

### Fixed

- Fixed the liquifier gun being pretty much unusable

## [1.25.2] - 2024-10-08

### Fixed

- Fixed reflections being slightly off from the game's color correction.

## [1.25.1] - 2024-10-08

### Fixed

- Fixed forcefields not working anymore after the max particles slider was changed.

## [1.25.0] - 2024-10-08

### Added

- Brand-new customization panel for the Gelly Gun (to get there, press M, hit "Mods").
- Added more effects and polished the Gelly Gun.
- Added a new control system for the grabber mode (middle click + E), it should be a lot easier to control fluid now.
- Added the new Gelly Forcefield object to Entities, which allows you to create physical forcefields to control the
  fluid.
- A new spawnmenu tab for Gelly, which allows you to easily spawn Gelly objects.

### Changed

- Error messages are now more thorough when a graphical error happens on the GMod side.

### Removed

- Removed old foam system

## [1.24.2] - 2024-10-07

### Fixed

- Fixed Gelly failing to load on the October 7th update of Garry's Mod.

## [1.24.1] - 2024-10-05

### Added

- Added more information to `gelly_performance_debugger`, which is incredibly useful for debugging slow performance.

### Fixed

- Fixed flushing rendering commands when the query would do it anyways

## [1.24.0] - 2024-10-04

### Added

- A new surface filter, which should work very well at 5 smoothness, 768p to 1080p.

## [1.23.1] - 2024-10-01

### Fixed

- Fixed the **infamous** teleportation bug that caused the fluid to teleport randomly when props were spawned.
- Fixed the forcefield bug in which a silhouette of the last spawned prop would appear when the forcefield was active.

## [1.23.0] - 2024-10-01

### Added

- Sun lighting! The fluid now reacts to the sun and reflects it when it's in view.
- Fluids can now be turned opaque with the new "Opaque" switch in the Presets tab of the customization menu.

### Changed

- All lights are significantly less wide in the fluid reflections to prevent the fluid from looking too bright.
- Blood preset is significantly more opaque.
- Blood sticks to surfaces more.

### Fixed

- Fixed cubemaps sometimes not updating properly
- Removed several debug prints that were left in the code

## [1.22.6] - 2024-09-30

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
