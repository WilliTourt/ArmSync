################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/FreeRTOS-Cpp/examples/StreamBuffer/receive.cpp \
../src/FreeRTOS-Cpp/examples/StreamBuffer/receiveFromISR.cpp \
../src/FreeRTOS-Cpp/examples/StreamBuffer/send.cpp \
../src/FreeRTOS-Cpp/examples/StreamBuffer/sendFromISR.cpp \
../src/FreeRTOS-Cpp/examples/StreamBuffer/staticStreamBuffer.cpp \
../src/FreeRTOS-Cpp/examples/StreamBuffer/streamBuffer.cpp 

SREC += \
Armsync_RA8P1.srec 

OBJS += \
./src/FreeRTOS-Cpp/examples/StreamBuffer/receive.o \
./src/FreeRTOS-Cpp/examples/StreamBuffer/receiveFromISR.o \
./src/FreeRTOS-Cpp/examples/StreamBuffer/send.o \
./src/FreeRTOS-Cpp/examples/StreamBuffer/sendFromISR.o \
./src/FreeRTOS-Cpp/examples/StreamBuffer/staticStreamBuffer.o \
./src/FreeRTOS-Cpp/examples/StreamBuffer/streamBuffer.o 

MAP += \
Armsync_RA8P1.map 

CPP_DEPS += \
./src/FreeRTOS-Cpp/examples/StreamBuffer/receive.d \
./src/FreeRTOS-Cpp/examples/StreamBuffer/receiveFromISR.d \
./src/FreeRTOS-Cpp/examples/StreamBuffer/send.d \
./src/FreeRTOS-Cpp/examples/StreamBuffer/sendFromISR.d \
./src/FreeRTOS-Cpp/examples/StreamBuffer/staticStreamBuffer.d \
./src/FreeRTOS-Cpp/examples/StreamBuffer/streamBuffer.d 


# Each subdirectory must supply rules for building sources it contributes
src/FreeRTOS-Cpp/examples/StreamBuffer/%.o: ../src/FreeRTOS-Cpp/examples/StreamBuffer/%.cpp
	$(file > $@.in,-mthumb -mfloat-abi=hard -mcpu=cortex-m85+nopacbti -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RENESAS_RA_ -DTF_LITE_STATIC_MEMORY -D_RA_CORE=CPU0 -D_RA_ORDINAL=1 -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_gen" -I"." -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_cfg/fsp_cfg/bsp" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_cfg/fsp_cfg" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/src" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc/api" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc/instances" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/arm/CMSIS_6/CMSIS/Core/Include" -std=c++17 -fabi-version=0 -fno-exceptions -fno-rtti -Wno-stringop-overflow -Wno-format-truncation -flax-vector-conversions --param=min-pagesize=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c++ "$<")
	@echo Building file: $< && arm-none-eabi-g++ @"$@.in"

