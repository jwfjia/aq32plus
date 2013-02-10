################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/MargAHRS.c \
../src/cli.c \
../src/cliSupport.c \
../src/computeAxisCommands.c \
../src/config.c \
../src/coordinateTransforms.c \
../src/flightCommand.c \
../src/linearAlgebra.c \
../src/lowPassFilter.c \
../src/main.c \
../src/mixer.c \
../src/pid.c \
../src/rfTelem.c \
../src/stm32f4xx_it.c \
../src/system_stm32f4xx.c \
../src/utilities.c \
../src/vertCompFilter.c 

OBJS += \
./src/MargAHRS.o \
./src/cli.o \
./src/cliSupport.o \
./src/computeAxisCommands.o \
./src/config.o \
./src/coordinateTransforms.o \
./src/flightCommand.o \
./src/linearAlgebra.o \
./src/lowPassFilter.o \
./src/main.o \
./src/mixer.o \
./src/pid.o \
./src/rfTelem.o \
./src/stm32f4xx_it.o \
./src/system_stm32f4xx.o \
./src/utilities.o \
./src/vertCompFilter.o 

C_DEPS += \
./src/MargAHRS.d \
./src/cli.d \
./src/cliSupport.d \
./src/computeAxisCommands.d \
./src/config.d \
./src/coordinateTransforms.d \
./src/flightCommand.d \
./src/linearAlgebra.d \
./src/lowPassFilter.d \
./src/main.d \
./src/mixer.d \
./src/pid.d \
./src/rfTelem.d \
./src/stm32f4xx_it.d \
./src/system_stm32f4xx.d \
./src/utilities.d \
./src/vertCompFilter.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Windows GCC C Compiler'
	arm-none-eabi-gcc -DSTM32F4XX -DUSE_STDPERIPH_DRIVER -DUSE_USB_OTG_FS -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\src" -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\Libraries\CMSIS\Include" -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\Libraries\Device\STM32F4xx\Include" -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\Libraries\STM32F4xx_StdPeriph_Driver\inc" -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\Libraries\STM32_USB_Device_Library\Core\inc" -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\Libraries\STM32_USB_Device_Library\Class\cdc\inc" -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\Libraries\STM32_USB_OTG_Driver\inc" -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\src\calibration" -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\src\drv" -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\src\max7456" -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\src\sensors" -I"C:\Users\John Ihlein\Desktop\STM32workspace\aq32Plus\src\vcp" -O0 -Wall -Wa,-adhlns="$@.lst" -fsigned-char -c -fmessage-length=0 -fomit-frame-pointer -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m4 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


