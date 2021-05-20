################################################################################
# Rev project
# Light probe processing tool
# And brdf study
################################################################################

add_executable(probeTool
	main.cpp
	Heitz/MicrosurfaceScattering.h
	Heitz/MicrosurfaceScattering.cpp)
target_include_directories(probeTool PUBLIC ../../engine/src)
target_include_directories(probeTool PUBLIC ../../include)
target_include_directories(probeTool PUBLIC ../../include/glm)
target_link_libraries(probeTool LINK_PUBLIC revCore revGfx revMath)
set_target_properties(probeTool PROPERTIES FOLDER "tools")