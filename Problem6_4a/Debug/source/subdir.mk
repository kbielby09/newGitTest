################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/Problem6_4a.c \
../source/mtb.c 

OBJS += \
./source/Problem6_4a.o \
./source/mtb.o 

C_DEPS += \
./source/Problem6_4a.d \
./source/mtb.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DPRINTF_FLOAT_ENABLE=0 -D__USE_CMSIS -DCR_INTEGER_PRINTF -DSDK_DEBUGCONSOLE=1 -D__MCUXPRESSO -DDEBUG -DSDK_OS_BAREMETAL -DFSL_RTOS_BM -DCPU_MKL25Z128VLK4 -DCPU_MKL25Z128VLK4_cm0plus -I"C:\Users\Khondker\Documents\MCUXpressoIDE_11.0.1_2563\workspace\Problem6_4a\board" -I"C:\Users\Khondker\Documents\MCUXpressoIDE_11.0.1_2563\workspace\Problem6_4a\source" -I"C:\Users\Khondker\Documents\MCUXpressoIDE_11.0.1_2563\workspace\Problem6_4a" -I"C:\Users\Khondker\Documents\MCUXpressoIDE_11.0.1_2563\workspace\Problem6_4a\drivers" -I"C:\Users\Khondker\Documents\MCUXpressoIDE_11.0.1_2563\workspace\Problem6_4a\utilities" -I"C:\Users\Khondker\Documents\MCUXpressoIDE_11.0.1_2563\workspace\Problem6_4a\startup" -I"C:\Users\Khondker\Documents\MCUXpressoIDE_11.0.1_2563\workspace\Problem6_4a\CMSIS" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


