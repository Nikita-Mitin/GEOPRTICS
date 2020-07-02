################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/subdir.mk
-include Drivers/u8g2/subdir.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
DisplaySTM32F4.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
DisplaySTM32F4.list \

OBJCOPY_BIN += \
DisplaySTM32F4.bin \


# All Target
all: DisplaySTM32F4.elf secondary-outputs

# Tool invocations
DisplaySTM32F4.elf: $(OBJS) $(USER_OBJS) M:\stm32\GEOPTICS\DisplaySTM32F4\STM32F407VGTX_FLASH.ld
	arm-none-eabi-gcc -o "DisplaySTM32F4.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"M:\stm32\GEOPTICS\DisplaySTM32F4\STM32F407VGTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="DisplaySTM32F4.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

DisplaySTM32F4.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "DisplaySTM32F4.list"
	@echo 'Finished building: $@'
	@echo ' '

DisplaySTM32F4.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "DisplaySTM32F4.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
