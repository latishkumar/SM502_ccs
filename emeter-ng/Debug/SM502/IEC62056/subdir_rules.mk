################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
SM502/IEC62056/iec62056_uart.obj: ../SM502/IEC62056/iec62056_uart.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"/home/abi/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.3.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="/home/abi/ti/ccsv7/ccs_base/msp430/include" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/DLMS/aes" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/DLMS/electricity_related_objects" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/DLMS/abstract_objects" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/SM502/PLC" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/SM502/IEC62056" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/SM502" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/DLMS/core" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/DLMS/src" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-toolkit" --include_path="/home/abi/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.3.LTS/include" --advice:power=all -g --undefine=_INLINE --define=__MSP430F6736__ --define=PHYSICAL_RELAY_DISCONNECT_ENABLED --define=CUSTOM_EMETER_H="emeter-1ph-neutral-6733_shunt.h" --define=CHUNK_SIZE=100 --define=DLMS_IEC_21 --define=DLMS --define=EEPROM_REV_2 --define=ALARM_ON_TAMPER --define=OPT_INT --diag_warning=225 --display_error_number --diag_wrap=off --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="SM502/IEC62056/iec62056_uart.d" --obj_directory="SM502/IEC62056" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '


