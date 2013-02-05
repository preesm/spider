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
liblocal_runtime.elf.size \

ELFCHECK += \
liblocal_runtime.elf.elfcheck \


# All Target
all: liblocal_runtime.elf secondary-outputs

# Tool invocations
liblocal_runtime.elf: $(OBJS) $(USER_OBJS)
	@echo Building target: $@
	@echo Invoking: MicroBlaze archiver
	mb-ar -r "liblocal_runtime.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo Finished building target: $@
	@echo ' '

liblocal_runtime.elf.size: liblocal_runtime.elf
	@echo Invoking: MicroBlaze Print Size
	mb-size liblocal_runtime.elf  |tee "liblocal_runtime.elf.size"
	@echo Finished building: $@
	@echo ' '

liblocal_runtime.elf.elfcheck: liblocal_runtime.elf
	@echo Invoking: Xilinx ELF Check
	elfcheck liblocal_runtime.elf -hw C:/work/Xilinx/projects/software/hw_platform/system.xml -pe microblaze_0  |tee "liblocal_runtime.elf.elfcheck"
	@echo Finished building: $@
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ARCHIVES)$(ELFSIZE)$(ELFCHECK)$(S_UPPER_DEPS) liblocal_runtime.elf
	-@echo ' '

secondary-outputs: $(ELFSIZE) $(ELFCHECK)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
