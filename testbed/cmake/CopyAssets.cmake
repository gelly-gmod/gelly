# This script also copies down the compiled shaders.

# We'll gather a list of files to copy, everything in assets/
# and the compiled shaders.

file(GLOB_RECURSE ASSETS RELATIVE "${ROOT_DIR}/assets" "${ROOT_DIR}/assets/*")
file(GLOB_RECURSE SHADERS RELATIVE "${ROOT_DIR}/src/shaders" "${ROOT_DIR}/src/shaders/*.dxbc")

foreach (ASSET ${ASSETS})
    get_filename_component(FILENAME ${ASSET} NAME)
    configure_file("${ROOT_DIR}/assets/${ASSET}" ${OUTPUT_DIR}/assets/${FILENAME} COPYONLY)
endforeach ()

foreach (SHADER ${SHADERS})
    get_filename_component(FILENAME ${SHADER} NAME)
    configure_file("${ROOT_DIR}/src/shaders/${SHADER}" ${OUTPUT_DIR}/shaders/${FILENAME} COPYONLY)
endforeach ()

message(STATUS "Copying assets to ${OUTPUT_DIR}/assets")