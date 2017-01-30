################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
ASM_SRCS += \
../firmware_loader/eeprom_read.asm \
../firmware_loader/flash_programmer.asm \
../firmware_loader/loader.asm \
../firmware_loader/power_handler.asm 

C_SRCS += \
../firmware_loader/eeprom_read_interface.c \
../firmware_loader/firmware_loader.c 

OBJS += \
./firmware_loader/eeprom_read.obj \
./firmware_loader/eeprom_read_interface.obj \
./firmware_loader/firmware_loader.obj \
./firmware_loader/flash_programmer.obj \
./firmware_loader/loader.obj \
./firmware_loader/power_handler.obj 

ASM_DEPS += \
./firmware_loader/eeprom_read.d \
./firmware_loader/flash_programmer.d \
./firmware_loader/loader.d \
./firmware_loader/power_handler.d 

C_DEPS += \
./firmware_loader/eeprom_read_interface.d \
./firmware_loader/firmware_loader.d 

C_DEPS__QUOTED += \
"firmware_loader/eeprom_read_interface.d" \
"firmware_loader/firmware_loader.d" 

OBJS__QUOTED += \
"firmware_loader/eeprom_read.obj" \
"firmware_loader/eeprom_read_interface.obj" \
"firmware_loader/firmware_loader.obj" \
"firmware_loader/flash_programmer.obj" \
"firmware_loader/loader.obj" \
"firmware_loader/power_handler.obj" 

ASM_DEPS__QUOTED += \
"firmware_loader/eeprom_read.d" \
"firmware_loader/flash_programmer.d" \
"firmware_loader/loader.d" \
"firmware_loader/power_handler.d" 

ASM_SRCS__QUOTED += \
"../firmware_loader/eeprom_read.asm" \
"../firmware_loader/flash_programmer.asm" \
"../firmware_loader/loader.asm" \
"../firmware_loader/power_handler.asm" 

C_SRCS__QUOTED += \
"../firmware_loader/eeprom_read_interface.c" \
"../firmware_loader/firmware_loader.c" 


