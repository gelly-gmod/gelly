# Sets "ARCH" and "ARCH_SUFFIX" which are FleX-specific architecture names used for finding libraries.
# ARCH = "win32" or "win64"
# ARCH_SUFFIX = "x86" or "x64"
function(GetFleXArchitecture)
    set(ARCH "" PARENT_SCOPE)
    set(ARCH_SUFFIX "" PARENT_SCOPE)
    if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "AMD64")
        set(ARCH "win64" PARENT_SCOPE)
        set(ARCH_SUFFIX "x64" PARENT_SCOPE)
    elseif (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "x86")
        set(ARCH "win32" PARENT_SCOPE)
        set(ARCH_SUFFIX "x86" PARENT_SCOPE)
    else ()
        message(FATAL_ERROR "Unsupported architecture: ${CMAKE_HOST_SYSTEM_PROCESSOR}")
    endif ()
endfunction()