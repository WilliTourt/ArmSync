################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../src/FreeRTOS/FreeRTOS-Kernel/portable/GCC/ARM_CRx_MPU/mpu_wrappers_v2_asm.S \
../src/FreeRTOS/FreeRTOS-Kernel/portable/GCC/ARM_CRx_MPU/portASM.S 

C_SRCS += \
../src/FreeRTOS/FreeRTOS-Kernel/portable/GCC/ARM_CRx_MPU/port.c 

C_DEPS += \
./src/FreeRTOS/FreeRTOS-Kernel/portable/GCC/ARM_CRx_MPU/port.d 

OBJS += \
./src/FreeRTOS/FreeRTOS-Kernel/portable/GCC/ARM_CRx_MPU/mpu_wrappers_v2_asm.o \
./src/FreeRTOS/FreeRTOS-Kernel/portable/GCC/ARM_CRx_MPU/port.o \
./src/FreeRTOS/FreeRTOS-Kernel/portable/GCC/ARM_CRx_MPU/portASM.o 

SREC += \
ArmSync_Solution_CPU0.srec 

S_UPPER_DEPS += \
./src/FreeRTOS/FreeRTOS-Kernel/portable/GCC/ARM_CRx_MPU/mpu_wrappers_v2_asm.d \
./src/FreeRTOS/FreeRTOS-Kernel/portable/GCC/ARM_CRx_MPU/portASM.d 

MAP += \
ArmSync_Solution_CPU0.map 


# Each subdirectory must supply rules for building sources it contributes
src/FreeRTOS/FreeRTOS-Kernel/portable/GCC/ARM_CRx_MPU/%.o: ../src/FreeRTOS/FreeRTOS-Kernel/portable/GCC/ARM_CRx_MPU/%.S
	$(file > $@.in,-mthumb -mfloat-abi=hard -mcpu=cortex-m85+nopacbti -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -x assembler-with-cpp -D_RENESAS_RA_ -D_RA_CORE=CPU0 -D_RA_ORDINAL=1 -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra_cfg/fsp_cfg/bsp" -I"." -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra_gen" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra_cfg/fsp_cfg" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/arm/CMSIS_6/CMSIS/Core/Include" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/fsp/inc" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/fsp/inc/api" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/fsp/inc/instances" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"
src/FreeRTOS/FreeRTOS-Kernel/portable/GCC/ARM_CRx_MPU/%.o: ../src/FreeRTOS/FreeRTOS-Kernel/portable/GCC/ARM_CRx_MPU/%.c
	$(file > $@.in,-mthumb -mfloat-abi=hard -mcpu=cortex-m85+nopacbti -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RENESAS_RA_ -D_RA_CORE=CPU0 -D_RA_ORDINAL=1 -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra_cfg/fsp_cfg/bsp" -I"." -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra_gen" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra_cfg/fsp_cfg" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/arm/CMSIS_6/CMSIS/Core/Include" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/fsp/inc" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/fsp/inc/api" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/fsp/inc/instances" -std=c99 -Wno-stringop-overflow -Wno-format-truncation -flax-vector-conversions --param=min-pagesize=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

