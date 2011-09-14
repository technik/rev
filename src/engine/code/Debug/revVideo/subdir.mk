################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../revVideo/video.cpp 

OBJS += \
./revVideo/video.o 

CPP_DEPS += \
./revVideo/video.d 


# Each subdirectory must supply rules for building sources it contributes
revVideo/%.o: ../revVideo/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D_linux -D_DEBUG -I"/home/technik/programming/rev/revsdk/src/engine/code" -O0 -g3 -pedantic -pedantic-errors -Wall -Werror -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


