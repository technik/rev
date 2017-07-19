################################################################################
# Rev project
# Sample project that creates a simple scene graph
################################################################################
cmake_minimum_required (VERSION 2.6)
project(revAll)

# --- Configurable options -----
option(REV_BUILD_TOOLS "Build the asset processing tools" OFF)
option(REV_BUILD_PLAYER "Build rev player. This is rev's default client application" ON)
option(REV_USE_VULKAN "Use vulkan back end for rendering" ON)

if(REV_USE_VULKAN)
set(REV_WITH_OPENGL OFF)
else()
set(REV_WITH_OPENGL ON)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
add_subdirectory(engine)
if(REV_BUILD_TOOLS)
	add_subdirectory(tools)
endif()

if(REV_BUILD_TOOLS)
	add_subdirectory(player)
endif()