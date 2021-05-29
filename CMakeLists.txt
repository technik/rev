################################################################################
# Rev project
################################################################################
cmake_minimum_required (VERSION 3.10)
project(revAll)

# --- Configurable options -----
option(REV_BUILD_TOOLS "Build the asset processing tools" OFF)
option(REV_BUILD_TEST "Build engine unit tests" ON)
option(REV_BUILD_BENCHMARK "Build engine benchmarks" ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	# Target AVX2 architecture
    add_compile_options(/arch:AVX2)
endif()

# Libraries
if(REV_BUILD_BENCHMARK)
	set(BENCHMARK_ENABLE_GTEST_TESTS FALSE)
	set(BENCHMARK_ENABLE_INSTALL FALSE)
	add_subdirectory(engine/lib/benchmark)
	set_target_properties(benchmark PROPERTIES FOLDER benchmarks)
	set_target_properties(benchmark_main PROPERTIES FOLDER benchmarks)
endif()

# Find GLSL compiler
find_package(Vulkan REQUIRED)
set(VULKAN_BIN_DIR ${Vulkan_INCLUDE_DIR}/../bin/)
set(GLSLANGVALIDATOR ${VULKAN_BIN_DIR}glslc.exe)

add_subdirectory(engine)

if(REV_BUILD_TOOLS)
	add_subdirectory(tools/gltfOptimizer)
	set_target_properties(gltfOptimizer PROPERTIES FOLDER "tools")
	add_subdirectory(tools/probe)
endif()

# Compile sample projects
add_subdirectory(samples/ShaderToy)
add_subdirectory(samples/GltfViewer)

if(REV_BUILD_TEST)
	enable_testing()
	include_directories(engine/src)
	add_subdirectory(test/unit/math)
endif()