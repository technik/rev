add_executable(shaderTest shader_test.cpp ../../../engine/src/graphics/driver/shaderProcessor.cpp ../../../engine/src/core/platform/filesystem/file.cpp)
set_target_properties(shaderTest PROPERTIES FOLDER test)
add_test(shader_unit_test shaderTest)