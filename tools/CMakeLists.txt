################################################################################
# Rev project
# Sample project that creates a simple scene graph
################################################################################
cmake_minimum_required (VERSION 3.10)
project(revTools)

include(../cmake/common.cmake)

#environment probes tool
file(GLOB_RECURSE PROBE_TOOL "probe/*.cpp" "probe/*.h")
include_directories("../include")
include_directories("../engine/src")
add_executable(probe ${PROBE_TOOL})
set_target_properties(probe PROPERTIES FOLDER "tools")
target_link_libraries (probe LINK_PRIVATE revCore revGfx)
