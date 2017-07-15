################################################################################
# Rev project
# Sample project that uses God of War shadows technique
################################################################################
cmake_minimum_required (VERSION 2.6)
project(math_tests)

set(EnginePath ../../../engine/src)
set(MathPath ${EnginePath}/math)

include_directories(${MathPath})

add_executable(kalmanFilter kalman.cpp ${MathPath}/filters.cpp)
set_target_properties(kalmanFilter PROPERTIES FOLDER test)
add_test(kalman_unit_test kalmanFilter)