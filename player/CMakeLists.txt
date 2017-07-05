################################################################################
# Rev project
# Sample project that uses God of War shadows technique
################################################################################
cmake_minimum_required (VERSION 2.6)
project(revPlayer)

if(REV_WITH_HEADTRACKING)
# Eigen
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

#OpenCV
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
endif()

add_executable(revPlayer main.cpp player.cpp player.h)
# Visual studio specifics
if(MSVC)
	add_definitions(-DNOMINMAX)
endif(MSVC)
if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=c++11)
endif()

target_link_libraries (revPlayer LINK_PUBLIC rev ${OpenCV_LIBS})