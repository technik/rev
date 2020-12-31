################################################################################
# Rev project
################################################################################
cmake_minimum_required (VERSION 2.6)
project(engine)

include(../cmake/common.cmake)

# ---- OpenGL ----
if(REV_WITH_OPENGL)
	find_package(OpenGL REQUIRED)
	add_definitions(-DOPENGL_45)
endif()

find_package(Vulkan REQUIRED)

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
file(GLOB_RECURSE GRAPHICS_SOURCE_FILES "src/graphics/*.cpp" "src/graphics/*.h" "src/graphics/*.inl")
GroupSources(src/graphics src)
add_library(revGfx STATIC ${GRAPHICS_SOURCE_FILES})
target_include_directories(revGfx PRIVATE "../include" ${CMAKE_CURRENT_SOURCE_DIR}/lib)
target_include_directories (revGfx PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src )
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

#######################################################################

if(UNIX) # Unix threads require pthread
	find_package(Threads REQUIRED)
	target_link_libraries(revCore ${CMAKE_THREAD_LIBS_INIT})
endif(UNIX)
if(MSVC) # Windows sockets
	target_link_libraries(revCore LINK_PUBLIC Ws2_32 Mswsock)
endif(MSVC)

# ---- More OpenGL ----
add_library(glew lib/glew/glew.c lib/glew/glewinfo.c lib/glew/visualinfo.c)
target_compile_definitions(glew PRIVATE -DGLEW_STATIC)
target_include_directories (glew PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)

target_include_directories (revGfx PUBLIC ${OPENGL_INCLUDE_DIR})
target_link_libraries (revGfx LINK_PUBLIC ${OPENGL_gl_LIBRARY} glew)
