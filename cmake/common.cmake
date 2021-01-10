# --- common configuration and useful cmake macros used across the engine
# Visual studio specifics
if(MSVC)
	add_definitions(-DNOMINMAX)
endif(MSVC)
if(CMAKE_COMPILER_IS_GNUCXX)
	add_definitions(-std=c++17)
endif()

# Clasify sources according to folder structure. Useful for having nice visual studio filters.
# This macro is derived from http://www.cmake.org/pipermail/cmake/2013-November/056336.html
macro(GroupSources curdir dirLabel)
	file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir}
		${PROJECT_SOURCE_DIR}/${curdir}/*)
	foreach(child ${children})
		if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})
			GroupSources(${curdir}/${child} ${dirLabel}/${child})
		else()
			string(REPLACE "/" "\\" groupname ${dirLabel})
			source_group(${groupname} FILES
				${PROJECT_SOURCE_DIR}/${curdir}/${child})
		endif()
	endforeach()
endmacro()

#####################################################################################
# Macro to add custom build for SPIR-V, with additional arbitrary glslangvalidator
# flags (run glslangvalidator --help for a list of possible flags).
# Inputs:
# _SOURCE can be more than one file (.vert + .frag)
# _OUTPUT is the .spv file, resulting from the linkage
# _FLAGS are the flags to add to the command line
# Outputs:
# SOURCE_LIST has _SOURCE appended to it
# OUTPUT_LIST has _OUTPUT appended to it
#
macro(_compile_GLSL_flags _SOURCE _OUTPUT _FLAGS SOURCE_LIST OUTPUT_LIST)
  LIST(APPEND ${SOURCE_LIST} ${_SOURCE})
  LIST(APPEND ${OUTPUT_LIST} ${_OUTPUT})
  if(GLSLANGVALIDATOR)
    set(_COMMAND ${GLSLANGVALIDATOR} ${_SOURCE} -o ${_OUTPUT} ${_FLAGS})
    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${_OUTPUT}
      COMMAND echo ${_COMMAND}
      COMMAND ${_COMMAND}
      MAIN_DEPENDENCY ${_SOURCE}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      )
  else(GLSLANGVALIDATOR)
    MESSAGE(WARNING "could not find GLSLANGVALIDATOR to compile shaders")
  endif(GLSLANGVALIDATOR)
endmacro()

#####################################################################################
# Macro to add custom build for SPIR-V, with debug information (glslangvalidator's -g flag)
# Inputs:
# _SOURCE can be more than one file (.vert + .frag)
# _OUTPUT is the .spv file, resulting from the linkage
# Outputs:
# SOURCE_LIST has _SOURCE appended to it
# OUTPUT_LIST has _OUTPUT appended to it
#
macro(_compile_GLSL _SOURCE _OUTPUT SOURCE_LIST OUTPUT_LIST)
  _compile_GLSL_flags(${_SOURCE} ${_OUTPUT} "-g" ${SOURCE_LIST} ${OUTPUT_LIST})
endmacro()

#####################################################################################
# Macro to add custom build for SPIR-V, without debug information
# Inputs:
# _SOURCE can be more than one file (.vert + .frag)
# _OUTPUT is the .spv file, resulting from the linkage
# Outputs:
# SOURCE_LIST has _SOURCE appended to it
# OUTPUT_LIST has _OUTPUT appended to it
#
macro(_compile_GLSL_no_debug_info _SOURCE _OUTPUT SOURCE_LIST OUTPUT_LIST)
  _compile_GLSL_flags(${_SOURCE} ${_OUTPUT} "" ${SOURCE_LIST} ${OUTPUT_LIST})
endmacro()