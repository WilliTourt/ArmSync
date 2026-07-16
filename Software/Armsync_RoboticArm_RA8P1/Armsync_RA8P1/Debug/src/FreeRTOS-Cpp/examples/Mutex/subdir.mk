################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/FreeRTOS-Cpp/examples/Mutex/lock.cpp \
../src/FreeRTOS-Cpp/examples/Mutex/mutex.cpp \
../src/FreeRTOS-Cpp/examples/Mutex/recursiveLock.cpp \
../src/FreeRTOS-Cpp/examples/Mutex/recursiveMutex.cpp \
../src/FreeRTOS-Cpp/examples/Mutex/staticMutex.cpp \
../src/FreeRTOS-Cpp/examples/Mutex/staticRecursiveMutex.cpp \
../src/FreeRTOS-Cpp/examples/Mutex/unlock.cpp 

SREC += \
Armsync_RA8P1.srec 

OBJS += \
./src/FreeRTOS-Cpp/examples/Mutex/lock.o \
./src/FreeRTOS-Cpp/examples/Mutex/mutex.o \
./src/FreeRTOS-Cpp/examples/Mutex/recursiveLock.o \
./src/FreeRTOS-Cpp/examples/Mutex/recursiveMutex.o \
./src/FreeRTOS-Cpp/examples/Mutex/staticMutex.o \
./src/FreeRTOS-Cpp/examples/Mutex/staticRecursiveMutex.o \
./src/FreeRTOS-Cpp/examples/Mutex/unlock.o 

MAP += \
Armsync_RA8P1.map 

CPP_DEPS += \
./src/FreeRTOS-Cpp/examples/Mutex/lock.d \
./src/FreeRTOS-Cpp/examples/Mutex/mutex.d \
./src/FreeRTOS-Cpp/examples/Mutex/recursiveLock.d \
./src/FreeRTOS-Cpp/examples/Mutex/recursiveMutex.d \
./src/FreeRTOS-Cpp/examples/Mutex/staticMutex.d \
./src/FreeRTOS-Cpp/examples/Mutex/staticRecursiveMutex.d \
./src/FreeRTOS-Cpp/examples/Mutex/unlock.d 


# Each subdirectory must supply rules for building sources it contributes
src/FreeRTOS-Cpp/examples/Mutex/%.o: ../src/FreeRTOS-Cpp/examples/Mutex/%.cpp
	$(file > $@.in,-mthumb -mfloat-abi=hard -mcpu=cortex-m85+nopacbti -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RENESAS_RA_ -DTF_LITE_STATIC_MEMORY -D_RA_CORE=CPU0 -D_RA_ORDINAL=1 -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_gen" -I"." -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_cfg/fsp_cfg/bsp" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_cfg/fsp_cfg" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/src" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc/api" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc/instances" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/arm/CMSIS_6/CMSIS/Core/Include" -std=c++17 -fabi-version=0 -fno-exceptions -fno-rtti -Wno-stringop-overflow -Wno-format-truncation -flax-vector-conversions --param=min-pagesize=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c++ "$<")
	@echo Building file: $< && arm-none-eabi-g++ @"$@.in"

