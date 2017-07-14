################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
DLMS/src/Billing_Data.obj: ../DLMS/src/Billing_Data.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"/home/abi/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.3.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="/home/abi/ti/ccsv7/ccs_base/msp430/include" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/DLMS/aes" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/DLMS/electricity_related_objects" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/DLMS/abstract_objects" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/SM502/PLC" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/SM502/IEC62056" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/SM502" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/DLMS/core" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/DLMS/src" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-toolkit" --include_path="/home/abi/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.3.LTS/include" --advice:power=all -g --undefine=_INLINE --define=__MSP430F6736__ --define=PHYSICAL_RELAY_DISCONNECT_ENABLED --define=CUSTOM_EMETER_H="emeter-1ph-neutral-6733_shunt.h" --define=CHUNK_SIZE=100 --define=DLMS_IEC_21 --define=DLMS --define=EEPROM_REV_2 --define=ALARM_ON_TAMPER --define=OPT_INT --diag_warning=225 --display_error_number --diag_wrap=off --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="DLMS/src/Billing_Data.d" --obj_directory="DLMS/src" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

DLMS/src/DLMS.obj: ../DLMS/src/DLMS.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"/home/abi/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.3.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="/home/abi/ti/ccsv7/ccs_base/msp430/include" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/DLMS/aes" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/DLMS/electricity_related_objects" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/DLMS/abstract_objects" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/SM502/PLC" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/SM502/IEC62056" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/SM502" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/DLMS/core" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/DLMS/src" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-toolkit" --include_path="/home/abi/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.3.LTS/include" --advice:power=all -g --undefine=_INLINE --define=__MSP430F6736__ --define=PHYSICAL_RELAY_DISCONNECT_ENABLED --define=CUSTOM_EMETER_H="emeter-1ph-neutral-6733_shunt.h" --define=CHUNK_SIZE=100 --define=DLMS_IEC_21 --define=DLMS --define=EEPROM_REV_2 --define=ALARM_ON_TAMPER --define=OPT_INT --diag_warning=225 --display_error_number --diag_wrap=off --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="DLMS/src/DLMS.d" --obj_directory="DLMS/src" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

DLMS/src/SharedVariables.obj: ../DLMS/src/SharedVariables.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"/home/abi/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.3.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="/home/abi/ti/ccsv7/ccs_base/msp430/include" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/DLMS/aes" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/DLMS/electricity_related_objects" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/DLMS/abstract_objects" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/SM502/PLC" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/SM502/IEC62056" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/SM502" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/DLMS/core" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/DLMS/src" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-toolkit" --include_path="/home/abi/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.3.LTS/include" --advice:power=all -g --undefine=_INLINE --define=__MSP430F6736__ --define=PHYSICAL_RELAY_DISCONNECT_ENABLED --define=CUSTOM_EMETER_H="emeter-1ph-neutral-6733_shunt.h" --define=CHUNK_SIZE=100 --define=DLMS_IEC_21 --define=DLMS --define=EEPROM_REV_2 --define=ALARM_ON_TAMPER --define=OPT_INT --diag_warning=225 --display_error_number --diag_wrap=off --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="DLMS/src/SharedVariables.d" --obj_directory="DLMS/src" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

DLMS/src/config_new.obj: ../DLMS/src/config_new.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"/home/abi/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.3.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="/home/abi/ti/ccsv7/ccs_base/msp430/include" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/DLMS/aes" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/DLMS/electricity_related_objects" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/DLMS/abstract_objects" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/SM502/PLC" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/SM502/IEC62056" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/SM502" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/DLMS/core" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/DLMS/src" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-toolkit" --include_path="/home/abi/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.3.LTS/include" --advice:power=all -g --undefine=_INLINE --define=__MSP430F6736__ --define=PHYSICAL_RELAY_DISCONNECT_ENABLED --define=CUSTOM_EMETER_H="emeter-1ph-neutral-6733_shunt.h" --define=CHUNK_SIZE=100 --define=DLMS_IEC_21 --define=DLMS --define=EEPROM_REV_2 --define=ALARM_ON_TAMPER --define=OPT_INT --diag_warning=225 --display_error_number --diag_wrap=off --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="DLMS/src/config_new.d" --obj_directory="DLMS/src" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

DLMS/src/uart_comms.obj: ../DLMS/src/uart_comms.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"/home/abi/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.3.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="/home/abi/ti/ccsv7/ccs_base/msp430/include" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/DLMS/aes" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/DLMS/electricity_related_objects" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/DLMS/abstract_objects" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/SM502/PLC" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/SM502/IEC62056" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/SM502" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/DLMS/core" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng/DLMS/src" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-ng" --include_path="/home/abi/Desktop/Arada firmware v0003-new/SM502_ccs/emeter-toolkit" --include_path="/home/abi/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.3.LTS/include" --advice:power=all -g --undefine=_INLINE --define=__MSP430F6736__ --define=PHYSICAL_RELAY_DISCONNECT_ENABLED --define=CUSTOM_EMETER_H="emeter-1ph-neutral-6733_shunt.h" --define=CHUNK_SIZE=100 --define=DLMS_IEC_21 --define=DLMS --define=EEPROM_REV_2 --define=ALARM_ON_TAMPER --define=OPT_INT --diag_warning=225 --display_error_number --diag_wrap=off --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="DLMS/src/uart_comms.d" --obj_directory="DLMS/src" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

