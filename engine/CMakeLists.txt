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
# Collect all sources
file(GLOB_RECURSE PROJECT_SOURCE_FILES "src/*.cpp" "src/*.h" "src/*.inl")
GroupSources(src rev)
add_library(rev STATIC ${PROJECT_SOURCE_FILES})
target_include_directories(rev PRIVATE "../include")
if(UNIX) # Unix threads require pthread
	find_package(Threads REQUIRED)
	target_link_libraries(rev ${CMAKE_THREAD_LIBS_INIT})
endif(UNIX)
if(MSVC) # Windows sockets
	target_link_libraries(rev LINK_PUBLIC Ws2_32 Mswsock)
endif(MSVC)

# ---- More OpenGL ----
add_library(glew lib/glew/glew.c lib/glew/glewinfo.c lib/glew/visualinfo.c)
target_compile_definitions(glew PRIVATE -DGLEW_STATIC)
target_include_directories (glew PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)
#find_library(GLEW glew32 HINTS ${CMAKE_CURRENT_SOURCE_DIR}/lib/glew/x86/)
#target_link_libraries (rev LINK_PUBLIC ${GLEW})

target_include_directories (rev PUBLIC ${OPENGL_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/lib)
target_link_libraries (rev LINK_PUBLIC ${OPENGL_gl_LIBRARY} glew)

#find_library(FREEIMAGE_LIB FreeImage ${CMAKE_CURRENT_SOURCE_DIR}/lib/freeImage/)
#target_link_libraries (rev LINK_PUBLIC ${OPENGL_gl_LIBRARY} ${FREEIMAGE_LIB} cjson )
