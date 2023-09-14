# Gelly Shaders

This folder contains the shaders used by Gelly for each platform (D3D9, D3D11).

# D3D9

These shaders are written in HLSL and are targeted for Shader Model 2.0. It's best to not use any fancy features or make giant shaders, as there is a good chance the compiler will fail to compile them.

# D3D11

These shaders are also written in HLSL and are targeted for Shader Model 5.0. You can use all the fancy features you want, but keep in mind that performance is key in this platform since these shaders are directly responsible for drawing the fluid.

# IDE

Personally, I use VSCode, and this folder has some VSCode-specific files that make it easier to write shaders. You can use whatever you want, but I recommend using VSCode with the [Shader languages support for VS Code](https://marketplace.visualstudio.com/items?itemName=slevesque.shader) extension. That is also recommended automatically once you open this folder in VSCode.

# Compiling

There is an offline compilation step done through CMake, and it's pretty much indirect. Simply compile anything that uses shaders, and those specific shaders will be compiled into a "embed.hlsl" file. These files are specially preprocessed to be compiled at runtime using D3DCompiler.