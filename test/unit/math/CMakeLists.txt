add_executable(algebraTest algebra_test.cpp)
set_target_properties(algebraTest PROPERTIES FOLDER test/math)
add_test(algebra_unit_test algebraTest)

add_executable(geometryTest geometry_test.cpp)
set_target_properties(geometryTest PROPERTIES FOLDER test/math)
add_test(geometry_unit_test geometryTest)