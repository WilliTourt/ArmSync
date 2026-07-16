################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/FreeRTOS-Cpp/examples/Queue/overwrite.cpp \
../src/FreeRTOS-Cpp/examples/Queue/overwriteFromISR.cpp \
../src/FreeRTOS-Cpp/examples/Queue/peek.cpp \
../src/FreeRTOS-Cpp/examples/Queue/queue.cpp \
../src/FreeRTOS-Cpp/examples/Queue/receive.cpp \
../src/FreeRTOS-Cpp/examples/Queue/receiveFromISR.cpp \
../src/FreeRTOS-Cpp/examples/Queue/sendToBack.cpp \
../src/FreeRTOS-Cpp/examples/Queue/sendToBackFromISR.cpp \
../src/FreeRTOS-Cpp/examples/Queue/sendToFront.cpp \
../src/FreeRTOS-Cpp/examples/Queue/sendToFrontFromISR.cpp \
../src/FreeRTOS-Cpp/examples/Queue/staticQueue.cpp 

SREC += \
Armsync_RA8P1.srec 

OBJS += \
./src/FreeRTOS-Cpp/examples/Queue/overwrite.o \
./src/FreeRTOS-Cpp/examples/Queue/overwriteFromISR.o \
./src/FreeRTOS-Cpp/examples/Queue/peek.o \
./src/FreeRTOS-Cpp/examples/Queue/queue.o \
./src/FreeRTOS-Cpp/examples/Queue/receive.o \
./src/FreeRTOS-Cpp/examples/Queue/receiveFromISR.o \
./src/FreeRTOS-Cpp/examples/Queue/sendToBack.o \
./src/FreeRTOS-Cpp/examples/Queue/sendToBackFromISR.o \
./src/FreeRTOS-Cpp/examples/Queue/sendToFront.o \
./src/FreeRTOS-Cpp/examples/Queue/sendToFrontFromISR.o \
./src/FreeRTOS-Cpp/examples/Queue/staticQueue.o 

MAP += \
Armsync_RA8P1.map 

CPP_DEPS += \
./src/FreeRTOS-Cpp/examples/Queue/overwrite.d \
./src/FreeRTOS-Cpp/examples/Queue/overwriteFromISR.d \
./src/FreeRTOS-Cpp/examples/Queue/peek.d \
./src/FreeRTOS-Cpp/examples/Queue/queue.d \
./src/FreeRTOS-Cpp/examples/Queue/receive.d \
./src/FreeRTOS-Cpp/examples/Queue/receiveFromISR.d \
./src/FreeRTOS-Cpp/examples/Queue/sendToBack.d \
./src/FreeRTOS-Cpp/examples/Queue/sendToBackFromISR.d \
./src/FreeRTOS-Cpp/examples/Queue/sendToFront.d \
./src/FreeRTOS-Cpp/examples/Queue/sendToFrontFromISR.d \
./src/FreeRTOS-Cpp/examples/Queue/staticQueue.d 


# Each subdirectory must supply rules for building sources it contributes
src/FreeRTOS-Cpp/examples/Queue/%.o: ../src/FreeRTOS-Cpp/examples/Queue/%.cpp
	$(file > $@.in,-mthumb -mfloat-abi=hard -mcpu=cortex-m85+nopacbti -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RENESAS_RA_ -DTF_LITE_STATIC_MEMORY -D_RA_CORE=CPU0 -D_RA_ORDINAL=1 -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_gen" -I"." -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_cfg/fsp_cfg/bsp" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_cfg/fsp_cfg" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/src" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc/api" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc/instances" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/arm/CMSIS_6/CMSIS/Core/Include" -std=c++17 -fabi-version=0 -fno-exceptions -fno-rtti -Wno-stringop-overflow -Wno-format-truncation -flax-vector-conversions --param=min-pagesize=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c++ "$<")
	@echo Building file: $< && arm-none-eabi-g++ @"$@.in"

