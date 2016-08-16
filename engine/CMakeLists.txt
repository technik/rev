################################################################################
# Rev project
################################################################################
cmake_minimum_required (VERSION 2.6)
project(engine)

# Collect all sources
file(GLOB_RECURSE PROJECT_SOURCE_FILES "src/*.cpp" "src/*.h" "src/*.inl")
set(PROJECT_SOURCE_FILES ${PROJECT_SOURCE_FILES} ${PROJECT_SOURCE_DIR}/lib/tinyxml2/tinyxml2.cpp) # Tiny xml is so small that we can directly embed it like this.

# Clasify sources according to folder structure
# This macro is courtesy of http://www.cmake.org/pipermail/cmake/2013-November/056336.html
macro(GroupSources curdir)
	file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir}
		${PROJECT_SOURCE_DIR}/${curdir}/*)
	foreach(child ${children})
		if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})
			GroupSources(${curdir}/${child})
		else()
			string(REPLACE "/" "\\" groupname ${curdir})
			source_group(${groupname} FILES
				${PROJECT_SOURCE_DIR}/${curdir}/${child})
		endif()
	endforeach()
endmacro()

GroupSources(src)

# Visual studio specifics
if(MSVC)
	add_definitions(-DNOMINMAX)
endif(MSVC)
if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=c++11)
endif()

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/glew ${CMAKE_CURRENT_SOURCE_DIR}/engine/lib)
add_library(rev STATIC ${PROJECT_SOURCE_FILES})
target_include_directories (rev PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/lib)
if(UNIX)
	find_package(GLEW)
	target_include_directories(rev PUBLIC ${GLEW_INCLUDE_DIRS})
	find_package(OpenGL)
	find_package(Threads REQUIRED)
	target_link_libraries(rev ${CMAKE_THREAD_LIBS_INIT})
endif(UNIX)
if(WIN32)
	find_library(GLEW_LIBRARIES glew32 ${CMAKE_CURRENT_SOURCE_DIR}/lib/glew)
	set(OPENGL_LIBRARIES opengl32)
endif(WIN32)
target_link_libraries (rev LINK_PUBLIC ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES})

# --- Bullet configuration ---
SET(USE_GRAPHICAL_BENCHMARK OFF CACHE BOOL "")
#SET(BUILD_SHARED_LIBS "Use shared libraries" OFF)
SET(BUILD_CPU_DEMOS OFF CACHE BOOL "")
SET(USE_GLUT OFF CACHE BOOL "")
SET(BUILD_BULLET3 ON CACHE BOOL "")
set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "")
SET(BUILD_OPENGL3_DEMOS OFF CACHE BOOL "")
set(BUILD_EXTRAS OFF CACHE BOOL "")
set(BUILD_UNIT_TESTS OFF CACHE BOOL "")
set(USE_MSVC_RUNTIME_LIBRARY_DLL ON CACHE BOOL "")
add_subdirectory(lib/bullet)
add_subdirectory(lib/vhacd)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "")
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "")
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "")
add_subdirectory(lib/assimp)

target_include_directories (rev PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/bullet/src)
target_include_directories (rev PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/vhacd/inc)
target_include_directories (rev PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/vhacd/public)
target_include_directories (rev PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/include)
set_target_properties(BulletDynamics PROPERTIES FOLDER "libs")
set_target_properties(Bullet3Common PROPERTIES FOLDER "libs")
set_target_properties(BulletCollision PROPERTIES FOLDER "libs")
set_target_properties(Bullet3Collision PROPERTIES FOLDER "libs")
set_target_properties(Bullet3Dynamics PROPERTIES FOLDER "libs")
set_target_properties(Bullet3Geometry PROPERTIES FOLDER "libs")
set_target_properties(LinearMath PROPERTIES FOLDER "libs")
set_target_properties(Bullet2FileLoader PROPERTIES FOLDER "libs")
set_target_properties(assimp PROPERTIES FOLDER "libs")
target_link_libraries(rev LINK_PUBLIC BulletDynamics Bullet3Common BulletCollision LinearMath)
target_link_libraries(rev LINK_PUBLIC VHACD_LIB)
target_link_libraries(rev LINK_PUBLIC assimp)