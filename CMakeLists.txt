# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project (EngineSandbox)

message(STATUS "Setting MSVC Flags")

set (CMAKE_CXX_STANDARD 17)

include_directories(Core)

# Include sub-projects.
add_subdirectory (Core)
add_subdirectory (EmptyLauncher)
add_subdirectory (FrameBuffer)
add_subdirectory (ShadowMaps)
add_subdirectory (TerrainDemo)
#  add_subdirectory (RendererOgl)

# add_dependencies(RendererOgl Core)
add_dependencies(EmptyLauncher Core)
add_dependencies(FrameBuffer Core)
add_dependencies(ShadowMaps Core)
add_dependencies(TerrainDemo Core)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT EmptyLauncher)


