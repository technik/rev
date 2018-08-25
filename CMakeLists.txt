################################################################################
# Rev project
# Sample project that creates a simple scene graph
################################################################################
cmake_minimum_required (VERSION 3.10)
project(revAll)

# --- Configurable options -----
option(REV_BUILD_TOOLS "Build the asset processing tools" OFF)
option(REV_BUILD_TEST "Build engine unit tests" ON)
option(REV_BUILD_PLAYER "Build rev player. This is rev's default client application" ON)
option(REV_WITH_OPENGL "Build OpenGL driver and related components. This allows using OpenGL as graphics back end" ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(engine)
if(REV_BUILD_TOOLS)
	add_subdirectory(tools)
endif()

if(REV_BUILD_PLAYER)
	add_subdirectory(player)
endif()

add_subdirectory(samples/Vulkraft)

if(REV_BUILD_TEST)
	enable_testing()
	include_directories(engine/src)
	add_subdirectory(test/unit/math)
	add_subdirectory(test/unit/game)
	add_subdirectory(test/unit/shaders)
endif()