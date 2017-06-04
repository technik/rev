################################################################################
# Rev project
# Sample project that creates a simple scene graph
################################################################################
cmake_minimum_required (VERSION 2.6)
project(revAll)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
add_subdirectory(engine)
option(REV_BUILD_TOOLS "Build the asset processing tools" OFF)
if(REV_BUILD_TOOLS)
	add_subdirectory(tools)
endif()
add_subdirectory(player)