################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/lrt_core.c \
../src/lrt_debug.c \
../src/lrt_ext_msg_mngr.c \
../src/lrt_task_mngr.c 

OBJS += \
./src/lrt_core.o \
./src/lrt_debug.o \
./src/lrt_ext_msg_mngr.o \
./src/lrt_task_mngr.o 

C_DEPS += \
./src/lrt_core.d \
./src/lrt_debug.d \
./src/lrt_ext_msg_mngr.d \
./src/lrt_task_mngr.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo Building file: $<
	@echo Invoking: MicroBlaze gcc compiler
	mb-gcc -Wall -O3 -c -fmessage-length=0 -IC:/work/Xilinx/projects/software/local_runtime_bsp_0/microblaze_0/include -mlittle-endian -mxl-barrel-shift -mxl-pattern-compare -mcpu=v8.40.a -mno-xl-soft-mul -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo Finished building: $<
	@echo ' '


