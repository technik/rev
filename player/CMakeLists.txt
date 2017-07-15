################################################################################
# Rev project
# Sample project that uses God of War shadows technique
################################################################################
cmake_minimum_required (VERSION 2.6)
project(revPlayer)

include(../cmake/common.cmake)

if(REV_WITH_HEADTRACKING)
#OpenCV
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
endif(REV_WITH_HEADTRACKING)

file(GLOB_RECURSE PLAYER_SOURCE_FILES "*.cpp" "*.h" "*.inl")
GroupSources(./ player)
add_executable(revPlayer ${PLAYER_SOURCE_FILES})

target_link_libraries (revPlayer LINK_PUBLIC rev ${OpenCV_LIBS})