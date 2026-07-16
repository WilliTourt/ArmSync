################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/FreeRTOS-Cpp/examples/Semaphore/binarySemaphore.cpp \
../src/FreeRTOS-Cpp/examples/Semaphore/countingSemaphore.cpp \
../src/FreeRTOS-Cpp/examples/Semaphore/give.cpp \
../src/FreeRTOS-Cpp/examples/Semaphore/giveFromISR.cpp \
../src/FreeRTOS-Cpp/examples/Semaphore/staticBinarySemaphore.cpp \
../src/FreeRTOS-Cpp/examples/Semaphore/staticCountingSemaphore.cpp \
../src/FreeRTOS-Cpp/examples/Semaphore/take.cpp 

SREC += \
Armsync_RA8P1.srec 

OBJS += \
./src/FreeRTOS-Cpp/examples/Semaphore/binarySemaphore.o \
./src/FreeRTOS-Cpp/examples/Semaphore/countingSemaphore.o \
./src/FreeRTOS-Cpp/examples/Semaphore/give.o \
./src/FreeRTOS-Cpp/examples/Semaphore/giveFromISR.o \
./src/FreeRTOS-Cpp/examples/Semaphore/staticBinarySemaphore.o \
./src/FreeRTOS-Cpp/examples/Semaphore/staticCountingSemaphore.o \
./src/FreeRTOS-Cpp/examples/Semaphore/take.o 

MAP += \
Armsync_RA8P1.map 

CPP_DEPS += \
./src/FreeRTOS-Cpp/examples/Semaphore/binarySemaphore.d \
./src/FreeRTOS-Cpp/examples/Semaphore/countingSemaphore.d \
./src/FreeRTOS-Cpp/examples/Semaphore/give.d \
./src/FreeRTOS-Cpp/examples/Semaphore/giveFromISR.d \
./src/FreeRTOS-Cpp/examples/Semaphore/staticBinarySemaphore.d \
./src/FreeRTOS-Cpp/examples/Semaphore/staticCountingSemaphore.d \
./src/FreeRTOS-Cpp/examples/Semaphore/take.d 


# Each subdirectory must supply rules for building sources it contributes
src/FreeRTOS-Cpp/examples/Semaphore/%.o: ../src/FreeRTOS-Cpp/examples/Semaphore/%.cpp
	$(file > $@.in,-mthumb -mfloat-abi=hard -mcpu=cortex-m85+nopacbti -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RENESAS_RA_ -DTF_LITE_STATIC_MEMORY -D_RA_CORE=CPU0 -D_RA_ORDINAL=1 -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_gen" -I"." -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_cfg/fsp_cfg/bsp" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_cfg/fsp_cfg" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/src" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc/api" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc/instances" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/arm/CMSIS_6/CMSIS/Core/Include" -std=c++17 -fabi-version=0 -fno-exceptions -fno-rtti -Wno-stringop-overflow -Wno-format-truncation -flax-vector-conversions --param=min-pagesize=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c++ "$<")
	@echo Building file: $< && arm-none-eabi-g++ @"$@.in"

