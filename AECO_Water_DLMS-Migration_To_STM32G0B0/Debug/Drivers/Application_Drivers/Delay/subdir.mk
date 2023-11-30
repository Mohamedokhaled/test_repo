################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Application_Drivers/Delay/delay.c 

OBJS += \
./Drivers/Application_Drivers/Delay/delay.o 

C_DEPS += \
./Drivers/Application_Drivers/Delay/delay.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Application_Drivers/Delay/%.o Drivers/Application_Drivers/Delay/%.su Drivers/Application_Drivers/Delay/%.cyclo: ../Drivers/Application_Drivers/Delay/%.c Drivers/Application_Drivers/Delay/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G0B0xx -c -I../Core/Inc -I../Drivers/Application_Drivers/BaseTimer -I../Drivers/Application_Drivers/billing -I../Drivers/Application_Drivers/Delay -I../Drivers/Application_Drivers/dlms -I../Drivers/Application_Drivers/Dlms_Interface -I../Drivers/Application_Drivers/Dlms_Interface/Includes -I../Drivers/Application_Drivers/Dlms_Interface/Includes/Src -I../Drivers/Application_Drivers/Display -I../Drivers/Application_Drivers/e2prom -I../Drivers/Application_Drivers/GSM_4G -I../Drivers/Application_Drivers/kernal -I../Drivers/Application_Drivers/MCU_interface -I../Drivers/Application_Drivers/RS485 -I../Drivers/Application_Drivers/RTC_Wrapper -I../Drivers/Application_Drivers/SolenoidValve -I../Drivers/Application_Drivers/Utilities -I../Drivers/Application_Drivers/ -I../Drivers/STM32G0xx_HAL_Driver/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G0xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-Application_Drivers-2f-Delay

clean-Drivers-2f-Application_Drivers-2f-Delay:
	-$(RM) ./Drivers/Application_Drivers/Delay/delay.cyclo ./Drivers/Application_Drivers/Delay/delay.d ./Drivers/Application_Drivers/Delay/delay.o ./Drivers/Application_Drivers/Delay/delay.su

.PHONY: clean-Drivers-2f-Application_Drivers-2f-Delay

