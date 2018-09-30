################################################################################
# Rev project
# Sample project for a Minecraft-like game using the vulkan backend
################################################################################
cmake_minimum_required (VERSION 3.10)
project(vulkraft)

include(../../cmake/common.cmake)

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "src/*.h" "src/*.inl")
GroupSources(src vulkraft)
add_executable(vulkraft ${SOURCE_FILES})
# Visual studio specifics

set_target_properties(vulkraft PROPERTIES FOLDER "samples")
target_link_libraries (vulkraft LINK_PUBLIC revCore revMath revGfx revInput)