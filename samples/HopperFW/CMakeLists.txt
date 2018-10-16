################################################################################
# Rev project
# Sample project for a Minecraft-like game using the vulkan backend
################################################################################
cmake_minimum_required (VERSION 3.10)
project(HopperFW)

include(../../cmake/common.cmake)

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "src/*.h" "src/*.inl")
GroupSources(src HopperFW)
add_executable(HopperFW ${SOURCE_FILES})

set_target_properties(HopperFW PROPERTIES FOLDER "samples")
target_link_libraries (HopperFW LINK_PUBLIC revCore revMath revGfx revInput revGame)