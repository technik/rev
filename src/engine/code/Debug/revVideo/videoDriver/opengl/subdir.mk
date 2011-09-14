################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../revVideo/videoDriver/opengl/videoDriverOpenGL.cpp 

OBJS += \
./revVideo/videoDriver/opengl/videoDriverOpenGL.o 

CPP_DEPS += \
./revVideo/videoDriver/opengl/videoDriverOpenGL.d 


# Each subdirectory must supply rules for building sources it contributes
revVideo/videoDriver/opengl/%.o: ../revVideo/videoDriver/opengl/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D_linux -D_DEBUG -I"/home/technik/programming/rev/revsdk/src/engine/code" -O0 -g3 -pedantic -pedantic-errors -Wall -Werror -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


