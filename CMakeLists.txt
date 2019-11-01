# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project (EngineSandbox)

message(STATUS "Setting MSVC Flags")

set(CMAKE_CXX_FLAG "${CMAKE_CXX_FLAGS} /std:c++11")

# Include sub-projects.
add_subdirectory ("EmptyLauncher")
add_subdirectory ("Core")
add_subdirectory ("RendererOgl")

add_dependencies(RendererOgl Core)
add_dependencies(EmptyLauncher RendererOgl)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT EmptyLauncher)
