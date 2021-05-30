################################################################################
# Rev project
# Sample project for a Minecraft-like game using the vulkan backend
################################################################################
cmake_minimum_required (VERSION 3.10)
project(ShaderToy)

include(../../cmake/common.cmake)

UNSET(GLSL_SOURCES)
UNSET(SPV_OUTPUT)
file(GLOB_RECURSE GLSL_HEADER_FILES "shaders/*.h" "shaders/*.glsl")
file(GLOB_RECURSE GLSL_SOURCE_FILES
    "shaders/*.comp"
    "shaders/*.frag"
    "shaders/*.vert"
    "shaders/*.rchit"
    "shaders/*.rahit"
    "shaders/*.rmiss"
    "shaders/*.rgen"
    )
foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    _compile_GLSL(${GLSL} "./shaders/${FILE_NAME}.spv" GLSL_SOURCES SPV_OUTPUT)
endforeach(GLSL)

list(APPEND GLSL_SOURCES ${GLSL_HEADER_FILES})
source_group(shaders FILES ${GLSL_SOURCES})

add_executable(ShaderToy
	main.cpp
	ShaderToy.h
	ShaderToy.cpp
	${GLSL_SOURCES})
target_link_libraries (ShaderToy LINK_PUBLIC revCore revMath revGfx revInput revGame ImGui)

# Visual studio specifics
set_target_properties(ShaderToy PROPERTIES FOLDER "samples")