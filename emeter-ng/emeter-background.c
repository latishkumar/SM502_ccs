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
//  File: emeter-background.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: emeter-background.c,v 1.50 2009/04/23 06:44:30 a0754793 Exp $
//
/*! \file emeter-structs.h */
//
//--------------------------------------------------------------------------
//
//  MSP430 background (interrupt) routines for e-meters
//
//  This software is appropriate for single phase and three phase e-meters
//  using a voltage sensor plus a CT or shunt resistor current sensors, or
//  a combination of a CT plus a shunt.
//
//    The background process deals with the input samples.
//    These are first stored into buffers.
//    The buffered samples are processed as follows:
//    -Voltage and current signals are converted to DC-less AC signals
//    -The current signal is phase compensated
//    -Voltage and current are signed multiplied to give power.
//    -Power samples are accumulated. The accumulated power samples are averaged (in foreground.c)
//     after a number of voltage cycles has been detected.
//
#include <configuration_defaults.h>
#include <stdint.h>
#include <stdlib.h>
#include <msp430.h>
#include <emeter-toolkit.h>
#include "SM502/Status.h"
#include "emeter-structs.h"

int16_t samples_per_second;

uint16_t adc_interrupts;

static int32_t sample_count = 0;

uint16_t battery_countdown;

extern struct neutral_parms_s neutral_c;


static __inline__ void log_parameters(void)
{
#if GAIN_STAGES > 1
    int i;
#else
#define i 0
#endif

    /* Take a snapshot of various values for logging purposes; tell the
       foreground to deal with them; and clear the working values ready
       for the next analysis period. */
    if (phase->metrology.V_endstops <= 0)
        phase->status |= V_OVERRANGE;
    else
        phase->status &= ~V_OVERRANGE;
    phase->metrology.V_endstops = ENDSTOP_HITS_FOR_OVERLOAD;

    // VRMS_SUPPORT
    transfer48(phase->metrology.dot_prod_logged.V_sq, phase->metrology.dot_prod.V_sq);

    if (phase->metrology.current.I_endstops <= 0)
        phase->status |= I_OVERRANGE;
    else
        phase->status &= ~I_OVERRANGE;
    phase->metrology.current.I_endstops = ENDSTOP_HITS_FOR_OVERLOAD;

    // NEUTRAL_MONITOR_SUPPORT
    if (phase->metrology.neutral.I_endstops <= 0)
        phase->status |= I_NEUTRAL_OVERRANGE;
    else
        phase->status &= ~I_NEUTRAL_OVERRANGE;
    phase->metrology.neutral.I_endstops = ENDSTOP_HITS_FOR_OVERLOAD;

    phase->metrology.current.dot_prod_logged.sample_count = phase->metrology.current.dot_prod.sample_count;
    phase->metrology.current.dot_prod.sample_count = 0;

    // NEUTRAL_MONITOR_SUPPORT
    phase->metrology.neutral.dot_prod_logged.sample_count = phase->metrology.neutral.dot_prod.sample_count;
	phase->metrology.neutral.dot_prod.sample_count = 0;


#if GAIN_STAGES > 1
    for (i = 0;  i < GAIN_STAGES;  i++)
#endif
    {

        // IRMS_SUPPORT
        transfer_ac_current(phase->metrology.current.dot_prod_logged.I_sq, phase->metrology.current.dot_prod.I_sq);
        transfer_ac_power(phase->metrology.current.dot_prod_logged.P_active, phase->metrology.current.dot_prod.P_active);

        // REACTIVE_POWER_BY_QUADRATURE_SUPPORT
        transfer_ac_power(phase->metrology.current.dot_prod_logged.P_reactive, phase->metrology.current.dot_prod.P_reactive);

        // NEUTRAL_MONITOR_SUPPORT
        transfer_ac_current(phase->metrology.neutral.dot_prod_logged.I_sq, phase->metrology.neutral.dot_prod.I_sq);
        transfer_ac_power(phase->metrology.neutral.dot_prod_logged.P_active, phase->metrology.neutral.dot_prod.P_active);

        // REACTIVE_POWER_BY_QUADRATURE_SUPPORT
        transfer_ac_power(phase->metrology.neutral.dot_prod_logged.P_reactive, phase->metrology.neutral.dot_prod.P_reactive);
    }
    
    phase->metrology.dot_prod_logged.sample_count = phase->metrology.dot_prod.sample_count;
    phase->metrology.dot_prod.sample_count = 0;

    /* Tell the foreground there are things to process. */
    phase->status |= NEW_LOG;
#if GAIN_STAGES <= 1
#undef i
#endif
}

extern  void perform_low_battry_backup();
extern MeterStatus status;

#define Max_F_Power_Faile 246
uint16_t F_Count=0;


/*---------------------------------------------------------------------------
  This is the main interrupt routine where the main signal processing is done
  ---------------------------------------------------------------------------*/
ISR(SD24B, adc_interrupt)
{
    int16_t V_sample;
    int16_t V_corrected;
    int16_t V_quad_corrected;
    current_sample_t corrected;
    current_sample_t I_live_sample;
    current_sample_t I_neutral_sample;

#define i 0
#define use_stage 0

    //static int16_t adc_buffer[3];
    static int16_t adc_v_buffer[1];
    static current_sample_t adc_i_buffer[2];
    int adc_ptr;

    int k;
    int x;
    int y;
    int z;

    sample_count++;

    if (!(TST_SD16IFG_VOLTAGE))
    {
        /* We do not have a complete set of samples yet, but we may need to pick
           up some current values at this time */
    	// NEUTRAL_MONITOR_SUPPORT)
        if ((TST_SD16IFG_NEUTRAL))
        {
            adc_i_buffer[1] = SD16MEM_NEUTRAL;
            CLR_SD16IFG_NEUTRAL;
        }

        if ((TST_SD16IFG_LIVE))
        {
            adc_i_buffer[0] = SD16MEM_LIVE;
            CLR_SD16IFG_LIVE;
        }
        return;
    }

    /* Filter away the DC bias.

       Do the phase lag compensation. Use a simple FIR approach,
       and absorb the non-unity gain of the filter in the overall
       current/power scaling later on. This is OK for the small
       phase shifts we expect to get. It would cause dynamic
       range problems for larger shifts. Note the some of this
       phase shift is due to the operation of the ADC itself. It
       performs sequential conversions of its 8 inputs, so there is
       some time delay between sampling of the various sensors.

       Accumulate power for each of the channels. These will
       be divided by the number of samples at the end of the
       measurement cycles, resulting in an average power
       value for each source.

       If RMS voltage and/or current readings are required, calculate the
       dot products needed to evaluate these. */


    /* Voltage is available */

    adc_v_buffer[0] = SD16MEM_VOLTAGE;
    CLR_SD16IFG_VOLTAGE;

    /* Pick up any current samples which may have occurred a little before the
       voltage sample, but not those which may have occurred just after the
       voltage sample. */

    // NEUTRAL_MONITOR_SUPPORT
    if (phase->metrology.neutral.in_phase_correction[0].sd16_preloaded_offset < 128  &&  (TST_SD16IFG_NEUTRAL))
    {
        adc_i_buffer[1] = SD16MEM_NEUTRAL;
        CLR_SD16IFG_NEUTRAL;
    }

    if (phase->metrology.current.in_phase_correction[0].sd16_preloaded_offset < 128  &&  (TST_SD16IFG_LIVE))
    {
        adc_i_buffer[0] = SD16MEM_LIVE;
        CLR_SD16IFG_LIVE;
    }

    /* We have a complete set of samples. Process them. */
    adc_ptr = -1;

    kick_watchdog();

    adc_ptr++;
    V_sample = adc_v_buffer[0];
      
    if ((V_sample >= V_ADC_MAX  ||  V_sample <= V_ADC_MIN)  &&  phase->metrology.V_endstops)
			phase->metrology.V_endstops--;

    V_sample = dc_filter_voltage(phase->metrology.V_dc_estimate, V_sample);

	// VRMS_SUPPORT
    sqac_voltage(phase->metrology.dot_prod.V_sq, V_sample);

    // REACTIVE_POWER_BY_QUADRATURE_SUPPORT & PHASE_CORRECTION_SUPPORT
	/* We need to save the history of the voltage signal if we are performing phase correction, and/or
	   measuring the quadrature shifted power (to obtain an accurate measure of one form of the reactive power). */
    phase->metrology.V_history[(int) phase->metrology.V_history_index] = V_sample;

    I_live_sample = dc_filter_current(phase->metrology.current.I_dc_estimate[0], phase->metrology.current.I_history[0][0]);
    adc_ptr++;
    corrected = adc_i_buffer[0];

    if ((corrected >= I_ADC_MAX  ||  corrected <= I_ADC_MIN)  &&  phase->metrology.current.I_endstops)
			phase->metrology.current.I_endstops--;

    phase->metrology.current.I_history[0][0] = corrected;

    // IRMS_SUPPORT  & POWER_FACTOR_SUPPORT
    sqac_current(phase->metrology.current.dot_prod.I_sq, I_live_sample);

    if (operating_mode == OPERATING_MODE_NORMAL)
    {
		/* Perform phase shift compensation, to allow for the time
		   between ADC samplings, internal phase shifts in CTs, etc.
		   This uses a 1 tap FIR (basically an interpolator/extrapolator) */

    	// PHASE_CORRECTION_SUPPORT
    	V_corrected = phase->metrology.V_history[(phase->metrology.V_history_index - phase->metrology.current.in_phase_correction[use_stage].step) & V_HISTORY_MASK];

    	mac_power(phase->metrology.current.dot_prod.P_active, V_corrected, I_live_sample);

	    // REACTIVE_POWER_BY_QUADRATURE_SUPPORT
		V_quad_corrected = (Q1_15_mul(phase->metrology.V_history[(phase->metrology.V_history_index - phase->metrology.current.quadrature_correction[use_stage].step - 1) & V_HISTORY_MASK], phase->metrology.current.quadrature_correction[use_stage].fir_beta) >> 1)
                                  + (phase->metrology.V_history[(phase->metrology.V_history_index - phase->metrology.current.quadrature_correction[use_stage].step) & V_HISTORY_MASK] >> 1);

        mac_power(phase->metrology.current.dot_prod.P_reactive, V_quad_corrected, I_live_sample);

	}
	++phase->metrology.current.dot_prod.sample_count;

	// NEUTRAL_MONITOR_SUPPORT
	I_neutral_sample = dc_filter_current(phase->metrology.neutral.I_dc_estimate[0], phase->metrology.neutral.I_history[0][0]);
    corrected = adc_i_buffer[1];
    if ((corrected >= I_ADC_MAX  ||  corrected <= I_ADC_MIN)  &&  phase->metrology.neutral.I_endstops)
    {
        phase->metrology.neutral.I_endstops--;
    }
    phase->metrology.neutral.I_history[0][0] = corrected;
    sqac_current(phase->metrology.neutral.dot_prod.I_sq, I_neutral_sample);
    if (operating_mode == OPERATING_MODE_NORMAL)
    {

        V_corrected = phase->metrology.V_history[(phase->metrology.V_history_index - phase->metrology.neutral.in_phase_correction[use_stage].step) & V_HISTORY_MASK];
        mac_power(phase->metrology.neutral.dot_prod.P_active, V_corrected, I_neutral_sample);

        V_quad_corrected = (Q1_15_mul(phase->metrology.V_history[(phase->metrology.V_history_index - phase->metrology.neutral.quadrature_correction[use_stage].step - 1) & V_HISTORY_MASK], phase->metrology.neutral.quadrature_correction[use_stage].fir_beta) >> 1)
                                          + (phase->metrology.V_history[(phase->metrology.V_history_index - phase->metrology.neutral.quadrature_correction[use_stage].step) & V_HISTORY_MASK] >> 1);

        mac_power(phase->metrology.neutral.dot_prod.P_reactive, V_quad_corrected, I_neutral_sample);
    }
    ++phase->metrology.neutral.dot_prod.sample_count;

	// REACTIVE_POWER_BY_QUADRATURE_SUPPORT & PHASE_CORRECTION_SUPPORT
    phase->metrology.V_history_index = (phase->metrology.V_history_index + 1) & V_HISTORY_MASK;

    ++phase->metrology.dot_prod.sample_count;

    // PER_PHASE_ACTIVE_ENERGY_SUPPORT
    /* We now play the last measurement interval's power level, evaluated
       in the foreground, through this measurement interval. In this way
       we can evenly pace the pulsing of the LED. The only error produced
       by this is the ambiguity in the number of samples per measurement.
       This should not exceed 1 or 2 in over 4000. */

    if ((phase->active_power_counter += phase->readings.active_power) >= PHASE_ENERGY_PULSE_THRESHOLD)
    {
        phase->active_power_counter -= PHASE_ENERGY_PULSE_THRESHOLD;
        //++phase->consumed_active_energy;
        /* Ideally we want to log the energy each kWh unit, but doing
        it with a mask here is good enough and faster. */
        //if ((phase->consumed_active_energy & 0x3FF) == 0)
        // phase->status |= ENERGY_LOGABLE;
        /* Pulse the indicator. Long pulses may not be reliable, as at full
        power we may be pulsing several times per second. People may
        check the meter's calibration with an instrument that counts
        the pulsing rate, so it is important the pulses are clear,
        distinct, and exactly at the rate of one per
        1/ENERGY_PULSES_PER_KW_HOUR kW/h. */

        phase_active_energy_pulse_start();
        phase->active_energy_pulse_remaining_time = ENERGY_PULSE_DURATION;
    }

    if (phase->active_energy_pulse_remaining_time  &&  --phase->active_energy_pulse_remaining_time == 0)
    {
        phase_active_energy_pulse_end();
    }

    F_Count ++;
    if(F_Count > Max_F_Power_Faile && status.LogEnergyLogged == 0 )
    {
        perform_low_battery_backup();   //Power Failed
    }

    /* Do the power cycle start detection */
    /* There is no hysteresis used here, but since the signal is
       changing rapidly at the zero crossings, and is always of
       large amplitude, miscounting cycles due to general noise
       should not occur. Spikes are another matter. A large spike
       could cause the power cycles to be miscounted, but does not
       matter very much. The cycle counting is not critical to power
       or energy measurement. */

    //MAINS_FREQUENCY_SUPPORT
    phase->metrology.cycle_sample_count += 256;

    if (abs(V_sample - phase->metrology.last_V_sample) <= phase->metrology.since_last*MAX_PER_SAMPLE_VOLTAGE_SLEW)
    {
        /* This doesn't look like a spike - do mains cycle detection, and
        estimate the precise mains period */
        if (V_sample < 0)
        {
            /* Log the sign of the signal */
            phase->status &= ~V_POS;
        }
        else
        {
            if (!(phase->status & V_POS))
            {
                F_Count=0;

                /* Apply limits to the sample count, to avoid spikes or dying power lines disturbing the
                frequency reading too much */
                /* The mains should be <40Hz or >70Hz to fail this test! */
                if (256*SAMPLES_PER_10_SECONDS/700 <= phase->metrology.cycle_sample_count  &&  phase->metrology.cycle_sample_count <= 256*SAMPLES_PER_10_SECONDS/400)
                {
                    /* A mains frequency measurement procedure based on interpolating zero crossings,
                    to get a fast update rate for step changes in the mains frequency */
                    /* Interpolate the zero crossing by successive approx. */
                    z = V_sample - phase->metrology.last_V_sample;
                    x = 0;
                    y = 0;
                    for (k = 0;  k < 8;  k++)
                    {
                        y <<= 1;
                        z >>= 1;
                        x += z;
                        if (x > V_sample)
                        x -= z;
                        else
                        y |= 1;
                    }
                    /* Now we need to allow for skipped samples, due to spike detection */
                    z = y;
                    while (phase->metrology.since_last > 1)
                    {
                        z += y;
                        phase->metrology.since_last--;
                    }
                    /* z is now the fraction of a sample interval between the zero
                    crossing and the current sample, in units of 1/256 of a sample */
                    /* A lightly damped filter should now be enough to remove noise and get a
                    stable value for the frequency */
                    phase->metrology.mains_period += ((int32_t) (phase->metrology.cycle_sample_count - z) << 12) - (phase->metrology.mains_period >> 4);
                    /* Start the next cycle with the residual fraction of a sample */
                    phase->metrology.cycle_sample_count = z;
                }
                else
                {
                    phase->metrology.cycle_sample_count = 0;
                }

                // POWER_FACTOR_SUPPORT
                /* Determine whether the current leads or lags, in a noise tolerant manner.
                Testing 50 cycles means we will respond in about one second to a genuine
                swap between lead and lag. Since that is also about the length of our
                measurement blocks, this seems a sensible response time. */
                if (I_live_sample < V_sample)
                {
                    if (phase->metrology.current.leading > -50)
                    phase->metrology.current.leading--;
                }
                else
                {
                    if (phase->metrology.current.leading < 50)
                    phase->metrology.current.leading++;
                }
                // NEUTRAL_MONITOR_SUPPORT
                if (I_neutral_sample < V_sample)
                {
                    if (phase->metrology.neutral.leading > -50)
                    phase->metrology.neutral.leading--;
                }
                else
                {
                    if (phase->metrology.neutral.leading < 50)
                    phase->metrology.neutral.leading++;
                }

                /* See if a sufficiently long measurement interval has been
                recorded, and catch the start of the next cycle. We do not
                really care how many cycles there are, as long as the block
                is a reasonable length. Setting a minimum of 1 second is
                better than counting cycles, as it is not affected by noise
                spikes. Synchronizing to a whole number of cycles reduces
                block to block jitter, though it doesn't affect the long
                term accuracy of the measurements. */
                if (phase->metrology.dot_prod.sample_count >= samples_per_second)
                {
                    log_parameters();
                    /* The foreground may be conserving power (e.g. in limp mode), so we
                    need to kick it. */
                    _BIC_SR_IRQ(LPM0_bits);
                }
            }

            /* Log the sign of the signal */
            phase->status |= V_POS;
        }

        phase->metrology.since_last = 0;
        phase->metrology.last_V_sample = V_sample;
    }

    phase->metrology.since_last++;

    if (phase->metrology.dot_prod.sample_count >= samples_per_second + 200)
    {
        /* We don't seem to be detecting the end of a mains cycle, so force
        the end of processing block condition. */
        log_parameters();
        /* The foreground may be conserving power (e.g. in limp mode), so we
        need to kick it. */
        _BIC_SR_IRQ(LPM0_bits);
    }

    // MAINS_FREQUENCY_SUPPORT
    /* Monitor the cycles and frequency of the current sensors, as limp
       mode is based on these. */
    phase->metrology.current.cycle_sample_count += 256;

    if (I_live_sample < 0)
    {
        /* Log the sign of the signal */
        phase->status &= ~I_POS;
    }
    else
    {
        if (!(phase->status & I_POS))
        {
            /* A negative to positive transition has occurred. Trust it
               blindly as a genuine zero crossing/start of cycle, even
               though it might really be due to a noise spike. */
            // MAINS_FREQUENCY_SUPPORT
            if (SAMPLES_PER_10_SECONDS/700 <= phase->metrology.current.cycle_sample_count  &&  phase->metrology.current.cycle_sample_count <= SAMPLES_PER_10_SECONDS/400)
                phase->metrology.current.mains_period += ((int32_t) phase->metrology.current.cycle_sample_count << 16) - (phase->metrology.current.mains_period >> 8);
            phase->metrology.current.cycle_sample_count = 0;
        }
        /* Log the sign of the signal */
        phase->status |= I_POS;
    }

    // NEUTRAL_MONITOR_SUPPORT
    // MAINS_FREQUENCY_SUPPORT
    phase->metrology.neutral.cycle_sample_count++;
    if (I_neutral_sample < 0)
    {
        /* Log the sign of the signal */
        phase->status &= ~I_NEUTRAL_POS;
    }
    else
    {
        if (!(phase->status & I_NEUTRAL_POS))
        {
            /* A negative to positive transition has occurred. Trust it
               blindly as a genuine zero crossing/start of cycle, even
               though it might really be due to a noise spike. */

            // MAINS_FREQUENCY_SUPPORT
            if (SAMPLES_PER_10_SECONDS/700 <= phase->metrology.neutral.cycle_sample_count  &&  phase->metrology.neutral.cycle_sample_count <= SAMPLES_PER_10_SECONDS/400)
                phase->metrology.neutral.mains_period += ((int32_t) phase->metrology.neutral.cycle_sample_count << 16) - (phase->metrology.neutral.mains_period >> 8);
            phase->metrology.neutral.cycle_sample_count = 0;
        }
        /* Log the sign of the signal */
        phase->status |= I_NEUTRAL_POS;
    }

    // TOTAL_ACTIVE_ENERGY_SUPPORT
    /* We now play the last measurement interval's power level, evaluated
       in the foreground, through this measurement interval. In this way
       we can evenly pace the pulsing of the LED. The only error produced
       by this is the ambiguity in the number of samples per measurement.
       This should not exceed 1 or 2 in over 4000. */

    if ((total_active_power_counter += total_active_power) >= TOTAL_ENERGY_PULSE_THRESHOLD)
    {
        total_active_power_counter -= TOTAL_ENERGY_PULSE_THRESHOLD;
        ++total_consumed_active_energy;
        /* Ideally we want to log the energy each kWh unit, but doing
           it with a mask here is good enough and faster. */
        if ((total_consumed_active_energy & 0x3FF) == 0)
            phase->status |= ENERGY_LOGABLE;
        /* Pulse the LED. Long pulses may not be reliable, as at full
           power we may be pulsing many times per second. People may
           check the meter's calibration with an instrument that counts
           the pulsing rate, so it is important the pulses are clear,
           distinct, and exactly at the rate of one per
           1/ENERGY_PULSES_PER_KW_HOUR kW/h. */

        total_active_energy_pulse_start();
        total_active_energy_pulse_remaining_time = ENERGY_PULSE_DURATION;
    }

    if (total_active_energy_pulse_remaining_time  &&  --total_active_energy_pulse_remaining_time == 0)
    {
        total_active_energy_pulse_end();
    }

    if ((total_reactive_power_counter += total_reactive_power) >= TOTAL_ENERGY_PULSE_THRESHOLD)
    {
        total_reactive_power_counter -= TOTAL_ENERGY_PULSE_THRESHOLD;
        ++total_consumed_reactive_energy;
        /* Ideally we want to log the energy each kWh unit, but doing
           it with a mask here is good enough and faster. */
        if ((total_consumed_reactive_energy & 0x3FF) == 0)
            phase->status |= ENERGY_LOGABLE;
        /* Pulse the LED. Long pulses may not be reliable, as at full
           power we may be pulsing many times per second. People may
           check the meter's calibration with an instrument that counts
           the pulsing rate, so it is important the pulses are clear,
           distinct, and exactly at the rate of one per
           1/ENERGY_PULSES_PER_KW_HOUR kW/h. */
        total_reactive_energy_pulse_start();
        total_reactive_energy_pulse_remaining_time = ENERGY_PULSE_DURATION;
    }

    if (total_reactive_energy_pulse_remaining_time  &&  --total_reactive_energy_pulse_remaining_time == 0)
    {
        total_reactive_energy_pulse_end();
    }

    custom_adc_interrupt();

    /* There may be some current samples available, which we need to pick up */
    if ((TST_SD16IFG_NEUTRAL))
    {
        adc_i_buffer[1] = SD16MEM_NEUTRAL;
        CLR_SD16IFG_NEUTRAL;
    }

    if ((TST_SD16IFG_LIVE))
    {
        adc_i_buffer[0] = SD16MEM_LIVE;
        CLR_SD16IFG_LIVE;
    }
    adc_interrupts++;
}
