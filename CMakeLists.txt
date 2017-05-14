################################################################################
# Rev project
# Sample project that creates a simple scene graph
################################################################################
cmake_minimum_required (VERSION 2.6)
project(revAll)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
add_subdirectory(engine)
add_subdirectory(tools)
add_subdirectory(samples/VRSample)