# CMakeList.txt : CMake project for CMakeProject1, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(MODULE_NAME EmptyLauncher)

# Add source to this project's executable.
add_executable (
	${MODULE_NAME}
	
	main.cpp
	DefaultState.cpp
	DefaultState.h
)
	
# TODO: Add tests and install targets if needed.

target_link_libraries(${MODULE_NAME} LINK_PUBLIC Core)


message("Copying Shaders")
add_custom_command(
		TARGET ${MODULE_NAME} PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
				${CMAKE_SOURCE_DIR}/Data/Shaders
				${CMAKE_CURRENT_BINARY_DIR}/Data/Shaders)
message("Done.")

#if( NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/Data/Images )
#	message("Copying Images")
#
#	add_custom_command(
#			TARGET ${MODULE_NAME} POST_BUILD
#			COMMAND ${CMAKE_COMMAND} -E copy_directory
#					${CMAKE_SOURCE_DIR}/Data/Images
#					${CMAKE_CURRENT_BINARY_DIR}/Data/Images)
#
#	message("Done.")
#endif()
#
#if( NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/Data/Objects )
#	message("Copying Objects")
#
#	add_custom_command(
#			TARGET ${MODULE_NAME} POST_BUILD
#			COMMAND ${CMAKE_COMMAND} -E copy_directory
#					${CMAKE_SOURCE_DIR}/Data/Objects
#					${CMAKE_CURRENT_BINARY_DIR}/Data/Objects)
#
#	message("Done.")
#endif()

message("Done.")