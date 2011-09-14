################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../revCore/time/time.cpp 

OBJS += \
./revCore/time/time.o 

CPP_DEPS += \
./revCore/time/time.d 


# Each subdirectory must supply rules for building sources it contributes
revCore/time/%.o: ../revCore/time/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D_linux -D_DEBUG -I"/home/technik/programming/rev/revsdk/src/engine/code" -O0 -g3 -pedantic -pedantic-errors -Wall -Werror -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


