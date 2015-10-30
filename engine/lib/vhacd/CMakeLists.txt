project(VHACD_LIB CXX C)

file(GLOB_RECURSE PROJECT_SOURCE_FILES "src/*.cpp" "src/*.h" "src/*.inl" "code/*.cc" "src/*.hh" "public/*.h")
file(GLOB_RECURSE PROJECT_HEADER_FILES "inc/*.h" "inc/*.inl")

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

set(LIB_TYPE "STATIC" CACHE STRING "STATIC, SHARED or MODULE?")
include_directories("inc" "public")
add_library(VHACD_LIB ${PROJECT_SOURCE_FILES} ${PROJECT_HEADER_FILES})