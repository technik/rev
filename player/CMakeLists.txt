################################################################################
# Rev project
# Sample project that uses God of War shadows technique
################################################################################
cmake_minimum_required (VERSION 3.10)
project(revPlayer)

include(../cmake/common.cmake)

file(GLOB_RECURSE PLAYER_SOURCE_FILES "src/*.cpp" "src/*.h" "src/*.inl")
GroupSources(src player)
add_executable(revPlayer ${PLAYER_SOURCE_FILES})

target_link_libraries (revPlayer LINK_PUBLIC rev)