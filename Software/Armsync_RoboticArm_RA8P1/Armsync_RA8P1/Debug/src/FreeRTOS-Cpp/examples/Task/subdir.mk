################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/FreeRTOS-Cpp/examples/Task/delay.cpp \
../src/FreeRTOS-Cpp/examples/Task/delayUntil.cpp \
../src/FreeRTOS-Cpp/examples/Task/getPriority.cpp \
../src/FreeRTOS-Cpp/examples/Task/notify.cpp \
../src/FreeRTOS-Cpp/examples/Task/notifyAndQuery.cpp \
../src/FreeRTOS-Cpp/examples/Task/notifyAndQueryFromISR.cpp \
../src/FreeRTOS-Cpp/examples/Task/notifyFromISR.cpp \
../src/FreeRTOS-Cpp/examples/Task/notifyGive.cpp \
../src/FreeRTOS-Cpp/examples/Task/notifyGiveFromISR.cpp \
../src/FreeRTOS-Cpp/examples/Task/notifyStateClear.cpp \
../src/FreeRTOS-Cpp/examples/Task/notifyTake.cpp \
../src/FreeRTOS-Cpp/examples/Task/notifyValueClear.cpp \
../src/FreeRTOS-Cpp/examples/Task/notifyWait.cpp \
../src/FreeRTOS-Cpp/examples/Task/resume.cpp \
../src/FreeRTOS-Cpp/examples/Task/resumeFromISR.cpp \
../src/FreeRTOS-Cpp/examples/Task/setPriority.cpp \
../src/FreeRTOS-Cpp/examples/Task/staticTask.cpp \
../src/FreeRTOS-Cpp/examples/Task/suspend.cpp \
../src/FreeRTOS-Cpp/examples/Task/task.cpp 

SREC += \
Armsync_RA8P1.srec 

OBJS += \
./src/FreeRTOS-Cpp/examples/Task/delay.o \
./src/FreeRTOS-Cpp/examples/Task/delayUntil.o \
./src/FreeRTOS-Cpp/examples/Task/getPriority.o \
./src/FreeRTOS-Cpp/examples/Task/notify.o \
./src/FreeRTOS-Cpp/examples/Task/notifyAndQuery.o \
./src/FreeRTOS-Cpp/examples/Task/notifyAndQueryFromISR.o \
./src/FreeRTOS-Cpp/examples/Task/notifyFromISR.o \
./src/FreeRTOS-Cpp/examples/Task/notifyGive.o \
./src/FreeRTOS-Cpp/examples/Task/notifyGiveFromISR.o \
./src/FreeRTOS-Cpp/examples/Task/notifyStateClear.o \
./src/FreeRTOS-Cpp/examples/Task/notifyTake.o \
./src/FreeRTOS-Cpp/examples/Task/notifyValueClear.o \
./src/FreeRTOS-Cpp/examples/Task/notifyWait.o \
./src/FreeRTOS-Cpp/examples/Task/resume.o \
./src/FreeRTOS-Cpp/examples/Task/resumeFromISR.o \
./src/FreeRTOS-Cpp/examples/Task/setPriority.o \
./src/FreeRTOS-Cpp/examples/Task/staticTask.o \
./src/FreeRTOS-Cpp/examples/Task/suspend.o \
./src/FreeRTOS-Cpp/examples/Task/task.o 

MAP += \
Armsync_RA8P1.map 

CPP_DEPS += \
./src/FreeRTOS-Cpp/examples/Task/delay.d \
./src/FreeRTOS-Cpp/examples/Task/delayUntil.d \
./src/FreeRTOS-Cpp/examples/Task/getPriority.d \
./src/FreeRTOS-Cpp/examples/Task/notify.d \
./src/FreeRTOS-Cpp/examples/Task/notifyAndQuery.d \
./src/FreeRTOS-Cpp/examples/Task/notifyAndQueryFromISR.d \
./src/FreeRTOS-Cpp/examples/Task/notifyFromISR.d \
./src/FreeRTOS-Cpp/examples/Task/notifyGive.d \
./src/FreeRTOS-Cpp/examples/Task/notifyGiveFromISR.d \
./src/FreeRTOS-Cpp/examples/Task/notifyStateClear.d \
./src/FreeRTOS-Cpp/examples/Task/notifyTake.d \
./src/FreeRTOS-Cpp/examples/Task/notifyValueClear.d \
./src/FreeRTOS-Cpp/examples/Task/notifyWait.d \
./src/FreeRTOS-Cpp/examples/Task/resume.d \
./src/FreeRTOS-Cpp/examples/Task/resumeFromISR.d \
./src/FreeRTOS-Cpp/examples/Task/setPriority.d \
./src/FreeRTOS-Cpp/examples/Task/staticTask.d \
./src/FreeRTOS-Cpp/examples/Task/suspend.d \
./src/FreeRTOS-Cpp/examples/Task/task.d 


# Each subdirectory must supply rules for building sources it contributes
src/FreeRTOS-Cpp/examples/Task/%.o: ../src/FreeRTOS-Cpp/examples/Task/%.cpp
	$(file > $@.in,-mthumb -mfloat-abi=hard -mcpu=cortex-m85+nopacbti -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RENESAS_RA_ -DTF_LITE_STATIC_MEMORY -D_RA_CORE=CPU0 -D_RA_ORDINAL=1 -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_gen" -I"." -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_cfg/fsp_cfg/bsp" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_cfg/fsp_cfg" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/src" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc/api" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc/instances" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/arm/CMSIS_6/CMSIS/Core/Include" -std=c++17 -fabi-version=0 -fno-exceptions -fno-rtti -Wno-stringop-overflow -Wno-format-truncation -flax-vector-conversions --param=min-pagesize=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c++ "$<")
	@echo Building file: $< && arm-none-eabi-g++ @"$@.in"

