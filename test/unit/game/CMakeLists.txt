add_executable(sceneTest scene_test.cpp ../../../engine/src/game/scene/scene.cpp ../../../engine/src/game/scene/sceneNode.cpp ../../../engine/src/game/scene/component.cpp)
target_link_libraries (sceneTest LINK_PUBLIC ${OPENGL_gl_LIBRARY} glew)
set_target_properties(sceneTest PROPERTIES FOLDER test)
add_test(scene_unit_test sceneTest)