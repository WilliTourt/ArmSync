################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ra/fsp/src/bsp/mcu/ra8p1/bsp_linker.c 

SREC += \
ArmSync_Solution_CPU1.srec 

C_DEPS += \
./ra/fsp/src/bsp/mcu/ra8p1/bsp_linker.d 

OBJS += \
./ra/fsp/src/bsp/mcu/ra8p1/bsp_linker.o 

MAP += \
ArmSync_Solution_CPU1.map 


# Each subdirectory must supply rules for building sources it contributes
ra/fsp/src/bsp/mcu/ra8p1/%.o: ../ra/fsp/src/bsp/mcu/ra8p1/%.c
	$(file > $@.in,-mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RENESAS_RA_ -D_RA_CORE=CPU1 -D_RA_ORDINAL=2 -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU1/ra_cfg/fsp_cfg/bsp" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU1/src" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU1/src/shared" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU1/src/app/drivers" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU1/src/app/gripper" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU1/src/app/ipc_isr" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU1/src/app/motor" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU1/src/app/safety" -I"." -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU1/ra_gen" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU1/ra_cfg/fsp_cfg" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU1/ra/arm/CMSIS_6/CMSIS/Core/Include" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU1/src" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU1/ra/fsp/inc" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU1/ra/fsp/inc/api" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU1/ra/fsp/inc/instances" -std=c99 -Wno-stringop-overflow -Wno-format-truncation --param=min-pagesize=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

