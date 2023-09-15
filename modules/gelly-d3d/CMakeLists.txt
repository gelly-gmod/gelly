add_library(gelly_d3d9 STATIC
        include/GellyD3D.h
        include/detail/d3d11/Shaders.h include/detail/d3d11/Texture.h include/detail/d3d9/Texture.h include/detail/d3d9/Shaders.h src/d3d9/Shaders.cpp src/d3d9/Texture.cpp include/detail/d3d11/ErrorHandling.h src/d3d11/Shaders.cpp src/d3d11/Texture.cpp include/detail/d3d11/Buffer.h include/detail/d3d11/Cleanup.h src/d3d11/Cleanup.cpp include/detail/d3d11/ConstantBuffer.h)

target_link_libraries(gelly_d3d9 PUBLIC d3d9.lib d3d11.lib d3dcompiler.lib)
target_include_directories(gelly_d3d9 PUBLIC include)