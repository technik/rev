add_executable(sceneTest scene_test.cpp ../../../engine/src/game/scene/scene.cpp)
set_target_properties(sceneTest PROPERTIES FOLDER test)
add_test(scene_unit_test sceneTest)