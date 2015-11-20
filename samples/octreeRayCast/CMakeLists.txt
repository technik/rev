################################################################################
# Rev project
# Sample project that renders a simple shader into a quad
################################################################################
cmake_minimum_required (VERSION 2.6)
project(octreeRayCastSample)

# Add rev to this project
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../engine ${CMAKE_CURRENT_BINARY_DIR}/rev)

add_executable(octreeRayCast main.cpp)
#target_link_libraries(shaderToy rev)
# Visual studio specifics
if(MSVC)
	add_definitions(-DNOMINMAX)
endif(MSVC)
if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=c++11)
endif()

target_link_libraries (octreeRayCast LINK_PUBLIC rev)