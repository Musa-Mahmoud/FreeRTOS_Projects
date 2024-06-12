################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
F:/Learning/Courses/Embeded\ System/ARM/Amgad\ Samir/Online/STM32CubeWorkspace/Common/ThirdParty/FreeRTOS/portable/MemMang/heap_4.c 

OBJS += \
./Common/ThirdParty/FreeRTOS/portable/MemMang/heap_4.o 

C_DEPS += \
./Common/ThirdParty/FreeRTOS/portable/MemMang/heap_4.d 


# Each subdirectory must supply rules for building sources it contributes
Common/ThirdParty/FreeRTOS/portable/MemMang/heap_4.o: F:/Learning/Courses/Embeded\ System/ARM/Amgad\ Samir/Online/STM32CubeWorkspace/Common/ThirdParty/FreeRTOS/portable/MemMang/heap_4.c Common/ThirdParty/FreeRTOS/portable/MemMang/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"F:/Learning/Courses/Embeded System/ARM/Amgad Samir/Online/STM32CubeWorkspace/Common/ThirdParty/FreeRTOS/include" -I"F:/Learning/Courses/Embeded System/ARM/Amgad Samir/Online/STM32CubeWorkspace/Common/ThirdParty/FreeRTOS/portable/GCC/ARM_CM4F" -I"F:/Learning/Courses/Embeded System/ARM/Amgad Samir/Online/STM32CubeWorkspace/Common/ThirdParty/SEGGER/Config" -I"F:/Learning/Courses/Embeded System/ARM/Amgad Samir/Online/STM32CubeWorkspace/Common/ThirdParty/SEGGER/OS" -I"F:/Learning/Courses/Embeded System/ARM/Amgad Samir/Online/STM32CubeWorkspace/Common/ThirdParty/SEGGER/SEGGER" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Common-2f-ThirdParty-2f-FreeRTOS-2f-portable-2f-MemMang

clean-Common-2f-ThirdParty-2f-FreeRTOS-2f-portable-2f-MemMang:
	-$(RM) ./Common/ThirdParty/FreeRTOS/portable/MemMang/heap_4.cyclo ./Common/ThirdParty/FreeRTOS/portable/MemMang/heap_4.d ./Common/ThirdParty/FreeRTOS/portable/MemMang/heap_4.o ./Common/ThirdParty/FreeRTOS/portable/MemMang/heap_4.su

.PHONY: clean-Common-2f-ThirdParty-2f-FreeRTOS-2f-portable-2f-MemMang

