################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/app/ik/src/solver/base/constraint_base.c \
../src/app/ik/src/solver/base/effector_base.c \
../src/app/ik/src/solver/base/node_base.c \
../src/app/ik/src/solver/base/solver_base.c 

SREC += \
ArmSync_Solution_CPU0.srec 

C_DEPS += \
./src/app/ik/src/solver/base/constraint_base.d \
./src/app/ik/src/solver/base/effector_base.d \
./src/app/ik/src/solver/base/node_base.d \
./src/app/ik/src/solver/base/solver_base.d 

OBJS += \
./src/app/ik/src/solver/base/constraint_base.o \
./src/app/ik/src/solver/base/effector_base.o \
./src/app/ik/src/solver/base/node_base.o \
./src/app/ik/src/solver/base/solver_base.o 

MAP += \
ArmSync_Solution_CPU0.map 


# Each subdirectory must supply rules for building sources it contributes
src/app/ik/src/solver/base/%.o: ../src/app/ik/src/solver/base/%.c
	$(file > $@.in,-mthumb -mfloat-abi=hard -mcpu=cortex-m85+nopacbti -O1 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RENESAS_RA_ -D_RA_CORE=CPU0 -D_RA_ORDINAL=1 -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra_cfg/fsp_cfg/bsp" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks/MotionPlanningTask" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/ik/include/public" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/ik/include/private" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/utils" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks/IKTask" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks/UITask" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks/CPUCommTask" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks/UartRecvTask" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks/NormalizeTask" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks/FusionTask" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/FreeRTOS/FreeRTOS-Cpp/include" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/FreeRTOS/FreeRTOS-Kernel/portable/RM_FSP_PORT" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/FreeRTOS/FreeRTOS-Kernel/include" -I"." -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra_gen" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra_cfg/fsp_cfg" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/arm/CMSIS_6/CMSIS/Core/Include" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/fsp/inc" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/fsp/inc/api" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/fsp/inc/instances" -std=c11 -Wno-stringop-overflow -Wno-format-truncation -flax-vector-conversions --param=min-pagesize=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

