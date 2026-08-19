################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/add.cc \
../ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/batch_matmul.cc \
../ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/conv.cc \
../ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/depthwise_conv.cc \
../ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/fully_connected.cc \
../ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/maximum_minimum.cc \
../ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/mul.cc \
../ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/pooling.cc \
../ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/softmax.cc \
../ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/svdf.cc \
../ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/transpose_conv.cc \
../ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/unidirectional_sequence_lstm.cc 

SREC += \
ArmSync_Solution_CPU0.srec 

CC_DEPS += \
./ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/add.d \
./ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/batch_matmul.d \
./ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/conv.d \
./ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/depthwise_conv.d \
./ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/fully_connected.d \
./ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/maximum_minimum.d \
./ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/mul.d \
./ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/pooling.d \
./ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/softmax.d \
./ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/svdf.d \
./ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/transpose_conv.d \
./ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/unidirectional_sequence_lstm.d 

OBJS += \
./ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/add.o \
./ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/batch_matmul.o \
./ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/conv.o \
./ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/depthwise_conv.o \
./ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/fully_connected.o \
./ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/maximum_minimum.o \
./ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/mul.o \
./ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/pooling.o \
./ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/softmax.o \
./ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/svdf.o \
./ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/transpose_conv.o \
./ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/unidirectional_sequence_lstm.o 

MAP += \
ArmSync_Solution_CPU0.map 


# Each subdirectory must supply rules for building sources it contributes
ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/%.o: ../ra/npu/tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/%.cc
	$(file > $@.in,-mthumb -mfloat-abi=hard -mcpu=cortex-m85+nopacbti -O1 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RA_CORE=CPU0 -D_RA_ORDINAL=1 -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra_cfg/fsp_cfg/bsp" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks/NPUTask" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/flash" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/arm" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks/MotionPlanningTask" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks/FusionTask" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks/UITask" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/ik/include/private" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/ik/include/public" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks/NormalizeTask" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/FreeRTOS/FreeRTOS-Kernel/include" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/FreeRTOS/FreeRTOS-Kernel/portable/RM_FSP_PORT" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/FreeRTOS/FreeRTOS-Cpp/include" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks/UartRecvTask" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks/CPUCommTask" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks/IKTask" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/utils" -I"." -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra_gen" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra_cfg/fsp_cfg" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/arm/CMSIS_6/CMSIS/Core/Include" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/fsp/inc" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/fsp/inc/api" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/fsp/inc/instances" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/npu/ethos-u-core-driver/include" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/arm/CMSIS-NN/Include" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/arm/CMSIS-NN" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/npu/ethos-u-core-software/lib/layer_by_layer_profiler/include" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/npu/ethos-u-core-software/lib/ethosu_monitor/include" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/npu/ethos-u-core-software/lib/ethosu_profiler/include" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/npu/ethos-u-core-software/lib/crc/include" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/npu/ethos-u-core-software/lib/arm_profiler/include" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/arm/CMSIS-View/EventRecorder/Include" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/arm/CMSIS-View/EventRecorder/Config" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/npu/tflite-micro" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/npu/ruy" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/npu/gemmlowp" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/fsp/src/rm_ethosu" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/arm/CMSIS-DSP/PrivateInclude" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/arm/CMSIS-DSP/Include" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/npu/flatbuffers/include" -std=c++20 -fabi-version=0 -Wno-stringop-overflow -Wno-format-truncation -w -flax-vector-conversions --param=min-pagesize=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c++ "$<")
	@echo Building file: $< && arm-none-eabi-g++ @"$@.in"

