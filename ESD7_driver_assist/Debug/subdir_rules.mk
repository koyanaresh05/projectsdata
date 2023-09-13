################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"C:/ti/ccs1220/ccs/tools/compiler/gcc-arm-none-eabi-9-2019-q4-major/bin/arm-none-eabi-gcc-9.2.1.exe" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfpu=fpv4-sp-d16 -DPART_TM4C123GH6PM -I"C:/Users/PredatorF-25.LOVE-THE-PREDAT/OneDrive/Desktop/Niladri/SOFTWARE/CCS/ESD7_driver_assist" -I"C:/ti/TivaWare_C_Series-2.1.4.178" -I"C:/ti/ccs1220/ccs/tools/compiler/gcc-arm-none-eabi-9-2019-q4-major/arm-none-eabi/include" -O0 -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -specs="nosys.specs" -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


