################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Components/Tasks/Src/ArmCtrlTask.cpp \
../src/Components/Tasks/Src/CPUCommTask.cpp \
../src/Components/Tasks/Src/CtrllerHandleTask.cpp \
../src/Components/Tasks/Src/GripperTask.cpp \
../src/Components/Tasks/Src/IKTask.cpp 

SREC += \
Armsync_RA8P1.srec 

OBJS += \
./src/Components/Tasks/Src/ArmCtrlTask.o \
./src/Components/Tasks/Src/CPUCommTask.o \
./src/Components/Tasks/Src/CtrllerHandleTask.o \
./src/Components/Tasks/Src/GripperTask.o \
./src/Components/Tasks/Src/IKTask.o 

MAP += \
Armsync_RA8P1.map 

CPP_DEPS += \
./src/Components/Tasks/Src/ArmCtrlTask.d \
./src/Components/Tasks/Src/CPUCommTask.d \
./src/Components/Tasks/Src/CtrllerHandleTask.d \
./src/Components/Tasks/Src/GripperTask.d \
./src/Components/Tasks/Src/IKTask.d 


# Each subdirectory must supply rules for building sources it contributes
src/Components/Tasks/Src/%.o: ../src/Components/Tasks/Src/%.cpp
	$(file > $@.in,-mthumb -mfloat-abi=hard -mcpu=cortex-m85+nopacbti -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RENESAS_RA_ -DTF_LITE_STATIC_MEMORY -D_RA_CORE=CPU0 -D_RA_ORDINAL=1 -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_gen" -I../src/Components/Tasks/Inc -I../src/Components/Inc -I../src/FreeRTOS-Cpp/FreeRTOS-Kernel/include -I../src/FreeRTOS-Cpp/FreeRTOS-Cpp/include -I../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/RM_FSP_PORT/ -I"." -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_cfg/fsp_cfg/bsp" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_cfg/fsp_cfg" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/src" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc/api" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc/instances" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/arm/CMSIS_6/CMSIS/Core/Include" -std=c++17 -fabi-version=0 -fno-exceptions -fno-rtti -Wno-stringop-overflow -Wno-format-truncation -flax-vector-conversions --param=min-pagesize=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c++ "$<")
	@echo Building file: $< && arm-none-eabi-g++ @"$@.in"

