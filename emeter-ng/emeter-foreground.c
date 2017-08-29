//--------------------------------------------------------------------------
//
//  Software for MSP430 based e-meters.
//
//  THIS PROGRAM IS PROVIDED "AS IS". TI MAKES NO WARRANTIES OR
//  REPRESENTATIONS, EITHER EXPRESS, IMPLIED OR STATUTORY,
//  INCLUDING ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
//  FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
//  COMPLETENESS OF RESPONSES, RESULTS AND LACK OF NEGLIGENCE.
//  TI DISCLAIMS ANY WARRANTY OF TITLE, QUIET ENJOYMENT, QUIET
//  POSSESSION, AND NON-INFRINGEMENT OF ANY THIRD PARTY
//  INTELLECTUAL PROPERTY RIGHTS WITH REGARD TO THE PROGRAM OR
//  YOUR USE OF THE PROGRAM.
//
//  IN NO EVENT SHALL TI BE LIABLE FOR ANY SPECIAL, INCIDENTAL,
//  CONSEQUENTIAL OR INDIRECT DAMAGES, HOWEVER CAUSED, ON ANY
//  THEORY OF LIABILITY AND WHETHER OR NOT TI HAS BEEN ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES, ARISING IN ANY WAY OUT
//  OF THIS AGREEMENT, THE PROGRAM, OR YOUR USE OF THE PROGRAM.
//  EXCLUDED DAMAGES INCLUDE, BUT ARE NOT LIMITED TO, COST OF
//  REMOVAL OR REINSTALLATION, COMPUTER TIME, LABOR COSTS, LOSS
//  OF GOODWILL, LOSS OF PROFITS, LOSS OF SAVINGS, OR LOSS OF
//  USE OR INTERRUPTION OF BUSINESS. IN NO EVENT WILL TI'S
//  AGGREGATE LIABILITY UNDER THIS AGREEMENT OR ARISING OUT OF
//  YOUR USE OF THE PROGRAM EXCEED FIVE HUNDRED DOLLARS
//  (U.S.$500).
//
//  Unless otherwise stated, the Program written and copyrighted
//  by Texas Instruments is distributed as "freeware".  You may,
//  only under TI's copyright in the Program, use and modify the
//  Program without any charge or restriction.  You may
//  distribute to third parties, provided that you transfer a
//  copy of this license to the third party and the third party
//  agrees to these terms by its first use of the Program. You
//  must reproduce the copyright notice and any other legend of
//  ownership on each copy or partial copy, of the Program.
//
//  You acknowledge and agree that the Program contains
//  copyrighted material, trade secrets and other TI proprietary
//  information and is protected by copyright laws,
//  international copyright treaties, and trade secret laws, as
//  well as other intellectual property laws.  To protect TI's
//  rights in the Program, you agree not to decompile, reverse
//  engineer, disassemble or otherwise translate any object code
//  versions of the Program to a human-readable form.  You agree
//  that in no event will you alter, remove or destroy any
//  copyright notice included in the Program.  TI reserves all
//  rights not specifically granted under this license. Except
//  as specifically provided herein, nothing in this agreement
//  shall be construed as conferring by implication, estoppel,
//  or otherwise, upon you, any license or other right under any
//  TI patents, copyrights or trade secrets.
//
//  You may not use the Program in non-TI devices.
//
//  File: emeter-foreground.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: emeter-foreground.c,v 1.43 2009/04/27 09:05:26 a0754793 Exp $
//
/*! \file emeter-structs.h */
//
//--------------------------------------------------------------------------
//
//  MSP430 foreground (non-interrupt) routines for e-meters
//
//  This software is appropriate for single phase and three phase e-meters
//  using a voltage sensor plus a CT or shunt resistor current sensors, or
//  a combination of a CT plus a shunt.
//
//    Foreground process includes:
//    -Using timer tick to wait
//    -Calculating the power per channel
//    -Determine if current channel needs scaling.
//    -Determine if needs to be in low power modes.
//    -Compensate reference from temperature sensor
//
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <io.h>
#include <emeter-toolkit.h>
#include "emeter-structs.h"
#include "configuration_defaults.h"
#include "manufacturer_specific_objects.h"
#include "Status.h"
#include "Tamper.h"
int32_t xxx;
int32_t yyy;

__inline__ long labs(long __x)
{
    return (__x < 0) ? -__x : __x;
}

void set_phase_correction(struct phase_correction_s *s, int correction)
{
	correction += 128;
	s->step = I_HISTORY_STEPS + (correction >> 8);
	correction = 127 - ((correction & 0xFF) >> 1);
	s->fir_beta = fir_coeffs[correction][0];
	s->fir_gain = fir_coeffs[correction][1];
}

#if defined(DYNAMIC_PHASE_CORRECTION_SUPPORT)
static void set_phase_gain_correction(struct phase_correction_s *s, int correction, int gain)
{
    correction += 128;
    s->step = I_HISTORY_STEPS + (correction >> 8);
    correction = 127 - ((correction & 0xFF) >> 1);
    s->fir_beta = fir_coeffs[correction][0];
    s->fir_gain = Q1_15_mul(gain, fir_coeffs[correction][1]);
}
#endif

void set_sd16_phase_correction(struct phase_correction_sd16_s *s, int ph, int correction)
{
    static unsigned short volatile * const sd16_locations[NUM_CURRENT_CHANNELS] =
    {
    	(unsigned short *) &SD16PRE_LIVE,
		(unsigned short *) &SD16PRE_NEUTRAL
    };
    uint8_t bump;

    /* Only try to nudge the converter's
     * timing when in normal operating mode.
     */
    if (operating_mode == OPERATING_MODE_NORMAL)
    {
        if ((bump = s->sd16_preloaded_offset - (correction & 0xFF)))
        {
            *sd16_locations[ph] = bump;
        }
    }
    /*
     * Always store the required correction.
     */
    s->step = I_HISTORY_STEPS - (correction >> 8);
    s->sd16_preloaded_offset = correction;
}

// NEUTRAL_MONITOR_SUPPORT and POWER_BALANCE_DETECTION_SUPPORT
static int32_t test_phase_balance(int64_t live_signal, int64_t neutral_signal, int threshold)
{
    int permitted_imbalance_fraction;

    /* This may be testing between two currents, or between two powers. In normal mode it
       is testing between two power readings. In limp mode it is testing between two
       current readings. */
    /* See which one is bigger, with some tolerance built in. If the signal measured
       from the neutral is more than 6.25% or 12.5% (options) different from the signal
       measured from the live there is something wrong (maybe fraudulent tampering, or
       just something faulty). In this case we use the current measured from the
       channel with the higher signal. When the channel is reasonably balanced, we use
       the signal from the live lead. If neither signal is above the threshold we use
       a more relaxed measure of imbalance (say 25% or even 50%), to allow for the lower
       accuracy of these small signals (remember, we need the test to work well under
       transient conditions, so the threshold needs to be far more relaxed than the
       basic measurement accuracy would indicate). Assessments are persistence checked
       to avoid transient conditions causing a false change of imbalance status. */
    if (live_signal <= threshold  &&  neutral_signal <= threshold)
        permitted_imbalance_fraction = RELAXED_IMBALANCE_FRACTION;
    else
        permitted_imbalance_fraction = permitted_power_imbalance_fraction;
    /* We have a signal strong enough for proper assessment. */
    if ((phase->status & PHASE_UNBALANCED))
    {
        /* We are looking for the restoration of balance between the
           live and neutral. */
        if ((live_signal - (live_signal >> permitted_imbalance_fraction)) < neutral_signal
            &&
            neutral_signal < (live_signal + (live_signal >> permitted_imbalance_fraction)))
        {
            /* Things might be balanced, but persistence check to be sure. */
            if (--current_unbalanced <= -PHASE_UNBALANCED_PERSISTENCE_CHECK)
            {
                /* Things look genuinely balanced. */
                phase->status &= ~(PHASE_UNBALANCED | CURRENT_FROM_NEUTRAL);
                current_unbalanced = 0;
                status.NeutralTamperStatus = 0;
            }
        }
        else
        {
            current_unbalanced = 0;
            /* The imbalance might have swapped around - check. */
            /* Here we just choose the greater signal each block, as we have
                already confirmed (i.e. debounced) the imbalance condition. */
            if (neutral_signal > live_signal)
                phase->status |= CURRENT_FROM_NEUTRAL;
            else
                phase->status &= ~CURRENT_FROM_NEUTRAL;
        }
    }
    else
    {
        /* We are looking for the live and neutral becoming unbalanced. */
        if ((live_signal - (live_signal >> permitted_imbalance_fraction)) > neutral_signal
            ||
            neutral_signal > (live_signal + (live_signal >> permitted_imbalance_fraction)))
        {
            /* Things might be unbalanced, but persistence check to be sure. */
            if (++current_unbalanced >= PHASE_UNBALANCED_PERSISTENCE_CHECK)
            {
                /* Things look genuinely unbalanced. */
                current_unbalanced = 0;
                phase->status |= PHASE_UNBALANCED;
                if ((neutral_signal > live_signal) && (neutral_signal > MIN_ACTIVE_POWER_CONSIDERED_ZERO))
                    phase->status |= CURRENT_FROM_NEUTRAL;
                else
                    phase->status &= ~CURRENT_FROM_NEUTRAL;
                //log neutral tamper
                log_neutral_tamper();
            }
        }
        else
        {
            current_unbalanced = 0;
        }
    }
    /* Clear the unbalanced detection, so we don't display unbalanced. This should eliminate
       flashing of the LED if the are transient conditions causing false indications of
       imbalance. */
    if (live_signal <= threshold  &&  neutral_signal <= threshold)
        phase->status &= ~PHASE_UNBALANCED;
    if (phase->status & CURRENT_FROM_NEUTRAL)
        return  neutral_signal;
    return  live_signal;
}


/*
 * MAINS_FREQUENCY_SUPPORT
 * The mains frequency, in steps of 10mHz
 */
int16_t frequency(void)
{
    int32_t x;

    /* Calculate the mains frequency in 1/100Hz increments, based on the mains
       period assessment from the background activity. */

    /* Normally we get the mains frequency from the voltage. Voltage is always
       present, and is not subject to the same level of distortion as the current
       waveform with difficult loads. */

    /*
    * REACTIVE_POWER_BY_QUADRATURE_SUPPORT
    * We have a whole cycle period in the upper 16 bits, but we want the delay for 90 degrees, so we shift 2
    * extra bits for that.
    */
    x = (phase->metrology.mains_period >> 18);
    set_phase_correction(&phase->metrology.current.quadrature_correction[0], x);

    // NEUTRAL_MONITOR_SUPPORT
    set_phase_correction(&phase->metrology.neutral.quadrature_correction[0], x);

    x = phase->metrology.mains_period;

    x = (int32_t) SAMPLES_PER_10_SECONDS*256L*10L/(x >> 16);
    return  x;
}


int32_t voltage(void)
{
    int16_t i;
    int32_t x;

    /* Calculate the RMS voltage in 10mV increments. Return -1 for over range
       (i.e. ADC clip). */
    if ((phase->status & V_OVERRANGE))
        return -1;
    x = div_sh48(phase->metrology.dot_prod_logged.V_sq, 26 - 2*ADC_BITS, phase->metrology.dot_prod_logged.sample_count);
	i = phase_nv->V_rms_scale_factor;
    x = isqrt32(x);
    x = (x >> 12)*i;
    x >>= 14;
    return x;
}


rms_current_t current(void)
{
    int16_t i;
    rms_current_t x;
    rms_current_t y;
    int64_t tmp;
    
    /* Calculate the RMS current in 1mA increments. Return -1 for over range
       (i.e. ADC clip). A side effect of this routine is it updates the dynamic
       phase correction settings, based on the newly calculated current. */
    /* We always have to work out the properly scaled current from both leads, in
       order to work out the FIR coefficients for the next block. */
    if ((phase->status & I_NEUTRAL_OVERRANGE))
    {
        y = (rms_current_t) ~0;
        phase->metrology.neutral.I_rms = y ;
    }
    else
    {
        tmp = div_ac_current(phase->metrology.neutral.dot_prod_logged.I_sq, phase->metrology.neutral.dot_prod_logged.sample_count);
        if (tmp < phase_nv->current.ac_offset)
        {
            y = 0;
        }
        else
        {
			y = isqrt64(tmp-neutral_nv->ac_offset) >> 26;
			i = neutral_nv->I_rms_scale_factor;
			y = mul48u_32_16(y, i)>>5;
        }
        phase->metrology.neutral.I_rms = y;
    }

    // PER_SENSOR_PRECALCULATED_PARAMETER_SUPPORT)
     phase->metrology.neutral.I_rms = y;


    if ((phase->status & I_OVERRANGE))
    {
        x = (rms_current_t) ~0;
    }
    else
    {
        tmp = div_ac_current(phase->metrology.current.dot_prod_logged.I_sq, phase->metrology.current.dot_prod_logged.sample_count);
        if (tmp < phase_nv->current.ac_offset)
        {
            x = 0;
        }
        else
        {
            tmp -= phase_nv->current.ac_offset;
		    x =  isqrt64(tmp) >> 26;
			i = phase_nv->current.I_rms_scale_factor;
            //Changed so can be consistent with F6736's display of parameters
            x = mul48u_32_16(x, i) >> 10;
        }
    }

	// PRECALCULATED_PARAMETER_SUPPORT
   // phase->metrology.current.I_rms = x;
#ifdef SHUNT_SUPPORT
    phase->readings.I_rms = x;
#endif
    /* The power calculation has provided us which is the appropriate
       current to use. */
    // NEUTRAL_MONITOR_SUPPORT
//    if((phase->status & CURRENT_FROM_NEUTRAL))
//    {
//        x = y;
//    }

    return  x;
}


/*! \brief Calculate the active power in the specified phase, based on the information within that
    phase structure.
    \brief Calculate the active power.
    \return The active power in 10mW increments. */
power_t active_power(void)
{
    int64_t x;
    int64_t y;
    int reversed;

    if (phase->status & (V_OVERRANGE | I_OVERRANGE))
        return (power_t) ~0;

    /* We can only do real power assessment in full operating mode. */
    /* If we have neutral monitoring for a single phase meter, we need to measure
       both power levels, and decide between them. Issues to be assessed here are
       whether one or both leads show reverse power, and whether the power levels
       are balanced. */

    // PHASE_REVERSED_DETECTION_SUPPORT
    /* If we find a negative power level we may be genuinely feeding power to the grid,
       or we may be seeing a tamper condition. This is application dependent. */
    reversed = FALSE;

    x = div_ac_power(phase->metrology.current.dot_prod_logged.P_active, phase->metrology.current.dot_prod_logged.sample_count);
    x >>= 12;
    x = mul48_32_16(x, phase_nv->current.P_scale_factor[0]);
    // PRECALCULATED_PARAMETER_SUPPORT
    phase->metrology.current.active_power = x;

// PHASE_REVERSED_DETECTION_SUPPORT
    if (x < 0)
    {
        // PHASE_REVERSED_IS_TAMPERING
        //x = -x;
        phase->status |= I_REVERSED;
        if (labs(x) > PHASE_REVERSED_THRESHOLD_POWER)
            reversed = TRUE;
    }
    else
    {
        phase->status &= ~I_REVERSED;   
    }

// NEUTRAL_MONITOR_SUPPORT
    y = div_ac_power(phase->metrology.neutral.dot_prod_logged.P_active, phase->metrology.neutral.dot_prod_logged.sample_count);
    y >>= 8;
    y = mul48_32_16(y, neutral_nv->P_scale_factor[0]);
    phase->metrology.neutral.active_power = y;
    #if defined(PER_SENSOR_PRECALCULATED_PARAMETER_SUPPORT)
    phase->metrology.neutral.readings.active_power = y;
    #endif
    // PHASE_REVERSED_DETECTION_SUPPORT
    if (y < 0)
    {
        //PHASE_REVERSED_IS_TAMPERING)
        y = -y;

        phase->status |= I_NEUTRAL_REVERSED;
        if (y > PHASE_REVERSED_THRESHOLD_POWER)
            reversed = TRUE;
    }
    else
    {
        phase->status &= ~I_NEUTRAL_REVERSED;  
    }
#ifdef SHUNT_SUPPORT
/*******************************************************************************************
 * This code section is added because of the reason that a shunt current less than 1A
 * is not in acceptable range
 */
    // calculate Irms current
    current();
    // POWER_BALANCE_DETECTION_SUPPORT
    if(phase->metrology.neutral.I_rms < 1000 && phase->readings.I_rms < 1000)
    {
        //Don't test power balance and reset if neutral tamper is set
        status.NeutralTamperStatus = 0;
        phase->status &= ~(PHASE_UNBALANCED | CURRENT_FROM_NEUTRAL);
        current_unbalanced = 0;
    }
    else
    {
        x = test_phase_balance(x, y, PHASE_UNBALANCED_THRESHOLD_POWER);
    }
    if((phase->status & CURRENT_FROM_NEUTRAL))
    {
        phase->readings.I_rms = phase->metrology .neutral.I_rms;
    }
/************************************************************************************************/
#endif
    // PHASE_REVERSED_DETECTION_SUPPORT
    if ((phase->status & PHASE_REVERSED))
    {
        if (!reversed)
        {
            if (--current_reversed <= -PHASE_REVERSED_PERSISTENCE_CHECK)
            {
                phase->status &= ~PHASE_REVERSED;
                current_reversed = 0;
            }
        }
        else
        {
            current_reversed = 0;
        }
    }
    else
    {
        if (reversed)
        {
            if (++current_reversed >= PHASE_REVERSED_PERSISTENCE_CHECK)
            {
                phase->status |= PHASE_REVERSED;
                current_reversed = 0;
            }
        }
        else
        {
            current_reversed = 0;
        }
    }

    return x;
}

/*
 * Calculate the phase reactive power.
 * return The reactive power in 10mW increments.
 */
power_t reactive_power(void)
{
    int64_t x;
    int16_t i;

    if (phase->status & (V_OVERRANGE | I_OVERRANGE))
        return (power_t) ~0;

    /* We can only do real power assessment in full operating mode. */
    // NEUTRAL_MONITOR_SUPPORT
    /* If we have neutral monitoring for a single phase meter, we need to use whichever
       channel has been selected by the anti-tamper validation scheme. */
    if ((phase->status & CURRENT_FROM_NEUTRAL))
    {
        x = div_ac_power(phase->metrology.neutral.dot_prod_logged.P_reactive, phase->metrology.neutral.dot_prod_logged.sample_count);
        i = Q1_15_mul(neutral_nv->P_scale_factor[0], phase->metrology.neutral.quadrature_correction[0].fir_gain);
        x >>= 6;
        x = mul48_32_16(x, i);
    }
    else

    {
        x = div_ac_power(phase->metrology.current.dot_prod_logged.P_reactive, phase->metrology.current.dot_prod_logged.sample_count);
        i = Q1_15_mul(phase_nv->current.P_scale_factor[0], phase->metrology.current.quadrature_correction[0].fir_gain);
        x >>= 12;
        x = mul48_32_16(x, i);
        x <<= 2;
    }

    #if defined(PER_SENSOR_PRECALCULATED_PARAMETER_SUPPORT)
        #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    phase->metrology.neutral.readings.reactive_power = x;
        #endif
    phase->metrology.current.readings.reactive_power = x;
    #endif
    return  x;//don changed this from x to -x to compensate for current transformer direction
}

/*
 * This function calculates the apparent power (i.e. the simple product of RMS voltage and RMS current),
 * from the information in the phase and phase_nv structures.
 * returns the apparent power in 10mW increments.
 */
int32_t apparent_power(void)
{
    int32_t p;
    int32_t x;
    int shift;

    /* Calculate apparent (VA) power in 0.01W increments */
    p = labs(phase->readings.active_power);
    x = labs(phase->readings.reactive_power);
    /* Justify for optimal accuracy */
    shift = 0;
    while ((p & 0xFFFFC000)  ||  (x & 0xFFFFC000))
    {
        shift++;
        p >>= 1;
        x >>= 1;
    }
    x = isqrt32(p*p + x*x);
    x >>= (16 - shift);

    return x;
}



int16_t power_factor()
{
    int32_t p;
    int32_t x;

    p = labs(phase->readings.active_power);
    x = labs(phase->readings.apparent_power);

    if (p  &&  x)
    {
        /* Justify for optimal accuracy */
        while ((p & 0x40000000) == 0  &&  (x & 0x40000000) == 0)
        {
            p <<= 1;
            x <<= 1;
        }
        x >>= 16;
        p /= x;
        p *= 10000;
        p >>= 16;
        /* Don't let a little imprecision cause strange answers */
        if (p > 10000)
            p = 10000;
    }
    else
    {
        p = 0;
    }
//    /* Use a negative PF to indicate an inductive load */
//    {
//        if (phase->metrology.current.leading < 0)
//            p = -p;
//    }
    return p;
}
/*
 * Calculate average voltage, power factor and frequency
 */
__inline__ void calculate_avergae_v_pf_f()
{
    if(phase->average_counter <= 0)//no data so set to zero
    {
        phase->inc_export_active_energy = 0;    // peak power
        phase->inc_reactive_energy_QI   = 0;    // average voltage
        phase->inc_reactive_energy_QII  = 0;    // average power factor
        phase->inc_reactive_energy_QIII = 0;    // average frequency
    }
    else
    {
        phase->inc_export_active_energy = phase->peak_power/100;                             // peak power
        phase->inc_reactive_energy_QII  = phase->voltage_accum/phase->average_counter;      // average voltage
        phase->inc_reactive_energy_QIII = phase->power_factor_accum/phase->average_counter; // average power factor
        phase->inc_reactive_energy_QIV  = phase->frequency_accum/phase->average_counter;      // average frequency

        //reset accumulators
        phase->peak_power         = 0;
        phase->voltage_accum      = 0;
        phase->power_factor_accum = 0;
        phase->frequency_accum    = 0;
        phase->average_counter    = 0;
    }
}
