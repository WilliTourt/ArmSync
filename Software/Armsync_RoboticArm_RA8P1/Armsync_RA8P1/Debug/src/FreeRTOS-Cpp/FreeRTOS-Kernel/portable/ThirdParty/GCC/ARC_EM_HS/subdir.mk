################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/ARC_EM_HS/arc_support.s 

C_SRCS += \
../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/ARC_EM_HS/arc_freertos_exceptions.c \
../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/ARC_EM_HS/freertos_tls.c \
../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/ARC_EM_HS/port.c 

S_DEPS += \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/ARC_EM_HS/arc_support.d 

SREC += \
Armsync_RA8P1.srec 

C_DEPS += \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/ARC_EM_HS/arc_freertos_exceptions.d \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/ARC_EM_HS/freertos_tls.d \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/ARC_EM_HS/port.d 

OBJS += \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/ARC_EM_HS/arc_freertos_exceptions.o \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/ARC_EM_HS/arc_support.o \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/ARC_EM_HS/freertos_tls.o \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/ARC_EM_HS/port.o 

MAP += \
Armsync_RA8P1.map 


# Each subdirectory must supply rules for building sources it contributes
src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/ARC_EM_HS/%.o: ../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/ARC_EM_HS/%.c
	$(file > $@.in,-mthumb -mfloat-abi=hard -mcpu=cortex-m85+nopacbti -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RENESAS_RA_ -D_RA_CORE=CPU0 -D_RA_ORDINAL=1 -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_gen" -I"." -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_cfg/fsp_cfg/bsp" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_cfg/fsp_cfg" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/src" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc/api" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc/instances" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/arm/CMSIS_6/CMSIS/Core/Include" -std=c99 -Wno-stringop-overflow -Wno-format-truncation -flax-vector-conversions --param=min-pagesize=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"
src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/ARC_EM_HS/%.o: ../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/ARC_EM_HS/%.s
	$(file > $@.in,-mthumb -mfloat-abi=hard -mcpu=cortex-m85+nopacbti -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -x assembler-with-cpp -D_RENESAS_RA_ -D_RA_CORE=CPU0 -D_RA_ORDINAL=1 -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_gen" -I"." -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_cfg/fsp_cfg/bsp" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_cfg/fsp_cfg" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/src" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc/api" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc/instances" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/arm/CMSIS_6/CMSIS/Core/Include" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

