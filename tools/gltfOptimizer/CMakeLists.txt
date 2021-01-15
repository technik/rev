cmake_minimum_required (VERSION 3.10)
project(gltfOptimizer)

set(OPTIMIZER_SRC src/main.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_executable(gltfOptimizer ${OPTIMIZER_SRC})

set_target_properties( gltfOptimizer PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")