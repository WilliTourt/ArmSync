################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/XCC/Xtensa/portasm.S \
../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/XCC/Xtensa/xtensa_context.S \
../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/XCC/Xtensa/xtensa_intr_asm.S \
../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/XCC/Xtensa/xtensa_vectors.S 

C_SRCS += \
../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/XCC/Xtensa/port.c \
../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/XCC/Xtensa/portclib.c \
../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/XCC/Xtensa/xtensa_init.c \
../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/XCC/Xtensa/xtensa_intr.c \
../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/XCC/Xtensa/xtensa_overlay_os_hook.c 

SREC += \
Armsync_RA8P1.srec 

C_DEPS += \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/XCC/Xtensa/port.d \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/XCC/Xtensa/portclib.d \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/XCC/Xtensa/xtensa_init.d \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/XCC/Xtensa/xtensa_intr.d \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/XCC/Xtensa/xtensa_overlay_os_hook.d 

OBJS += \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/XCC/Xtensa/port.o \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/XCC/Xtensa/portasm.o \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/XCC/Xtensa/portclib.o \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/XCC/Xtensa/xtensa_context.o \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/XCC/Xtensa/xtensa_init.o \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/XCC/Xtensa/xtensa_intr.o \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/XCC/Xtensa/xtensa_intr_asm.o \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/XCC/Xtensa/xtensa_overlay_os_hook.o \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/XCC/Xtensa/xtensa_vectors.o 

S_UPPER_DEPS += \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/XCC/Xtensa/portasm.d \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/XCC/Xtensa/xtensa_context.d \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/XCC/Xtensa/xtensa_intr_asm.d \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/XCC/Xtensa/xtensa_vectors.d 

MAP += \
Armsync_RA8P1.map 


# Each subdirectory must supply rules for building sources it contributes
src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/XCC/Xtensa/%.o: ../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/XCC/Xtensa/%.c
	$(file > $@.in,-mthumb -mfloat-abi=hard -mcpu=cortex-m85+nopacbti -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RENESAS_RA_ -D_RA_CORE=CPU0 -D_RA_ORDINAL=1 -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_gen" -I"." -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_cfg/fsp_cfg/bsp" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_cfg/fsp_cfg" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/src" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc/api" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc/instances" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/arm/CMSIS_6/CMSIS/Core/Include" -std=c99 -Wno-stringop-overflow -Wno-format-truncation -flax-vector-conversions --param=min-pagesize=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"
src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/XCC/Xtensa/%.o: ../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/XCC/Xtensa/%.S
	$(file > $@.in,-mthumb -mfloat-abi=hard -mcpu=cortex-m85+nopacbti -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -x assembler-with-cpp -D_RENESAS_RA_ -D_RA_CORE=CPU0 -D_RA_ORDINAL=1 -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_gen" -I"." -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_cfg/fsp_cfg/bsp" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_cfg/fsp_cfg" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/src" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc/api" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc/instances" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/arm/CMSIS_6/CMSIS/Core/Include" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

