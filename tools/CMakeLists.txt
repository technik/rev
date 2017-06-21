################################################################################
# Rev project
# Sample project that creates a simple scene graph
################################################################################
cmake_minimum_required (VERSION 2.6)
project(revTools)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

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

# libraries
find_package(assimp REQUIRED)
link_directories(${ASSIMP_LIBRARY_DIRS})

# tool folders
file(GLOB_RECURSE MDLPROC_SOURCE_FILES "modelProcessor/*.cpp" "modelProcessor/*.h" "modelProcessor/*.inl")
add_executable(modelProcessor ${MDLPROC_SOURCE_FILES})
set_target_properties(modelProcessor PROPERTIES FOLDER "tools")

target_include_directories(modelProcessor PUBLIC ${ASSIMP_INCLUDE_DIRS})
target_link_libraries(modelProcessor LINK_PUBLIC ${ASSIMP_LIBRARIES} zlibstatic)
target_link_libraries(modelProcessor LINK_PUBLIC rev)

# shader processor
file(GLOB_RECURSE SHDPROC_SOURCE_FILES "shaderProcessor/*.cpp" "shaderProcessor/*.h" "shaderProcessor/*.inl")
add_executable(shaderProcessor ${SHDPROC_SOURCE_FILES})
set_target_properties(shaderProcessor PROPERTIES FOLDER "tools")

target_link_libraries(shaderProcessor LINK_PUBLIC rev)