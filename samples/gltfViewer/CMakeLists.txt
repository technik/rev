################################################################################
# Rev project
# Sample project for a Minecraft-like game using the vulkan backend
################################################################################
cmake_minimum_required (VERSION 3.10)
project(GltfViewer)

include(../../cmake/common.cmake)

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "src/*.h" "src/*.inl")
GroupSources(src GltfViewer)
add_executable(GltfViewer ${SOURCE_FILES})
# Visual studio specifics

set_target_properties(GltfViewer PROPERTIES FOLDER "samples")
target_link_libraries (GltfViewer LINK_PUBLIC revCore revMath revGfx revGame revInput)