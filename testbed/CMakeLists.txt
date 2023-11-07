# Since Testbed is programmed in tandem with Gelly, we do skip
# the part where we would add the gelly subdirectory.

add_executable(testbed
        src/main.cpp
        src/Window.h src/Window.cpp src/Logging.h src/ILogger.h src/CConsoleLogger.cpp src/CConsoleLogger.h src/Logging.cpp src/Rendering.h)

add_subdirectory(vendor/SDL)

target_include_directories(
        testbed
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL/include
)

target_link_libraries(
        testbed
        PRIVATE
        SDL2
)

include_and_link_gelly(testbed)

# https://github.com/libsdl-org/SDL/issues/6399#issuecomment-1280612287
if (WIN32)
    add_custom_command(
            TARGET testbed POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL2::SDL2>" "$<TARGET_FILE_DIR:testbed>"
            VERBATIM
    )
endif ()