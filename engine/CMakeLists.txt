################################################################################
# Rev project
################################################################################
cmake_minimum_required (VERSION 2.6)
project(rev)

# Collect all sources
file(GLOB_RECURSE PROJECT_SOURCE_FILES "src/*.cpp" "src/*.h" "src/*.inl")
list(APPEND ${PROJECT_SOURCE_FILES} lib/tinyxml2/tinyxml2.cpp) # Tiny xml is so small that we can directly embed it like this.

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

#target_include_directories (rev PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
#include_directories(src)

# Visual studio specifics
if(MSVC)
	add_definitions(-DNOMINMAX)
endif(MSVC)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/glew ${CMAKE_CURRENT_SOURCE_DIR}/engine/lib)
add_library(rev STATIC ${PROJECT_SOURCE_FILES})
target_include_directories (rev PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/lib)
find_library(GLEW glew32 ${CMAKE_CURRENT_SOURCE_DIR}/lib/glew)
target_link_libraries (rev ${GLEW} openGL32)