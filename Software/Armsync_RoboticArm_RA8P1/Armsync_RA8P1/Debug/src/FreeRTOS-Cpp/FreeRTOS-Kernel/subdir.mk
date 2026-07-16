################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/FreeRTOS-Cpp/FreeRTOS-Kernel/croutine.c \
../src/FreeRTOS-Cpp/FreeRTOS-Kernel/event_groups.c \
../src/FreeRTOS-Cpp/FreeRTOS-Kernel/list.c \
../src/FreeRTOS-Cpp/FreeRTOS-Kernel/queue.c \
../src/FreeRTOS-Cpp/FreeRTOS-Kernel/stream_buffer.c \
../src/FreeRTOS-Cpp/FreeRTOS-Kernel/tasks.c \
../src/FreeRTOS-Cpp/FreeRTOS-Kernel/timers.c 

SREC += \
Armsync_RA8P1.srec 

C_DEPS += \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/croutine.d \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/event_groups.d \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/list.d \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/queue.d \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/stream_buffer.d \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/tasks.d \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/timers.d 

OBJS += \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/croutine.o \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/event_groups.o \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/list.o \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/queue.o \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/stream_buffer.o \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/tasks.o \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/timers.o 

MAP += \
Armsync_RA8P1.map 


# Each subdirectory must supply rules for building sources it contributes
src/FreeRTOS-Cpp/FreeRTOS-Kernel/%.o: ../src/FreeRTOS-Cpp/FreeRTOS-Kernel/%.c
	$(file > $@.in,-mthumb -mfloat-abi=hard -mcpu=cortex-m85+nopacbti -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RENESAS_RA_ -D_RA_CORE=CPU0 -D_RA_ORDINAL=1 -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_gen" -I../src/Components/Inc -I../src/Components/Tasks/Inc -I../src/FreeRTOS-Cpp/FreeRTOS-Kernel/include -I../src/FreeRTOS-Cpp/FreeRTOS-Cpp/include -I../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/RM_FSP_PORT/ -I"." -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_cfg/fsp_cfg/bsp" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_cfg/fsp_cfg" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/src" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc/api" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc/instances" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/arm/CMSIS_6/CMSIS/Core/Include" -std=c99 -Wno-stringop-overflow -Wno-format-truncation -flax-vector-conversions --param=min-pagesize=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

