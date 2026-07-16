################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/portasm.S \
../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/xtensa_context.S \
../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/xtensa_loadstore_handler.S \
../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/xtensa_vector_defaults.S \
../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/xtensa_vectors.S 

C_SRCS += \
../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/FreeRTOS-openocd.c \
../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/port.c \
../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/port_common.c \
../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/port_systick.c \
../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/xtensa_init.c \
../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/xtensa_overlay_os_hook.c 

SREC += \
Armsync_RA8P1.srec 

C_DEPS += \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/FreeRTOS-openocd.d \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/port.d \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/port_common.d \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/port_systick.d \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/xtensa_init.d \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/xtensa_overlay_os_hook.d 

OBJS += \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/FreeRTOS-openocd.o \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/port.o \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/port_common.o \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/port_systick.o \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/portasm.o \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/xtensa_context.o \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/xtensa_init.o \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/xtensa_loadstore_handler.o \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/xtensa_overlay_os_hook.o \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/xtensa_vector_defaults.o \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/xtensa_vectors.o 

S_UPPER_DEPS += \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/portasm.d \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/xtensa_context.d \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/xtensa_loadstore_handler.d \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/xtensa_vector_defaults.d \
./src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/xtensa_vectors.d 

MAP += \
Armsync_RA8P1.map 


# Each subdirectory must supply rules for building sources it contributes
src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/%.o: ../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/%.c
	$(file > $@.in,-mthumb -mfloat-abi=hard -mcpu=cortex-m85+nopacbti -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RENESAS_RA_ -D_RA_CORE=CPU0 -D_RA_ORDINAL=1 -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_gen" -I"." -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_cfg/fsp_cfg/bsp" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_cfg/fsp_cfg" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/src" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc/api" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc/instances" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/arm/CMSIS_6/CMSIS/Core/Include" -std=c99 -Wno-stringop-overflow -Wno-format-truncation -flax-vector-conversions --param=min-pagesize=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"
src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/%.o: ../src/FreeRTOS-Cpp/FreeRTOS-Kernel/portable/ThirdParty/GCC/Xtensa_ESP32/%.S
	$(file > $@.in,-mthumb -mfloat-abi=hard -mcpu=cortex-m85+nopacbti -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -x assembler-with-cpp -D_RENESAS_RA_ -D_RA_CORE=CPU0 -D_RA_ORDINAL=1 -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_gen" -I"." -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_cfg/fsp_cfg/bsp" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra_cfg/fsp_cfg" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/src" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc/api" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/fsp/inc/instances" -I"E:/Folders/Documents/Dev/Renesas/RA8P1/Projects/Armsync_RA8P1/ra/arm/CMSIS_6/CMSIS/Core/Include" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

