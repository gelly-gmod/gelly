add_custom_target(
        GellyD3D11Shaders
        COMMAND ${CMAKE_COMMAND}
        -DSRC_DIR=${CMAKE_CURRENT_SOURCE_DIR}
        -DLOCAL_SHADER_PATH="shaders/d3d11"
        -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH} # Passthrough since this variable isn't set when we run this CMake script.
        -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/PreprocessD3D11.cmake
        COMMENT "Compiling D3D11 shaders..."
)

add_custom_target(
        GellyD3D9Shaders
        COMMAND ${CMAKE_COMMAND}
        -DSRC_DIR=${CMAKE_CURRENT_SOURCE_DIR}
        -DLOCAL_SHADER_PATH="shaders/d3d9"
        -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH} # Passthrough since this variable isn't set when we run this CMake script.
        -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/PreprocessD3D9.cmake
        COMMENT "Compiling D3D9 shaders..."
)