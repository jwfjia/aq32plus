################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/drv/drv_i2c.c \
../src/drv/drv_led.c \
../src/drv/drv_pwmEsc.c \
../src/drv/drv_pwmServo.c \
../src/drv/drv_rx.c \
../src/drv/drv_spi.c \
../src/drv/drv_system.c \
../src/drv/drv_timingFunctions.c \
../src/drv/drv_uart2.c \
../src/drv/drv_uart3.c \
../src/drv/drv_usb.c 

OBJS += \
./src/drv/drv_i2c.o \
./src/drv/drv_led.o \
./src/drv/drv_pwmEsc.o \
./src/drv/drv_pwmServo.o \
./src/drv/drv_rx.o \
./src/drv/drv_spi.o \
./src/drv/drv_system.o \
./src/drv/drv_timingFunctions.o \
./src/drv/drv_uart2.o \
./src/drv/drv_uart3.o \
./src/drv/drv_usb.o 

C_DEPS += \
./src/drv/drv_i2c.d \
./src/drv/drv_led.d \
./src/drv/drv_pwmEsc.d \
./src/drv/drv_pwmServo.d \
./src/drv/drv_rx.d \
./src/drv/drv_spi.d \
./src/drv/drv_system.d \
./src/drv/drv_timingFunctions.d \
./src/drv/drv_uart2.d \
./src/drv/drv_uart3.d \
./src/drv/drv_usb.d 


# Each subdirectory must supply rules for building sources it contributes
src/drv/%.o: ../src/drv/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Windows GCC C Compiler'
	arm-none-eabi-gcc -DSTM32F4XX -DUSE_STDPERIPH_DRIVER -DUSE_USB_OTG_FS -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\src" -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\Libraries\CMSIS\Include" -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\Libraries\Device\STM32F4xx\Include" -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\Libraries\STM32F4xx_StdPeriph_Driver\inc" -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\Libraries\STM32_USB_Device_Library\Core\inc" -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\Libraries\STM32_USB_Device_Library\Class\cdc\inc" -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\Libraries\STM32_USB_OTG_Driver\inc" -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\src\calibration" -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\src\drv" -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\src\sensors" -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\src\vcp" -O0 -Wall -Wa,-adhlns="$@.lst" -fsigned-char -c -fmessage-length=0 -fomit-frame-pointer -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m4 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


