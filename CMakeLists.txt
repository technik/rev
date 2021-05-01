################################################################################
# Rev project
# Sample project that creates a simple scene graph
################################################################################
cmake_minimum_required (VERSION 3.10)
project(revAll)

# --- Configurable options -----
option(REV_BUILD_TOOLS "Build the asset processing tools" OFF)
option(REV_BUILD_TEST "Build engine unit tests" ON)
option(REV_BUILD_BENCHMARK "Build engine benchmarks" ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 20)

# Libraries
add_subdirectory(engine/lib/benchmark)
set_target_properties(benchmark PROPERTIES FOLDER benchmarks)
set_target_properties(benchmark_main PROPERTIES FOLDER benchmarks)

# Find GLSL compiler
find_package(Vulkan REQUIRED)
set(VULKAN_BIN_DIR ${Vulkan_INCLUDE_DIR}/../bin/)
set(GLSLANGVALIDATOR ${VULKAN_BIN_DIR}glslc.exe)

add_subdirectory(engine)

if(REV_BUILD_TOOLS)
	add_subdirectory(tools/gltfOptimizer)
	set_target_properties(gltfOptimizer PROPERTIES FOLDER "tools")
endif()

# Compile sample projects
add_subdirectory(samples/ShaderToy)
add_subdirectory(samples/GltfViewer)

if(REV_BUILD_TEST)
	enable_testing()
	include_directories(engine/src)
	add_subdirectory(test/unit/math)
endif()