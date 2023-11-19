project(gsc CXX)
set(CMAKE_CXX_STANDARD 17)

add_executable(gsc src/main.cpp
        src/ShellCmd.cpp
        src/ShellCmd.h
        src/ShaderFile.cpp
        src/ShaderFile.h)