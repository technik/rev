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
# Cubemap processor

# model processor
file(GLOB_RECURSE MDLPROC_SOURCE_FILES "modelProcessor/*.cpp" "modelProcessor/*.h" "modelProcessor/*.inl")
add_executable(modelProcessor ${MDLPROC_SOURCE_FILES})
set_target_properties(modelProcessor PROPERTIES FOLDER "tools")

#find_package(zlib REQUIRED)
#target_link_libraries(modelProcessor LINK_PUBLIC zlib)

target_include_directories(modelProcessor PUBLIC ${ASSIMP_INCLUDE_DIRS})
target_link_libraries(modelProcessor LINK_PUBLIC ${ASSIMP_LIBRARIES} zlibstatic)
target_link_libraries(modelProcessor LINK_PUBLIC rev)

# shader processor
if(REV_WITH_OPENGL)
	file(GLOB_RECURSE SHDPROC_SOURCE_FILES "shaderProcessor/*.cpp" "shaderProcessor/*.h" "shaderProcessor/*.inl")
	add_executable(shaderProcessor ${SHDPROC_SOURCE_FILES})
	set_target_properties(shaderProcessor PROPERTIES FOLDER "tools")
endif()