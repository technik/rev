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
	add_subdirectory(include/benchmark)
	set_target_properties(benchmark PROPERTIES FOLDER libs)
	set_target_properties(benchmark_main PROPERTIES FOLDER libs)
endif()

# Find Vulkan and GLSL compiler
find_package(Vulkan REQUIRED)
set(VULKAN_BIN_DIR ${Vulkan_INCLUDE_DIR}/../bin/)
set(GLSLANGVALIDATOR ${VULKAN_BIN_DIR}glslc.exe)

# ImGui source
set(ImGui_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui/)
set(ImGui_SRC 
	${ImGui_ROOT}imgui.cpp
	${ImGui_ROOT}imgui_demo.cpp
	${ImGui_ROOT}imgui_draw.cpp
	${ImGui_ROOT}imgui_tables.cpp
	${ImGui_ROOT}imgui_widgets.cpp
	${ImGui_ROOT}backends/imgui_impl_vulkan.cpp
	${ImGui_ROOT}backends/imgui_impl_vulkan.h
	${ImGui_ROOT}backends/imgui_impl_win32.cpp
	${ImGui_ROOT}backends/imgui_impl_win32.h)
add_library(ImGui STATIC ${ImGui_SRC})
target_include_directories(ImGui PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include/imgui
	${Vulkan_INCLUDE_DIRS})
set_target_properties(ImGui PROPERTIES FOLDER libs)

add_subdirectory(engine)

if(REV_BUILD_TOOLS)
	add_subdirectory(tools/gltfOptimizer)
	set_target_properties(gltfOptimizer PROPERTIES FOLDER "tools")
	add_subdirectory(tools/probe)
endif()

# Compile sample projects
add_subdirectory(samples/Pluto)
add_subdirectory(samples/ShaderToy)
add_subdirectory(samples/GltfViewer)

if(REV_BUILD_TEST)
	enable_testing()
	include_directories(engine)
	add_subdirectory(test/unit/math)
endif()