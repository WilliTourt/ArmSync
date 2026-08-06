################################################################################
# ArmSync CPU0 App Layer — C++ sources
################################################################################

CXX_SRCS += \
../src/cpp_main.cpp \
../src/app/tasks/tasks.cpp \
../src/app/tasks/Blink.cpp \
../src/app/tasks/ArmCtrlTask.cpp \
../src/app/tasks/CPUCommTask/CPUCommTask.cpp \
../src/app/tasks/IKTask/IKTask.cpp \
../src/app/tasks/UartRecvTask/UartRecvTask.cpp \
../src/app/drivers/tft_spi.cpp \
../src/app/utils/ElegantDebug.cpp

CXX_DEPS += \
./src/app/cpp_main.d \
./src/app/tasks/tasks.d \
./src/app/tasks/Blink.d \
./src/app/tasks/ArmCtrlTask.d \
./src/app/tasks/CPUCommTask/CPUCommTask.d \
./src/app/tasks/IKTask/IKTask.d \
./src/app/tasks/UartRecvTask/UartRecvTask.d \
./src/app/drivers/tft_spi.d \
./src/app/utils/ElegantDebug.d

CXX_OBJS += \
./src/app/cpp_main.o \
./src/app/tasks/tasks.o \
./src/app/tasks/Blink.o \
./src/app/tasks/ArmCtrlTask.o \
./src/app/tasks/CPUCommTask/CPUCommTask.o \
./src/app/tasks/IKTask/IKTask.o \
./src/app/tasks/UartRecvTask/UartRecvTask.o \
./src/app/drivers/tft_spi.o \
./src/app/utils/ElegantDebug.o

# Include paths for C++
CXX_INCLUDES = \
-I"../src" \
-I"../src/FreeRTOS/FreeRTOS-Kernel/include" \
-I"../src/FreeRTOS/FreeRTOS-Kernel/portable/RM_FSP_PORT" \
-I"../src/app/tasks" \
-I"../src/app/tasks/CPUCommTask" \
-I"../src/app/tasks/IKTask" \
-I"../src/app/tasks/UartRecvTask" \
-I"../src/app/drivers" \
-I"../src/app/utils" \
-I"../src/shared" \
-I"../src/FreeRTOS/FreeRTOS-Cpp/include"

# Common compiler flags (shared with C)
CXX_BASE_FLAGS = \
-mthumb -mfloat-abi=hard -mcpu=cortex-m85+nopacbti -O2 \
-fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections \
-fno-strict-aliasing -fno-rtti -fno-exceptions \
-Wunused -Wuninitialized -Wall -Wextra \
-Wconversion -Wpointer-arith -Wshadow -Wfloat-equal -g \
-Wno-stringop-overflow -Wno-format-truncation \
-flax-vector-conversions --param=min-pagesize=0 \
-D_RENESAS_RA_ -D_RA_CORE=CPU0 -D_RA_ORDINAL=1 \
-std=c++17 \
-I"../ra_cfg/fsp_cfg/bsp" \
-I"." \
-I"../ra_gen" \
-I"../ra_cfg/fsp_cfg" \
-I"../ra/arm/CMSIS_6/CMSIS/Core/Include" \
-I"../ra/fsp/inc" \
-I"../ra/fsp/inc/api" \
-I"../ra/fsp/inc/instances" \
$(CXX_INCLUDES)

# Pattern rule: compile .cpp -> .o
src/app/%.o: ../src/app/%.cpp
	$(file > $@.in,$(CXX_BASE_FLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c++ "$<")
	@echo Building file: $< && arm-none-eabi-g++ @"$@.in"

# Special case: cpp_main.cpp is directly under src/
src/app/cpp_main.o: ../src/cpp_main.cpp
	$(file > $@.in,$(CXX_BASE_FLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c++ "$<")
	@echo Building file: $< && arm-none-eabi-g++ @"$@.in"
