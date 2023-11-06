# Since Testbed is programmed in tandem with Gelly, we do skip
# the part where we would add the gelly subdirectory.

add_executable(testbed
        src/main.cpp
        )

include_and_link_gelly(testbed)