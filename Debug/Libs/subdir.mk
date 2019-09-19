################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/denis/workspace/SDM/Libs/CRC16.c \
/home/denis/workspace/SDM/Libs/Serial.c \
/home/denis/workspace/SDM/Libs/SysConfig.c \
/home/denis/workspace/SDM/Libs/modbus-data.c \
/home/denis/workspace/SDM/Libs/modbus-rtu.c \
/home/denis/workspace/SDM/Libs/modbus-tcp.c \
/home/denis/workspace/SDM/Libs/modbus.c 

OBJS += \
./Libs/CRC16.o \
./Libs/Serial.o \
./Libs/SysConfig.o \
./Libs/modbus-data.o \
./Libs/modbus-rtu.o \
./Libs/modbus-tcp.o \
./Libs/modbus.o 

C_DEPS += \
./Libs/CRC16.d \
./Libs/Serial.d \
./Libs/SysConfig.d \
./Libs/modbus-data.d \
./Libs/modbus-rtu.d \
./Libs/modbus-tcp.d \
./Libs/modbus.d 


# Each subdirectory must supply rules for building sources it contributes
Libs/CRC16.o: /home/denis/workspace/SDM/Libs/CRC16.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/denis/workspace/SDM/Libs -I/home/denis/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/arm-linux-gnueabihf/libc/usr/include -O0 -march=armv7-a -fno-tree-vectorize -mthumb-interwork -mfloat-abi=hard -mtune=cortex-a9 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Libs/Serial.o: /home/denis/workspace/SDM/Libs/Serial.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/denis/workspace/SDM/Libs -I/home/denis/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/arm-linux-gnueabihf/libc/usr/include -O0 -march=armv7-a -fno-tree-vectorize -mthumb-interwork -mfloat-abi=hard -mtune=cortex-a9 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Libs/SysConfig.o: /home/denis/workspace/SDM/Libs/SysConfig.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/denis/workspace/SDM/Libs -I/home/denis/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/arm-linux-gnueabihf/libc/usr/include -O0 -march=armv7-a -fno-tree-vectorize -mthumb-interwork -mfloat-abi=hard -mtune=cortex-a9 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Libs/modbus-data.o: /home/denis/workspace/SDM/Libs/modbus-data.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/denis/workspace/SDM/Libs -I/home/denis/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/arm-linux-gnueabihf/libc/usr/include -O0 -march=armv7-a -fno-tree-vectorize -mthumb-interwork -mfloat-abi=hard -mtune=cortex-a9 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Libs/modbus-rtu.o: /home/denis/workspace/SDM/Libs/modbus-rtu.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/denis/workspace/SDM/Libs -I/home/denis/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/arm-linux-gnueabihf/libc/usr/include -O0 -march=armv7-a -fno-tree-vectorize -mthumb-interwork -mfloat-abi=hard -mtune=cortex-a9 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Libs/modbus-tcp.o: /home/denis/workspace/SDM/Libs/modbus-tcp.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/denis/workspace/SDM/Libs -I/home/denis/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/arm-linux-gnueabihf/libc/usr/include -O0 -march=armv7-a -fno-tree-vectorize -mthumb-interwork -mfloat-abi=hard -mtune=cortex-a9 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Libs/modbus.o: /home/denis/workspace/SDM/Libs/modbus.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/home/denis/workspace/SDM/Libs -I/home/denis/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/arm-linux-gnueabihf/libc/usr/include -O0 -march=armv7-a -fno-tree-vectorize -mthumb-interwork -mfloat-abi=hard -mtune=cortex-a9 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


