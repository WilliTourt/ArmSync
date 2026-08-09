################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/app/ik/src/bstv.c \
../src/app/ik/src/build_info.c \
../src/app/ik/src/chain.c \
../src/app/ik/src/ik.c \
../src/app/ik/src/log_static.c \
../src/app/ik/src/memory.c \
../src/app/ik/src/quat_static.c \
../src/app/ik/src/retcodes.c \
../src/app/ik/src/solver_static.c \
../src/app/ik/src/transform_chains.c \
../src/app/ik/src/transform_tree.c \
../src/app/ik/src/util.c \
../src/app/ik/src/vec3_static.c \
../src/app/ik/src/vector.c 

SREC += \
ArmSync_Solution_CPU0.srec 

C_DEPS += \
./src/app/ik/src/bstv.d \
./src/app/ik/src/build_info.d \
./src/app/ik/src/chain.d \
./src/app/ik/src/ik.d \
./src/app/ik/src/log_static.d \
./src/app/ik/src/memory.d \
./src/app/ik/src/quat_static.d \
./src/app/ik/src/retcodes.d \
./src/app/ik/src/solver_static.d \
./src/app/ik/src/transform_chains.d \
./src/app/ik/src/transform_tree.d \
./src/app/ik/src/util.d \
./src/app/ik/src/vec3_static.d \
./src/app/ik/src/vector.d 

OBJS += \
./src/app/ik/src/bstv.o \
./src/app/ik/src/build_info.o \
./src/app/ik/src/chain.o \
./src/app/ik/src/ik.o \
./src/app/ik/src/log_static.o \
./src/app/ik/src/memory.o \
./src/app/ik/src/quat_static.o \
./src/app/ik/src/retcodes.o \
./src/app/ik/src/solver_static.o \
./src/app/ik/src/transform_chains.o \
./src/app/ik/src/transform_tree.o \
./src/app/ik/src/util.o \
./src/app/ik/src/vec3_static.o \
./src/app/ik/src/vector.o 

MAP += \
ArmSync_Solution_CPU0.map 


# Each subdirectory must supply rules for building sources it contributes
src/app/ik/src/%.o: ../src/app/ik/src/%.c
	$(file > $@.in,-mthumb -mfloat-abi=hard -mcpu=cortex-m85+nopacbti -O1 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RENESAS_RA_ -D_RA_CORE=CPU0 -D_RA_ORDINAL=1 -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra_cfg/fsp_cfg/bsp" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/ik/include/public" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/ik/include/private" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/utils" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/drivers" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks/IKTask" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks/UITask" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks/CPUCommTask" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks/UartRecvTask" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks/NormalizeTask" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/FreeRTOS/FreeRTOS-Cpp/include" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/FreeRTOS/FreeRTOS-Kernel/portable/RM_FSP_PORT" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/FreeRTOS/FreeRTOS-Kernel/include" -I"." -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra_gen" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra_cfg/fsp_cfg" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/arm/CMSIS_6/CMSIS/Core/Include" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/fsp/inc" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/fsp/inc/api" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/fsp/inc/instances" -std=c11 -Wno-stringop-overflow -Wno-format-truncation -flax-vector-conversions --param=min-pagesize=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

