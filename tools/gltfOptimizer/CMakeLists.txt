cmake_minimum_required (VERSION 3.10)
project(gltfOptimizer)

set(OPTIMIZER_SRC src/main.cpp)
add_executable(gltfOptimizer ${OPTIMIZER_SRC})
target_include_directories(gltfOptimizer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include,
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../engine/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../engine/src/game/scene/gltf)
target_link_libraries(gltfOptimizer LINK_PUBLIC revCore)

set_target_properties( gltfOptimizer PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")