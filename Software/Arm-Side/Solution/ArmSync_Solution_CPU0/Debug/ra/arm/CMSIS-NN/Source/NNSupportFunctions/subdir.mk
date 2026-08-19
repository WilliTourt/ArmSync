################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_padded_s8.c \
../ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_s16.c \
../ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_s4.c \
../ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_s8.c \
../ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_lstm_calculate_gate_s16.c \
../ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_lstm_calculate_gate_s8_s16.c \
../ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_lstm_step_s16.c \
../ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_lstm_step_s8.c \
../ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_mat_mul_core_1x_s4.c \
../ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_mat_mul_core_1x_s8.c \
../ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_mat_mul_core_4x_s8.c \
../ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_mat_mult_nt_interleaved_t_even_s4.c \
../ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_mat_mult_nt_t_s16.c \
../ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_mat_mult_nt_t_s4.c \
../ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_mat_mult_nt_t_s8.c \
../ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_mat_mult_nt_t_s8_s32.c \
../ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_transpose_conv_row_s8_s32.c \
../ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_vec_mat_mul_result_acc_s16.c \
../ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_vec_mat_mul_result_acc_s8_s16.c \
../ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_per_ch_s8.c \
../ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_s16.c \
../ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_s16_s16.c \
../ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_s4.c \
../ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_s8.c \
../ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_svdf_s8.c \
../ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nntables.c \
../ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_q7_to_q15_with_offset.c \
../ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_s8_to_s16_unordered_with_offset.c 

SREC += \
ArmSync_Solution_CPU0.srec 

C_DEPS += \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_padded_s8.d \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_s16.d \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_s4.d \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_s8.d \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_lstm_calculate_gate_s16.d \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_lstm_calculate_gate_s8_s16.d \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_lstm_step_s16.d \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_lstm_step_s8.d \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_mat_mul_core_1x_s4.d \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_mat_mul_core_1x_s8.d \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_mat_mul_core_4x_s8.d \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_mat_mult_nt_interleaved_t_even_s4.d \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_mat_mult_nt_t_s16.d \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_mat_mult_nt_t_s4.d \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_mat_mult_nt_t_s8.d \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_mat_mult_nt_t_s8_s32.d \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_transpose_conv_row_s8_s32.d \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_vec_mat_mul_result_acc_s16.d \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_vec_mat_mul_result_acc_s8_s16.d \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_per_ch_s8.d \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_s16.d \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_s16_s16.d \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_s4.d \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_s8.d \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_svdf_s8.d \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nntables.d \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_q7_to_q15_with_offset.d \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_s8_to_s16_unordered_with_offset.d 

OBJS += \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_padded_s8.o \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_s16.o \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_s4.o \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_s8.o \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_lstm_calculate_gate_s16.o \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_lstm_calculate_gate_s8_s16.o \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_lstm_step_s16.o \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_lstm_step_s8.o \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_mat_mul_core_1x_s4.o \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_mat_mul_core_1x_s8.o \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_mat_mul_core_4x_s8.o \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_mat_mult_nt_interleaved_t_even_s4.o \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_mat_mult_nt_t_s16.o \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_mat_mult_nt_t_s4.o \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_mat_mult_nt_t_s8.o \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_mat_mult_nt_t_s8_s32.o \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_transpose_conv_row_s8_s32.o \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_vec_mat_mul_result_acc_s16.o \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_vec_mat_mul_result_acc_s8_s16.o \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_per_ch_s8.o \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_s16.o \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_s16_s16.o \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_s4.o \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_s8.o \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_svdf_s8.o \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_nntables.o \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_q7_to_q15_with_offset.o \
./ra/arm/CMSIS-NN/Source/NNSupportFunctions/arm_s8_to_s16_unordered_with_offset.o 

MAP += \
ArmSync_Solution_CPU0.map 


# Each subdirectory must supply rules for building sources it contributes
ra/arm/CMSIS-NN/Source/NNSupportFunctions/%.o: ../ra/arm/CMSIS-NN/Source/NNSupportFunctions/%.c
	$(file > $@.in,-mthumb -mfloat-abi=hard -mcpu=cortex-m85+nopacbti -O1 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RENESAS_RA_ -D_RA_CORE=CPU0 -D_RA_ORDINAL=1 -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra_cfg/fsp_cfg/bsp" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks/MotionPlanningTask" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/ik/include/public" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/ik/include/private" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/utils" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/arm" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/flash" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks/IKTask" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks/NPUTask" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks/UITask" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks/CPUCommTask" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks/UartRecvTask" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks/NormalizeTask" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks/FusionTask" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/app/tasks" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/FreeRTOS/FreeRTOS-Cpp/include" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/FreeRTOS/FreeRTOS-Kernel/portable/RM_FSP_PORT" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src/FreeRTOS/FreeRTOS-Kernel/include" -I"." -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra_gen" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra_cfg/fsp_cfg" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/arm/CMSIS_6/CMSIS/Core/Include" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/src" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/fsp/inc" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/fsp/inc/api" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/fsp/inc/instances" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/npu/ethos-u-core-driver/include" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/arm/CMSIS-NN/Include" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/arm/CMSIS-NN" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/npu/ethos-u-core-software/lib/layer_by_layer_profiler/include" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/npu/ethos-u-core-software/lib/ethosu_monitor/include" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/npu/ethos-u-core-software/lib/ethosu_profiler/include" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/npu/ethos-u-core-software/lib/crc/include" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/npu/ethos-u-core-software/lib/arm_profiler/include" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/arm/CMSIS-View/EventRecorder/Include" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/arm/CMSIS-View/EventRecorder/Config" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/npu/tflite-micro" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/npu/ruy" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/npu/gemmlowp" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/fsp/src/rm_ethosu" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/arm/CMSIS-DSP/PrivateInclude" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/arm/CMSIS-DSP/Include" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/ArmSync_Solution_CPU0/ra/npu/flatbuffers/include" -std=c11 -Wno-stringop-overflow -Wno-format-truncation -w -flax-vector-conversions --param=min-pagesize=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

