################################################################################
# Rev project
# Procedural generation sandbox
################################################################################
cmake_minimum_required (VERSION 3.10)
project(Pluto)

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

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "src/*.h" "src/*.inl")
GroupSources(src src)
add_executable(Pluto ${SOURCE_FILES} ${GLSL_SOURCES})
set_target_properties( Pluto PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
target_link_libraries (Pluto LINK_PUBLIC revCore revMath revGfx revGame revInput ImGui)

# Visual studio specifics
set_target_properties(Pluto PROPERTIES FOLDER "samples")