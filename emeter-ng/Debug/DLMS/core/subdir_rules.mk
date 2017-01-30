################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
DLMS/core/iec62056_46_link.obj: ../DLMS/core/iec62056_46_link.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"/home/eliab/ti/ccsv6/tools/compiler/ti-cgt-msp430_15.12.1.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="/home/eliab/ti/ccsv6/ccs_base/msp430/include" --include_path="/home/eliab/workspace/SM502_firmware_ccs/SM502_ccs/emeter-toolkit" --include_path="/home/eliab/ti/ccsv6/tools/compiler/ti-cgt-msp430_15.12.1.LTS/include" --advice:power=all -g --define=__MSP430F6736__ --define=CUSTOM_EMETER_H="emeter-1ph-neutral-6733_shunt.h" --define=CHUNK_SIZE=100 --define=DLMS_IEC_21 --define=DLMS --define=EEPROM_REV_2 --define=ALARM_ON_TAMPER --define=PHYSICAL_RELAY_DISCONNECT_ENABLED --define=OPT_INT --diag_wrap=off --display_error_number --diag_warning=225 --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="DLMS/core/iec62056_46_link.d" --obj_directory="DLMS/core" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

DLMS/core/packing.obj: ../DLMS/core/packing.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"/home/eliab/ti/ccsv6/tools/compiler/ti-cgt-msp430_15.12.1.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="/home/eliab/ti/ccsv6/ccs_base/msp430/include" --include_path="/home/eliab/workspace/SM502_firmware_ccs/SM502_ccs/emeter-toolkit" --include_path="/home/eliab/ti/ccsv6/tools/compiler/ti-cgt-msp430_15.12.1.LTS/include" --advice:power=all -g --define=__MSP430F6736__ --define=CUSTOM_EMETER_H="emeter-1ph-neutral-6733_shunt.h" --define=CHUNK_SIZE=100 --define=DLMS_IEC_21 --define=DLMS --define=EEPROM_REV_2 --define=ALARM_ON_TAMPER --define=PHYSICAL_RELAY_DISCONNECT_ENABLED --define=OPT_INT --diag_wrap=off --display_error_number --diag_warning=225 --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="DLMS/core/packing.d" --obj_directory="DLMS/core" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

DLMS/core/xframe.obj: ../DLMS/core/xframe.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"/home/eliab/ti/ccsv6/tools/compiler/ti-cgt-msp430_15.12.1.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="/home/eliab/ti/ccsv6/ccs_base/msp430/include" --include_path="/home/eliab/workspace/SM502_firmware_ccs/SM502_ccs/emeter-toolkit" --include_path="/home/eliab/ti/ccsv6/tools/compiler/ti-cgt-msp430_15.12.1.LTS/include" --advice:power=all -g --define=__MSP430F6736__ --define=CUSTOM_EMETER_H="emeter-1ph-neutral-6733_shunt.h" --define=CHUNK_SIZE=100 --define=DLMS_IEC_21 --define=DLMS --define=EEPROM_REV_2 --define=ALARM_ON_TAMPER --define=PHYSICAL_RELAY_DISCONNECT_ENABLED --define=OPT_INT --diag_wrap=off --display_error_number --diag_warning=225 --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="DLMS/core/xframe.d" --obj_directory="DLMS/core" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '


