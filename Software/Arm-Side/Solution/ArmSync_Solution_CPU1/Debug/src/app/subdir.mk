################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/app/ElegantDebug.cpp 

SREC += \
ArmSync_Solution_CPU1.srec 

OBJS += \
./src/app/ElegantDebug.o 

MAP += \
ArmSync_Solution_CPU1.map 

CPP_DEPS += \
./src/app/ElegantDebug.d 


# Each subdirectory must supply rules for building sources it contributes
src/app/%.o: ../src/app/%.cpp
	$(file > $@.in,-mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RA_CORE=CPU1 -D_RA_ORDINAL=2 -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU1/src/app/arm" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU1/src/app" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU1/src/app/ipc" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU1/ra_cfg/fsp_cfg/bsp" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU1/src" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU1/src/app/drivers" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU1/src/app/gripper" -I"." -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU1/ra_gen" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU1/ra_cfg/fsp_cfg" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU1/ra/arm/CMSIS_6/CMSIS/Core/Include" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU1/src" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU1/ra/fsp/inc" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU1/ra/fsp/inc/api" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU1/ra/fsp/inc/instances" -std=c++11 -fabi-version=0 -Wno-stringop-overflow -Wno-format-truncation --param=min-pagesize=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c++ "$<")
	@echo Building file: $< && arm-none-eabi-g++ @"$@.in"

