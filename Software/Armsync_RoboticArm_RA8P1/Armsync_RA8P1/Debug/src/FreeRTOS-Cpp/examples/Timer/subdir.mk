################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/FreeRTOS-Cpp/examples/Timer/changePeriod.cpp \
../src/FreeRTOS-Cpp/examples/Timer/changePeriodFromISR.cpp \
../src/FreeRTOS-Cpp/examples/Timer/getExpiryTime.cpp \
../src/FreeRTOS-Cpp/examples/Timer/getName.cpp \
../src/FreeRTOS-Cpp/examples/Timer/getPeriod.cpp \
../src/FreeRTOS-Cpp/examples/Timer/isActive.cpp \
../src/FreeRTOS-Cpp/examples/Timer/reset.cpp \
../src/FreeRTOS-Cpp/examples/Timer/resetFromISR.cpp \
../src/FreeRTOS-Cpp/examples/Timer/startFromISR.cpp \
../src/FreeRTOS-Cpp/examples/Timer/staticTimer.cpp \
../src/FreeRTOS-Cpp/examples/Timer/stopFromISR.cpp \
../src/FreeRTOS-Cpp/examples/Timer/timer.cpp 

SREC += \
Armsync_RA8P1.srec 

OBJS += \
./src/FreeRTOS-Cpp/examples/Timer/changePeriod.o \
./src/FreeRTOS-Cpp/examples/Timer/changePeriodFromISR.o \
./src/FreeRTOS-Cpp/examples/Timer/getExpiryTime.o \
./src/FreeRTOS-Cpp/examples/Timer/getName.o \
./src/FreeRTOS-Cpp/examples/Timer/getPeriod.o \
./src/FreeRTOS-Cpp/examples/Timer/isActive.o \
./src/FreeRTOS-Cpp/examples/Timer/reset.o \
./src/FreeRTOS-Cpp/examples/Timer/resetFromISR.o \
./src/FreeRTOS-Cpp/examples/Timer/startFromISR.o \
./src/FreeRTOS-Cpp/examples/Timer/staticTimer.o \
./src/FreeRTOS-Cpp/examples/Timer/stopFromISR.o \
./src/FreeRTOS-Cpp/examples/Timer/timer.o 

MAP += \
Armsync_RA8P1.map 

CPP_DEPS += \
./src/FreeRTOS-Cpp/examples/Timer/changePeriod.d \
./src/FreeRTOS-Cpp/examples/Timer/changePeriodFromISR.d \
./src/FreeRTOS-Cpp/examples/Timer/getExpiryTime.d \
./src/FreeRTOS-Cpp/examples/Timer/getName.d \
./src/FreeRTOS-Cpp/examples/Timer/getPeriod.d \
./src/FreeRTOS-Cpp/examples/Timer/isActive.d \
./src/FreeRTOS-Cpp/examples/Timer/reset.d \
./src/FreeRTOS-Cpp/examples/Timer/resetFromISR.d \
./src/FreeRTOS-Cpp/examples/Timer/startFromISR.d \
./src/FreeRTOS-Cpp/examples/Timer/staticTimer.d \
./src/FreeRTOS-Cpp/examples/Timer/stopFromISR.d \
./src/FreeRTOS-Cpp/examples/Timer/timer.d 


# Each subdirectory must supply rules for building sources it contributes
src/FreeRTOS-Cpp/examples/Timer/%.o: ../src/FreeRTOS-Cpp/examples/Timer/%.cpp
	$(file > $@.in,-mthumb -mfloat-abi=hard -mcpu=cortex-m85+nopacbti -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RENESAS_RA_ -DTF_LITE_STATIC_MEMORY -D_RA_CORE=CPU0 -D_RA_ORDINAL=1 -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_gen" -I"." -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_cfg/fsp_cfg/bsp" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_cfg/fsp_cfg" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/src" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc/api" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc/instances" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/arm/CMSIS_6/CMSIS/Core/Include" -std=c++17 -fabi-version=0 -fno-exceptions -fno-rtti -Wno-stringop-overflow -Wno-format-truncation -flax-vector-conversions --param=min-pagesize=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c++ "$<")
	@echo Building file: $< && arm-none-eabi-g++ @"$@.in"

