################################################################################
# Rev project
# Sample project that creates a simple scene graph
################################################################################
cmake_minimum_required (VERSION 2.6)
project(revTools)

include(../cmake/common.cmake)

# libraries
find_package(assimp REQUIRED)
link_directories(${ASSIMP_LIBRARY_DIRS})

# --- tool folders ---
# shader processor
if(REV_WITH_OPENGL)
	file(GLOB_RECURSE SHDPROC_SOURCE_FILES "shaderProcessor/*.cpp" "shaderProcessor/*.h" "shaderProcessor/*.inl")
	add_executable(shaderProcessor ${SHDPROC_SOURCE_FILES})
	set_target_properties(shaderProcessor PROPERTIES FOLDER "tools")
endif()