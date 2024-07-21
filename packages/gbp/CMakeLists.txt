project(gbp CXX)
set(CMAKE_CXX_STANDARD 20)

add_executable(gbp
        src/main.cpp
        src/collect-dll-list-from-argv.h
        src/collect-dll-list-from-argv.cpp
        src/pack-dll.h
        src/pack-dll.cpp
)

set_target_properties(gbp PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

set(GBP_PATH ${CMAKE_CURRENT_BINARY_DIR}/gbp.exe PARENT_SCOPE)