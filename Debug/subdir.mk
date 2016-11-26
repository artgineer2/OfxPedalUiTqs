################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ChipInit.c \
../ComputeModuleFuncts.c \
../Events.c \
../JsonFuncts.c \
../PinDrivers.c \
../UserInterface.c \
../main.c \
../utilityFuncts.c 

OBJS += \
./ChipInit.o \
./ComputeModuleFuncts.o \
./Events.o \
./JsonFuncts.o \
./PinDrivers.o \
./UserInterface.o \
./main.o \
./utilityFuncts.o 

C_DEPS += \
./ChipInit.d \
./ComputeModuleFuncts.d \
./Events.d \
./JsonFuncts.d \
./PinDrivers.d \
./UserInterface.d \
./main.d \
./utilityFuncts.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega645 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


