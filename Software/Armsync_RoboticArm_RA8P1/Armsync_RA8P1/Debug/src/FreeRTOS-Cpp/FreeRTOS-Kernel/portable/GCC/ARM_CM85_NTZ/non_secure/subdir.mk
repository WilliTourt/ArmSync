################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/GCC/ARM_CM85_NTZ/non_secure/mpu_wrappers_v2_asm.c \
../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/GCC/ARM_CM85_NTZ/non_secure/port.c \
../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/GCC/ARM_CM85_NTZ/non_secure/portasm.c 

SREC += \
Armsync_RA8P1.srec 

C_DEPS += \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/GCC/ARM_CM85_NTZ/non_secure/mpu_wrappers_v2_asm.d \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/GCC/ARM_CM85_NTZ/non_secure/port.d \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/GCC/ARM_CM85_NTZ/non_secure/portasm.d 

OBJS += \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/GCC/ARM_CM85_NTZ/non_secure/mpu_wrappers_v2_asm.o \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/GCC/ARM_CM85_NTZ/non_secure/port.o \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/GCC/ARM_CM85_NTZ/non_secure/portasm.o 

MAP += \
Armsync_RA8P1.map 


# Each subdirectory must supply rules for building sources it contributes
src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/GCC/ARM_CM85_NTZ/non_secure/%.o: ../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/GCC/ARM_CM85_NTZ/non_secure/%.c
	$(file > $@.in,-mthumb -mfloat-abi=hard -mcpu=cortex-m85+nopacbti -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RENESAS_RA_ -D_RA_CORE=CPU0 -D_RA_ORDINAL=1 -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_gen" -I../src/Components/Inc -I../src/Components/Tasks/Inc -I../src/FreeRTOS-Cpp/FreeRTOS-Kernel/include -I../src/FreeRTOS-Cpp/FreeRTOS-Cpp/include -I../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/GCC/ARM_CM85_NTZ/non_secure -I"." -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_cfg/fsp_cfg/bsp" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_cfg/fsp_cfg" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/src" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc/api" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc/instances" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/arm/CMSIS_6/CMSIS/Core/Include" -std=c99 -Wno-stringop-overflow -Wno-format-truncation -flax-vector-conversions --param=min-pagesize=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

