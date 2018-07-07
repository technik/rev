add_executable(shaderTest shader_test.cpp)
set_target_properties(shaderTest PROPERTIES FOLDER test)
add_test(shader_unit_test shaderTest)