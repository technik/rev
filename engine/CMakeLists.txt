################################################################################
# Rev project
################################################################################
cmake_minimum_required (VERSION 3.10)
project(engine)

include(../cmake/common.cmake)

#######################################################################
####### Create separate libraries for each module of the engine #######
# Core
file(GLOB_RECURSE CORE_SOURCE_FILES "src/core/*.cpp" "src/core/*.h" "src/core/*.inl")
GroupSources(src/core src)
add_library(revCore STATIC ${CORE_SOURCE_FILES})
target_include_directories (revCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src )
set_target_properties(revCore PROPERTIES FOLDER rev)

# Input
file(GLOB_RECURSE INPUT_SOURCE_FILES "src/input/*.cpp" "src/input/*.h" "src/input/*.inl")
GroupSources(src/math src)
add_library(revInput STATIC ${INPUT_SOURCE_FILES})
target_include_directories (revInput PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src )
set_target_properties(revInput PROPERTIES FOLDER rev)

# Math
file(GLOB_RECURSE MATH_SOURCE_FILES "src/math/*.cpp" "src/math/*.h" "src/core/*.inl")
GroupSources(src/math src)
add_library(revMath STATIC ${MATH_SOURCE_FILES})
target_include_directories (revMath PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src )
set_target_properties(revMath PROPERTIES FOLDER rev)

# graphics
# ImGui source
set(ImGui_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/)
set(ImGui_SRC ${ImGui_ROOT}imgui.cpp ${ImGui_ROOT}imgui_demo.cpp ${ImGui_ROOT}imgui_draw.cpp ${ImGui_ROOT}imgui_tables.cpp ${ImGui_ROOT}imgui_widgets.cpp)
file(GLOB_RECURSE GRAPHICS_SOURCE_FILES "src/graphics/*.cpp" "src/graphics/*.h" "src/graphics/*.inl")
GroupSources(src/graphics src)
source_group("lib/imgui" FILES ${ImGui_SRC})
add_library(revGfx STATIC ${GRAPHICS_SOURCE_FILES} ${ImGui_SRC})
target_include_directories(revGfx PRIVATE "../include" ${CMAKE_CURRENT_SOURCE_DIR}/lib)
target_include_directories (revGfx PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(revGfx LINK_PRIVATE revMath revCore)
set_target_properties(revGfx PROPERTIES FOLDER rev)
target_include_directories(revGfx PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(revGfx LINK_PRIVATE ${Vulkan_LIBRARY})

# game
file(GLOB_RECURSE GAME_SOURCE_FILES "src/game/*.cpp" "src/game/*.h" "src/game/*.inl")
GroupSources(src/game src)
add_library(revGame STATIC ${GAME_SOURCE_FILES})
target_include_directories(revGame PRIVATE "../include" ${CMAKE_CURRENT_SOURCE_DIR}/lib)
target_include_directories (revGame PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src )
target_include_directories (revGame PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include )
target_link_libraries(revGame LINK_PUBLIC revCore revMath revInput revGfx)
set_target_properties(revGame PROPERTIES FOLDER rev)
