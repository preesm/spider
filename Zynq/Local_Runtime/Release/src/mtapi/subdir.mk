################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/mtapi/mtapi.c 

OBJS += \
./src/mtapi/mtapi.o 

C_DEPS += \
./src/mtapi/mtapi.d 


# Each subdirectory must supply rules for building sources it contributes
src/mtapi/%.o: ../src/mtapi/%.c
	@echo Building file: $<
	@echo Invoking: MicroBlaze gcc compiler
	mb-gcc -Wall -O3 -c -fmessage-length=0 -IC:/work/Xilinx/projects/software/local_runtime_bsp_0/microblaze_0/include -mlittle-endian -mxl-barrel-shift -mxl-pattern-compare -mcpu=v8.40.a -mno-xl-soft-mul -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo Finished building: $<
	@echo ' '


