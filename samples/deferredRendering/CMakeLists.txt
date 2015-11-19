################################################################################
# Rev project
# Sample project that creates a simple scene graph
################################################################################
cmake_minimum_required (VERSION 2.6)
project(deferredRenderingSample)

# Add rev to this project
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../engine ${CMAKE_CURRENT_BINARY_DIR}/rev)

add_executable(deferredRendering main.cpp)
# Visual studio specifics
if(MSVC)
	add_definitions(-DNOMINMAX)
endif(MSVC)
if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=c++11)
endif()

target_link_libraries (deferredRendering LINK_PUBLIC rev)