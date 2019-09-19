################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MainServer.c \
../Tabelas_GEMAP.c 

OBJS += \
./MainServer.o \
./Tabelas_GEMAP.o 

C_DEPS += \
./MainServer.d \
./Tabelas_GEMAP.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/denis/workspace/SDM/Libs -I/home/denis/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/arm-linux-gnueabihf/libc/usr/include -O0 -march=armv7-a -fno-tree-vectorize -mthumb-interwork -mfloat-abi=hard -mtune=cortex-a9 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


