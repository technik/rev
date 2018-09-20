################################################################################
# Rev project
# Sample project for a Minecraft-like game using the vulkan backend
################################################################################
# A doom 3 map loader
cmake_minimum_required (VERSION 3.10)
project(Doom3)

include(../../cmake/common.cmake)

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "src/*.h" "src/*.inl")
GroupSources(src Doom3)
add_executable(Doom3 ${SOURCE_FILES})

set_target_properties(Doom3 PROPERTIES FOLDER "samples")
target_link_libraries (Doom3 LINK_PUBLIC rev)