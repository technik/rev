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

file(GLOB_RECURSE CJSON_SOURCE_FILES "lib/cjson/*.cpp" "lib/cjson/*.h" "lib/cjson/*.inl")
add_library(cjson STATIC ${CJSON_SOURCE_FILES})
target_include_directories(cjson PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/cjson)
set_target_properties(cjson PROPERTIES FOLDER "libs")

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
find_library(FREEIMAGE_LIB FreeImage ${CMAKE_CURRENT_SOURCE_DIR}/lib/freeImage/)

target_link_libraries (rev LINK_PUBLIC ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} ${FREEIMAGE_LIB} cjson)

if(MSVC)
	target_link_libraries(rev LINK_PUBLIC Ws2_32 Mswsock)
endif(MSVC)