################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
ELFSIZE += \
local_runtime.elf.size \

ELFCHECK += \
local_runtime.elf.elfcheck \


# All Target
all: local_runtime.elf secondary-outputs

# Tool invocations
local_runtime.elf: $(OBJS) ../src/lscript.ld $(USER_OBJS)
	@echo Building target: $@
	@echo Invoking: MicroBlaze gcc linker
	mb-gcc -Wl,--no-relax -Wl,-T -Wl,../src/lscript.ld -LC:/work/Xilinx/projects/software/local_runtime_bsp_0/microblaze_0/lib -mlittle-endian -mxl-barrel-shift -mxl-pattern-compare -mcpu=v8.40.a -mno-xl-soft-mul -o"local_runtime.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo Finished building target: $@
	@echo ' '

local_runtime.elf.size: local_runtime.elf
	@echo Invoking: MicroBlaze Print Size
	mb-size local_runtime.elf  |tee "local_runtime.elf.size"
	@echo Finished building: $@
	@echo ' '

local_runtime.elf.elfcheck: local_runtime.elf
	@echo Invoking: Xilinx ELF Check
	elfcheck local_runtime.elf -hw C:/work/Xilinx/projects/software/hw_platform/system.xml -pe microblaze_0  |tee "local_runtime.elf.elfcheck"
	@echo Finished building: $@
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES)$(ELFSIZE)$(ELFCHECK)$(S_UPPER_DEPS) local_runtime.elf
	-@echo ' '

secondary-outputs: $(ELFSIZE) $(ELFCHECK)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
