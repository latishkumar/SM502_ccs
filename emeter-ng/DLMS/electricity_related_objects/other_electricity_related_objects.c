/*
 * other_electricity_related_objects.c
 *
 *  Created on: May 16, 2017
 *      Author: abi
 */
#include "headers.h"
#include "other_electricity_related_objects.h"

/* Active quadrant
 *  0 = creep or no Quadrant detected
 *  1,2,3,4 = Active Quadrant
 */

uint8_t active_quadrant = 0;

/*
 * Phase Presence
 * B0: phase 1; B1:phase 2; B2:phase 3
 */
uint8_t phase_presence = 1;

uint16_t transformer_ratio_voltage_num = 2300; // Value in decivolts (dV) This is the phase-neutral voltage for 1ph

uint16_t transformer_ratio_voltage_den = 2300;   // Value in decivolts (dV) This is the phase-neutral voltage for 1ph

uint16_t transformer_ratio_current_num = 50; // This is the base current (Ib) value in deciamps (dA)

uint16_t transformer_ratio_current_den = 50; // This is the base current (Ib) value in deciamps (dA)
