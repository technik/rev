################################################################################
# Rev project
################################################################################
cmake_minimum_required (VERSION 3.10)
project(engine)

include(../cmake/common.cmake)

#######################################################################
####### Create separate libraries for each module of the engine #######
# Core
file(GLOB_RECURSE CORE_SOURCE_FILES "core/*.cpp" "core/*.h" "core/*.inl")
GroupSources(core src)
add_library(revCore STATIC ${CORE_SOURCE_FILES})
target_include_directories (revCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(revCore PROPERTIES FOLDER rev)

# Input
file(GLOB_RECURSE INPUT_SOURCE_FILES "input/*.cpp" "input/*.h" "input/*.inl")
GroupSources(math src)
add_library(revInput STATIC ${INPUT_SOURCE_FILES})
target_include_directories (revInput PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(revInput PROPERTIES FOLDER rev)

# Math
file(GLOB_RECURSE MATH_SOURCE_FILES "math/*.cpp" "math/*.h" "math/*.inl")
add_library(revMath STATIC ${MATH_SOURCE_FILES})
target_include_directories (revMath PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(revMath PROPERTIES FOLDER rev)

# graphics
file(GLOB_RECURSE GFX_SOURCE_FILES "gfx/*.cpp" "gfx/*.h" "gfx/*.inl")
GroupSources(gfx src)
add_library(revGfx STATIC ${GFX_SOURCE_FILES})
target_include_directories(revGfx PUBLIC ${Vulkan_INCLUDE_DIRS})
target_include_directories (revGfx PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} "../include")
target_link_libraries(revGfx LINK_PRIVATE revMath revCore ImGui)
target_link_libraries(revGfx LINK_PRIVATE ${Vulkan_LIBRARY})
set_target_properties(revGfx PROPERTIES FOLDER rev)

# game
file(GLOB_RECURSE GAME_SOURCE_FILES "game/*.cpp" "game/*.h" "game/*.inl")
GroupSources(game src)
add_library(revGame STATIC ${GAME_SOURCE_FILES})
target_include_directories(revGame PRIVATE "../include" ${CMAKE_CURRENT_SOURCE_DIR}/lib)
target_include_directories (revGame PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src )
target_include_directories (revGame PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_libraries(revGame LINK_PUBLIC revCore revMath revInput revGfx)
set_target_properties(revGame PROPERTIES FOLDER rev)

# Benchmarks
add_executable(matrixBenchmark benchmark/matrix.cpp)
target_include_directories (matrixBenchmark PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(matrixBenchmark LINK_PUBLIC benchmark::benchmark revMath)
set_target_properties(matrixBenchmark PROPERTIES FOLDER benchmarks)