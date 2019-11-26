# CMakeList.txt : CMake project for CMakeProject1, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(MODULE_NAME TerrainDemo)

# Add source to this project's executable.
add_executable (
	${MODULE_NAME}
	
	TerrainDemo.cpp
	TerrainDemo.h
)
	
# TODO: Add tests and install targets if needed.

target_link_libraries(${MODULE_NAME} LINK_PUBLIC Core)
