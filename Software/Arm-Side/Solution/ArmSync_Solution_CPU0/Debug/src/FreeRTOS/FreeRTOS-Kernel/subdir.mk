################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/FreeRTOS/FreeRTOS-Kernel/croutine.c \
../src/FreeRTOS/FreeRTOS-Kernel/event_groups.c \
../src/FreeRTOS/FreeRTOS-Kernel/list.c \
../src/FreeRTOS/FreeRTOS-Kernel/queue.c \
../src/FreeRTOS/FreeRTOS-Kernel/stream_buffer.c \
../src/FreeRTOS/FreeRTOS-Kernel/tasks.c \
../src/FreeRTOS/FreeRTOS-Kernel/timers.c 

SREC += \
ArmSync_Solution_CPU0.srec 

C_DEPS += \
./src/FreeRTOS/FreeRTOS-Kernel/croutine.d \
./src/FreeRTOS/FreeRTOS-Kernel/event_groups.d \
./src/FreeRTOS/FreeRTOS-Kernel/list.d \
./src/FreeRTOS/FreeRTOS-Kernel/queue.d \
./src/FreeRTOS/FreeRTOS-Kernel/stream_buffer.d \
./src/FreeRTOS/FreeRTOS-Kernel/tasks.d \
./src/FreeRTOS/FreeRTOS-Kernel/timers.d 

OBJS += \
./src/FreeRTOS/FreeRTOS-Kernel/croutine.o \
./src/FreeRTOS/FreeRTOS-Kernel/event_groups.o \
./src/FreeRTOS/FreeRTOS-Kernel/list.o \
./src/FreeRTOS/FreeRTOS-Kernel/queue.o \
./src/FreeRTOS/FreeRTOS-Kernel/stream_buffer.o \
./src/FreeRTOS/FreeRTOS-Kernel/tasks.o \
./src/FreeRTOS/FreeRTOS-Kernel/timers.o 

MAP += \
ArmSync_Solution_CPU0.map 


# Each subdirectory must supply rules for building sources it contributes
src/FreeRTOS/FreeRTOS-Kernel/%.o: ../src/FreeRTOS/FreeRTOS-Kernel/%.c
	$(file > $@.in,-mthumb -mfloat-abi=hard -mcpu=cortex-m85+nopacbti -O1 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RENESAS_RA_ -D_RA_CORE=CPU0 -D_RA_ORDINAL=1 -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra_cfg/fsp_cfg/bsp" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/shared" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/utils" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/drivers" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks/IKTask" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks/CPUCommTask" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks/UartRecvTask" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/FreeRTOS/FreeRTOS-Cpp/include" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/FreeRTOS/FreeRTOS-Kernel/portable/RM_FSP_PORT" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/FreeRTOS/FreeRTOS-Kernel/include" -I"." -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra_gen" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra_cfg/fsp_cfg" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/arm/CMSIS_6/CMSIS/Core/Include" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/fsp/inc" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/fsp/inc/api" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/fsp/inc/instances" -std=c11 -Wno-stringop-overflow -Wno-format-truncation -flax-vector-conversions --param=min-pagesize=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

