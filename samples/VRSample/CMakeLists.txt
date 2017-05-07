################################################################################
# Rev project
# Sample project that uses God of War shadows technique
################################################################################
cmake_minimum_required (VERSION 2.6)
project(VRSample)

add_executable(VRSample main.cpp player.cpp)
# Visual studio specifics
if(MSVC)
	add_definitions(-DNOMINMAX)
endif(MSVC)
if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=c++11)
endif()

target_link_libraries (VRSample LINK_PUBLIC rev)