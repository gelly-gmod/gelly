# GellyX API - 0.1.0

This is the API for interfacing with gelly-gmod's binary module--known as GellyX. It's named this to prevent confusion with the Lua API, which is just called Gelly.
This API is maintained and documented--it's also semantically versioned, so you can rely on it not to break between versions.

The raw API, (aka the Lua API) is constantly updated and some methods may be removed or changed, therefore it is recommended to rely on this API instead. The raw API has zero versioning and is not documented.

## Versioning
As mentioned above, the API uses [semantic versioning](https://semver.org/). This means that the version number is split into three parts: `major.minor.patch`. The major version is incremented when there are breaking changes, the minor version is incremented when there are new features, and the patch version is incremented when there are bugfixes.

---

The current version has the major version of `0`, which means that it is not guaranteed to be stable.

# Usage

GellyX is modeled like any other GMod addon API, it exists in the global namespace and uses the `gellyx` prefix. The primary usecase is the soft error functions, which bind to certain raw API functions but return a boolean instead of throwing an error. For simplicity, there's also convenience functions for the most common usecases, such as:

- Emitting particles in specific shapes
- Detecting when something has collided with particles
- Selecting presets / creating presets
- Changing quality settings
- Changing the simulation speed
- and more!