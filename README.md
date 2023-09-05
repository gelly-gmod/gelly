# gelly 💦

Gelly is a fun fluid simulation addon to Garry's Mod.
**Gelly is not cross-platform, and only supports Windows.**

## System requirements

## Note about NVIDIA GPUs and AMD GPUs

Ensure that you have the latest drivers installed. Gelly will not work without a
DX11.1 capable GPU. Gelly is highly GPU-intensive, so you'll probably want some GPU that has vendor-specific extensions
to speed up simulation. Most NVIDIA GPUs after 2012 have these extensions. NVIDIA GPUs are recommended, as they have the
most compatibility with Gelly. Personally, I use a RTX 3060 TI with Gelly, and it can simulate a million particles at
around 40 FPS.

Right now, AMD GPU support has not been tested, but it should work.

## Minimum requirements

- Windows 10 or higher
- DirectX 11.1 capable GPU with >= 2GB VRAM, NVIDIA or AMD only.
- At least 8GB of RAM

# Building from source

Gelly tries not to restrict what IDE you use to build it. However, it's highly recommended to use CLion as several
profiles and presets are set up so that you can easily switch between building the binary module or test applications.

## Prerequisites

- CMake 3.15 or higher
- Windows SDK 10.0.22000.0 or higher, any Windows SDK with up-to-date headers should work.
- Visual Studio 2022 OR MinGW-w64 9.0 or higher
- Ninja (preferred generator)
- CLion in New UI mode (optional)

The prerequisites aren't set in stone, but it's highly recommended to use the above versions of the tools. For example,
you can build using a Windows SDK lower than 10.0.22000.0 if it has DX11.1 and DX9 headers, along with WRL headers. You
should also be able to build with VS2019, but that hasn't been tested.

Again, CLion is optional but highly recommended. It's the IDE I use to develop Gelly, and it's the IDE that has the most
painless setup. If you don't want to use CLion, you can use any IDE that supports CMake.

## Building

1. Start by recursively cloning the repository. This is critical for the dependencies to be detected by CMake.
2. Open the project in CLion. If you're not using CLion, open the project in your IDE of choice.
3. If you're using CLion, you can switch between building the binary module and the test applications by changing the
   CMake profile. Make sure to synchronize the profile with the target. For example, if you're using the
   gelly-binary-module profile, make sure to select the gelly_binary_module target.
4. If you're not using CLion, then you'll have to use your IDE's custom profile system. For VS2022, CMakePresets.json is
   the analog to CLion's CMake profiles. Currently, Gelly has no file for this, but contributions for this are welcome.
5. Build the target. If you're using CLion, you can build the target by clicking the white hammer next to the
   target. If you're not using CLion, you'll have to use your IDE's build system.
6. Check the build output. If you're using CLion, you can see the build output as cmake-build-{profile name} in the
   Project window. If you're not using
   CLion, you'll have to check the build output in your IDE.
7. For gelly-binary-module, the output is in the {output_build_directory}/release directory. For any gelly test
   application,
   the output is in the {output_build_directory}/modules/{module_being_tested_name}/test directory.

# Installation (from building manually)

Copy all the DLLs in the `gelly-binary-module` release directory starting with `NvFlex` into the root of your Garry's
Mod folder. It's usually
named `GarrysMod`. Also copy the DLLs that are "GFSDK_AfterMath_Lib" and "amd_ags" into the root of your Garry's Mod
folder. The ending of these DLLs may change depending on what architecture you're building for.

After that, copy the "gmcl_gelly" DLL into your `garrysmod/lua/bin` folder. If `bin` does not exist, create
the `bin` folder in the `lua` folder.

# Using Gelly

**KNOWN BUG: Gelly's compositor is not finished, so you may experience rendering bugs in GMod after launching Gelly.**

**KNOWN BUG: Gelly's compositor does not work correctly with `gmod_mcore_test 1`, so set that convar to zero to run
Gelly.**

**KNOWN BUG: Gelly crashes each shutdown of GMod.**

There's no user-facing addon yet, so you'll have to use a test script to use Gelly in GMod. You can find the test script
in the `test-script` branch. I'd recommend copying it to your lua folder and running it in game
by `lua_openscript_cl gelly_test_script.lua`

Test script controls:

- R to spawn particles at your camera position and in your camera direction.
- M to clear all particles.