################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Application_Drivers/Dlms_Interface/Image.c \
../Drivers/Application_Drivers/Dlms_Interface/dlms-module.c \
../Drivers/Application_Drivers/Dlms_Interface/hdlc_rx.c \
../Drivers/Application_Drivers/Dlms_Interface/obis_init.c \
../Drivers/Application_Drivers/Dlms_Interface/obis_meter_if.c \
../Drivers/Application_Drivers/Dlms_Interface/sys-hal.c \
../Drivers/Application_Drivers/Dlms_Interface/sys-util.c 

OBJS += \
./Drivers/Application_Drivers/Dlms_Interface/Image.o \
./Drivers/Application_Drivers/Dlms_Interface/dlms-module.o \
./Drivers/Application_Drivers/Dlms_Interface/hdlc_rx.o \
./Drivers/Application_Drivers/Dlms_Interface/obis_init.o \
./Drivers/Application_Drivers/Dlms_Interface/obis_meter_if.o \
./Drivers/Application_Drivers/Dlms_Interface/sys-hal.o \
./Drivers/Application_Drivers/Dlms_Interface/sys-util.o 

C_DEPS += \
./Drivers/Application_Drivers/Dlms_Interface/Image.d \
./Drivers/Application_Drivers/Dlms_Interface/dlms-module.d \
./Drivers/Application_Drivers/Dlms_Interface/hdlc_rx.d \
./Drivers/Application_Drivers/Dlms_Interface/obis_init.d \
./Drivers/Application_Drivers/Dlms_Interface/obis_meter_if.d \
./Drivers/Application_Drivers/Dlms_Interface/sys-hal.d \
./Drivers/Application_Drivers/Dlms_Interface/sys-util.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Application_Drivers/Dlms_Interface/%.o Drivers/Application_Drivers/Dlms_Interface/%.su Drivers/Application_Drivers/Dlms_Interface/%.cyclo: ../Drivers/Application_Drivers/Dlms_Interface/%.c Drivers/Application_Drivers/Dlms_Interface/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G0B0xx -c -I../Core/Inc -I../Drivers/Application_Drivers/BaseTimer -I../Drivers/Application_Drivers/billing -I../Drivers/Application_Drivers/Delay -I../Drivers/Application_Drivers/dlms -I../Drivers/Application_Drivers/Dlms_Interface -I../Drivers/Application_Drivers/Dlms_Interface/Includes -I../Drivers/Application_Drivers/Dlms_Interface/Includes/Src -I../Drivers/Application_Drivers/Display -I../Drivers/Application_Drivers/e2prom -I../Drivers/Application_Drivers/GSM_4G -I../Drivers/Application_Drivers/kernal -I../Drivers/Application_Drivers/MCU_interface -I../Drivers/Application_Drivers/RS485 -I../Drivers/Application_Drivers/RTC_Wrapper -I../Drivers/Application_Drivers/SolenoidValve -I../Drivers/Application_Drivers/Utilities -I../Drivers/Application_Drivers/ -I../Drivers/STM32G0xx_HAL_Driver/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G0xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-Application_Drivers-2f-Dlms_Interface

clean-Drivers-2f-Application_Drivers-2f-Dlms_Interface:
	-$(RM) ./Drivers/Application_Drivers/Dlms_Interface/Image.cyclo ./Drivers/Application_Drivers/Dlms_Interface/Image.d ./Drivers/Application_Drivers/Dlms_Interface/Image.o ./Drivers/Application_Drivers/Dlms_Interface/Image.su ./Drivers/Application_Drivers/Dlms_Interface/dlms-module.cyclo ./Drivers/Application_Drivers/Dlms_Interface/dlms-module.d ./Drivers/Application_Drivers/Dlms_Interface/dlms-module.o ./Drivers/Application_Drivers/Dlms_Interface/dlms-module.su ./Drivers/Application_Drivers/Dlms_Interface/hdlc_rx.cyclo ./Drivers/Application_Drivers/Dlms_Interface/hdlc_rx.d ./Drivers/Application_Drivers/Dlms_Interface/hdlc_rx.o ./Drivers/Application_Drivers/Dlms_Interface/hdlc_rx.su ./Drivers/Application_Drivers/Dlms_Interface/obis_init.cyclo ./Drivers/Application_Drivers/Dlms_Interface/obis_init.d ./Drivers/Application_Drivers/Dlms_Interface/obis_init.o ./Drivers/Application_Drivers/Dlms_Interface/obis_init.su ./Drivers/Application_Drivers/Dlms_Interface/obis_meter_if.cyclo ./Drivers/Application_Drivers/Dlms_Interface/obis_meter_if.d ./Drivers/Application_Drivers/Dlms_Interface/obis_meter_if.o ./Drivers/Application_Drivers/Dlms_Interface/obis_meter_if.su ./Drivers/Application_Drivers/Dlms_Interface/sys-hal.cyclo ./Drivers/Application_Drivers/Dlms_Interface/sys-hal.d ./Drivers/Application_Drivers/Dlms_Interface/sys-hal.o ./Drivers/Application_Drivers/Dlms_Interface/sys-hal.su ./Drivers/Application_Drivers/Dlms_Interface/sys-util.cyclo ./Drivers/Application_Drivers/Dlms_Interface/sys-util.d ./Drivers/Application_Drivers/Dlms_Interface/sys-util.o ./Drivers/Application_Drivers/Dlms_Interface/sys-util.su

.PHONY: clean-Drivers-2f-Application_Drivers-2f-Dlms_Interface

