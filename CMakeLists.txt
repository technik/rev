################################################################################
# Rev project
# Sample project that creates a simple scene graph
################################################################################
cmake_minimum_required (VERSION 2.6)
project(revAll)

# --- Configurable options -----
option(REV_BUILD_TOOLS "Build the asset processing tools" OFF)
option(REV_BUILD_TESTS "Build the unit tests for the engine" OFF)
option(REV_BUILD_PLAYER "Build rev player. This is rev's default client application" ON)
option(REV_WITH_OPENGL "Build OpenGL driver and related components. This allows using OpenGL as graphics back end" ON)
option(REV_WITH_HEADTRACKING "Build OpenGL driver and related components. This allows using OpenGL as graphics back end" ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(cmake/common.cmake)

# Libraries required by various modules
# ---- Eigen ----
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

add_subdirectory(engine)
if(REV_BUILD_TOOLS)
	add_subdirectory(tools)
endif()

if(REV_BUILD_PLAYER)
	add_subdirectory(player)
endif()

# ---- Tests ----
if(REV_BUILD_TESTS)
	enable_testing()
	add_subdirectory("test/unit/math") # Unit tests
endif()