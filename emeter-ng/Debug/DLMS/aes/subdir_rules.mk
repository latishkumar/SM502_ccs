################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
DLMS/aes/TI_aes.obj: ../DLMS/aes/TI_aes.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccs62/ccsv6/tools/compiler/msp430_15.12.3.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="C:/ti/ccs62/ccsv6/ccs_base/msp430/include" --include_path="M:/sm502-CCS/final/SM502_ccs/emeter-toolkit" --include_path="C:/ti/ccs62/ccsv6/tools/compiler/msp430_15.12.3.LTS/include" --advice:power=all -g --define=__MSP430F6736__ --define=CUSTOM_EMETER_H="emeter-1ph-neutral-6733_shunt.h" --define=CHUNK_SIZE=100 --define=DLMS_IEC_21 --define=DLMS --define=EEPROM_REV_2 --define=ALARM_ON_TAMPER --define=PHYSICAL_RELAY_DISCONNECT_ENABLED --define=OPT_INT --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="DLMS/aes/TI_aes.d" --obj_directory="DLMS/aes" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


