project(gsc CXX)
set(CMAKE_CXX_STANDARD 17)

add_executable(gsc src/main.cpp
        src/ShellCmd.cpp
        src/ShellCmd.h
        src/ShaderFile.cpp
        src/ShaderFile.h
        src/ShaderFileCompiler.cpp
        src/ShaderFileCompiler.h
        src/GlueCodeGen.cpp
        src/GlueCodeGen.h)

set(GSC_PATH ${CMAKE_CURRENT_BINARY_DIR}/gsc.exe PARENT_SCOPE)