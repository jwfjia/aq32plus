################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libraries/STM32_USB_Device_Library/Core/src/usbd_core.c \
../Libraries/STM32_USB_Device_Library/Core/src/usbd_ioreq.c \
../Libraries/STM32_USB_Device_Library/Core/src/usbd_req.c 

OBJS += \
./Libraries/STM32_USB_Device_Library/Core/src/usbd_core.o \
./Libraries/STM32_USB_Device_Library/Core/src/usbd_ioreq.o \
./Libraries/STM32_USB_Device_Library/Core/src/usbd_req.o 

C_DEPS += \
./Libraries/STM32_USB_Device_Library/Core/src/usbd_core.d \
./Libraries/STM32_USB_Device_Library/Core/src/usbd_ioreq.d \
./Libraries/STM32_USB_Device_Library/Core/src/usbd_req.d 


# Each subdirectory must supply rules for building sources it contributes
Libraries/STM32_USB_Device_Library/Core/src/%.o: ../Libraries/STM32_USB_Device_Library/Core/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Windows GCC C Compiler'
	arm-none-eabi-gcc -DSTM32F4XX -DUSE_STDPERIPH_DRIVER -DUSE_USB_OTG_FS -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\src" -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\Libraries\CMSIS\Include" -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\Libraries\Device\STM32F4xx\Include" -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\Libraries\STM32F4xx_StdPeriph_Driver\inc" -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\Libraries\STM32_USB_Device_Library\Core\inc" -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\Libraries\STM32_USB_Device_Library\Class\cdc\inc" -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\Libraries\STM32_USB_OTG_Driver\inc" -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\src\calibration" -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\src\drv" -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\src\gps" -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\src\max7456" -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\src\sensors" -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\src\vcp" -O0 -Wall -Wa,-adhlns="$@.lst" -fsigned-char -c -fmessage-length=0 -fomit-frame-pointer -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m4 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


