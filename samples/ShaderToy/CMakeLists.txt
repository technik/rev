################################################################################
# Rev project
# Sample project for a Minecraft-like game using the vulkan backend
################################################################################
cmake_minimum_required (VERSION 3.10)
project(ShaderToy)

include(../../cmake/common.cmake)

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "src/*.h" "src/*.inl")
GroupSources(src ShaderToy)
add_executable(ShaderToy ${SOURCE_FILES})
# Visual studio specifics

set_target_properties(ShaderToy PROPERTIES FOLDER "samples")
target_link_libraries (ShaderToy LINK_PUBLIC rev)