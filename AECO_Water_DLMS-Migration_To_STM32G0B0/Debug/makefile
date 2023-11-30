################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32G0xx_HAL_Driver/Src/subdir.mk
-include Drivers/Application_Drivers/kernal/subdir.mk
-include Drivers/Application_Drivers/e2prom/subdir.mk
-include Drivers/Application_Drivers/dlms/subdir.mk
-include Drivers/Application_Drivers/billing/subdir.mk
-include Drivers/Application_Drivers/Utilities/subdir.mk
-include Drivers/Application_Drivers/SolenoidValve/subdir.mk
-include Drivers/Application_Drivers/RTC_Wrapper/subdir.mk
-include Drivers/Application_Drivers/RS485/subdir.mk
-include Drivers/Application_Drivers/MCU_interface/subdir.mk
-include Drivers/Application_Drivers/GSM_4G/subdir.mk
-include Drivers/Application_Drivers/Dlms_Interface/Srcs/subdir.mk
-include Drivers/Application_Drivers/Dlms_Interface/subdir.mk
-include Drivers/Application_Drivers/Display/subdir.mk
-include Drivers/Application_Drivers/Delay/subdir.mk
-include Drivers/Application_Drivers/BaseTimer/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := migration_task
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
migration_task.elf \

MAP_FILES += \
migration_task.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
migration_task.list \


# All Target
all: main-build

# Main-build Target
main-build: migration_task.elf secondary-outputs

# Tool invocations
migration_task.elf migration_task.map: $(OBJS) $(USER_OBJS) E:\WorkSpace\migration_task\STM32G0B0RETX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "migration_task.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m0plus -T"E:\WorkSpace\migration_task\STM32G0B0RETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="migration_task.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

migration_task.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "migration_task.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) default.size.stdout migration_task.elf migration_task.list migration_task.map
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
