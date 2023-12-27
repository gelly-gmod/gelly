# gelly 🌊

Gelly is a fluid simulation and rendering library for easy integration of dynamic fluids into any game engine.
**Read the next section for information on how to use it in GMod.**

---

# Bleeding-edge installation / development

1. Clone the repository
2. Run `git submodule update --init --recursive` to get all submodules
3. If you're just wanting to get your own local copy of the DLL, you can use:
    - `cmake --build --target gelly-gmod --preset Gelly-GMod-RelWithDebInfo`
    - The resulting DLL will be in `bin/packages/gelly-gmod/gmcl_gelly-gmod_winXX.dll`.
    - It's not that different if you want to develop, you can just use any modern IDE and it will pick up on the CMake presets. Choose the build presets for whatever you want to develop for.
4. In the GMod root folder, copy over the following DLLs from the FleX submodule in `packages/gelly/modules/gelly-fluid-sim/vendor/FleX`
    - `bin/winXX/amd_ags_xXX.dll`
    - `bin/winXX/NvFlexReleaseD3D11_xXX.dll`
    - `bin/winXX/NvFlexDeviceRelease_xXX.dll`
    - `nvToolsExt64_1.dll`
    - Yes.. it's quite complicated and a better solution is being worked on.
5. Once complete, head to `garrysmod/addons` from the root folder.
6. Clone the repository again, but select the `sp-addon` branch.
    - `git clone -b sp-addon https://github.com/yogwoggf/gelly.git`
7. Play GMod in singleplayer.
    - The current feedback for if things went smooth is if it can't find the DLL, it'll make a popup saying so.
    - Soon, there will be a notification in the top right corner of the screen.

---
The rest of this README is about the internal workings and development process. There's no need to read further if you just
want to use Gelly for a specific integration such as GMod.
# Project structure

Gelly is a monorepo, and referring to Gelly usually means to the library or the project as a whole. The project is split into multiple packages, each with their own purpose.

## Packages

### gelly

This contains the core library, which handles fluid simulation and rendering. It's written in C++ and supports multiple backends for rendering and simulation.
Currently, only D3D11 for rendering is supported, but there might be a chance for an advanced backend in Vulkan.

### gelly-gmod

This contains the GMod integration, which is written in C++ and Lua. It's a GMod module that exposes the core library to Lua, and composites the fluid renderer onto the screen.
It's the main "spectacle" of the project, and is the main reason why Gelly exists.

### testbed

This contains a personal visualization engine for testing the core library. It's written in C++ and uses the core library to simulate and render fluids.
It uses D3D11, fastgltf and many more libraries to support basic loading of glTF scenes and having them interact with fluids. Most of the time, it's
pretty much a playground for the core library--but should be used over gelly-gmod for renderer changes.

## Branches

### master

This contains the bleeding edge of the project. It's the main branch, and is the most unstable. There is currently no versioning standard or
lifecycle procedures, so this is the only branch that is actively developed on.

### sp-addon

This contains the GMod addon, it's split this way for flexibility. For example, power users can clone this branch to get bleeding-edge updates to the addon.