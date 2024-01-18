# gelly-cpu-refs

Gelly **CPU** **Ref**erences is a module dedicated to the implementation of several algorithms
using the CPU.

The main goal of the module is to provide a well-formed reference implementation of the algorithms,
which can be used to validate the correctness of the GPU implementations.

The reference implementations may be
accelerated using vectorization--**but by no means are they meant to be optimized for real-time applications.**

These algorithms are visualized and tested for stability and robustness, and are
finally implemented in the GPU, which usually live in the `gelly-fluid-renderer` module.

## Design

The algorithms are header-only, and are meant to be used as a library.
This allows for their usage in other Gelly modules and potentially as fallbacks for the GPU implementations.

The output of this module is a simple visualizer executable powered by raylib, which can be used to preview samples of
the algorithms.

## Algorithms

The currently planned algorithms are:

- Marching cubes with smoothing
- Space-skipping octrees for particles
- Frustum culling