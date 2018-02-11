################################################################################
# Rev project
# Sample project that uses God of War shadows technique
################################################################################
cmake_minimum_required (VERSION 2.6)
project(revPlayer)

include(../cmake/common.cmake)

file(GLOB_RECURSE PLAYER_SOURCE_FILES "src/*.cpp" "src/*.h" "src/*.inl")
GroupSources(src player)
add_executable(revPlayer ${PLAYER_SOURCE_FILES})
# Visual studio specifics
if(MSVC)
	add_definitions(-DNOMINMAX)
endif(MSVC)
if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=c++11)
endif()

target_link_libraries (revPlayer LINK_PUBLIC rev)