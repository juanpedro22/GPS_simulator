################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SDK/platform/system/src/clock/fsl_clock_manager.c \
../SDK/platform/system/src/clock/fsl_clock_manager_common.c 

OBJS += \
./SDK/platform/system/src/clock/fsl_clock_manager.o \
./SDK/platform/system/src/clock/fsl_clock_manager_common.o 

C_DEPS += \
./SDK/platform/system/src/clock/fsl_clock_manager.d \
./SDK/platform/system/src/clock/fsl_clock_manager_common.d 


# Each subdirectory must supply rules for building sources it contributes
SDK/platform/system/src/clock/%.o: ../SDK/platform/system/src/clock/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -D"FSL_OSA_BM_TIMER_CONFIG=2" -D"CPU_MKL25Z128VLK4" -D"PRINTF_ADVANCED_ENABLE=1" -D"PRINTF_FLOAT_ENABLE=1" -I"C:/Users/juan_/OneDrive/Documentos/Mestrado/Software Real Time/test2/gps/SDK/platform/hal/inc" -I"C:/Users/juan_/OneDrive/Documentos/Mestrado/Software Real Time/test2/gps/SDK/platform/hal/src/sim/MKL25Z4" -I"C:/Users/juan_/OneDrive/Documentos/Mestrado/Software Real Time/test2/gps/SDK/platform/system/src/clock/MKL25Z4" -I"C:/Users/juan_/OneDrive/Documentos/Mestrado/Software Real Time/test2/gps/SDK/platform/system/inc" -I"C:/Users/juan_/OneDrive/Documentos/Mestrado/Software Real Time/test2/gps/SDK/platform/osa/inc" -I"C:/Users/juan_/OneDrive/Documentos/Mestrado/Software Real Time/test2/gps/SDK/platform/CMSIS/Include" -I"C:/Users/juan_/OneDrive/Documentos/Mestrado/Software Real Time/test2/gps/SDK/platform/devices" -I"C:/Users/juan_/OneDrive/Documentos/Mestrado/Software Real Time/test2/gps/SDK/platform/devices/MKL25Z4/include" -I"C:/Users/juan_/OneDrive/Documentos/Mestrado/Software Real Time/test2/gps/SDK/platform/devices/MKL25Z4/startup" -I"C:/Users/juan_/OneDrive/Documentos/Mestrado/Software Real Time/test2/gps/Generated_Code/SDK/platform/devices/MKL25Z4/startup" -I"C:/Users/juan_/OneDrive/Documentos/Mestrado/Software Real Time/test2/gps/Sources" -I"C:/Users/juan_/OneDrive/Documentos/Mestrado/Software Real Time/test2/gps/Generated_Code" -I"C:/Users/juan_/OneDrive/Documentos/Mestrado/Software Real Time/test2/gps/SDK/platform/utilities/src" -I"C:/Users/juan_/OneDrive/Documentos/Mestrado/Software Real Time/test2/gps/SDK/platform/utilities/inc" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


