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
//  File: emeter-setup.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: emeter-setup.c,v 1.44 2009/04/27 09:46:32 a0754793 Exp $
//
/*! \file emeter-structs.h */
//
//--------------------------------------------------------------------------
//
//  MSP430 setup routines for e-meters.
//
//  This software is appropriate for single phase and three phase e-meters
//  using a voltage sensor plus CT or shunt resistor current sensors, or
//  a combination of a CT plus a shunt.
//
#include <stdint.h>
#include <string.h>

#include <msp430.h>
//#include <emeter-toolkit.h>

#include "SM502/EventTypes.h"
//#include "SM502/graphics.h"
#include "SM502/Tamper.h"
#include "SM502/Logger.h"
#include "emeter-rtc.h"

extern rtc_t rtcc;



#if defined(__MSP430_HAS_SD16_2__)  ||  defined(__MSP430_HAS_SD16_3__)

#define SD16CONF0_FUDGE     0x70
#define SD16CONF1_FUDGE     0x48    //0x38



/*
 * Analog front-end initialization routine.
 *
 * Configures the sigma-delta ADC module as analog front-end for
 * a tamper-resistant meter using a current transformer and a
 * shunt as current sensors (see configuration of channel 0 and 1).
 */
static __inline__ void init_analog_front_end_normal(void)
{
    /*
     * First it makes sure that the Embedded Signal Processing is
     * disabled, otherwise it will not be possible to modify the
     * SD16 registers.
     */
    #if defined(__MSP430_HAS_ESP430E__)
    ESPCTL &= ~ESPEN;
    #endif

    /*
     * The general configuration of the analog front-end,
     * that applies to all channels: clock selection (SMCLK) and divider
     * settings (depending on SMCLK frequency) and reference voltage
     * selections.
     */

    SD16CCTL_VOLTAGE &= ~SD16SC;
    SD16CCTL_LIVE &= ~SD16SC;
    #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    SD16CCTL_NEUTRAL &= ~SD16SC;
    #endif

    SD16CTL = SD16SSEL_1  /* Clock is SMCLK */
            | SD16DIV_3   /* Divide by 8 => ADC clock: 1.048576MHz */
            | SD16REFON;  /* Use internal reference */

    SD16INCTL_LIVE = SD16INCH_CURRENT | CURRENT_LIVE_GAIN;          /* Set gain for channel 0 (I1) */
    SD16CCTL_LIVE = SD16OSR_256 | SD16DF | SD16GRP | SD16IE;        /* Set oversampling ratio to 256 (default) */
    SD16PRE_LIVE = 0;

        #if defined(NEUTRAL_MONITOR_SUPPORT)
    SD16INCTL_NEUTRAL = SD16INCH_CURRENT | CURRENT_NEUTRAL_GAIN;    /* Set gain for channel 1 (I2) */
    SD16CCTL_NEUTRAL = SD16OSR_256 | SD16DF | SD16GRP | SD16IE;     /* Set oversampling ratio to 256 (default) */
    SD16PRE_NEUTRAL = 0;
        #endif

    /* Configure analog front-end channel 2 - Voltage */
    SD16INCTL_VOLTAGE = SD16INCH_0 | VOLTAGE_GAIN;                  /* Set gain for channel 2 (V) */
    SD16CCTL_VOLTAGE = SD16OSR_256 | SD16DF | SD16SC | SD16IE;      /* Set oversampling ratio to 256 (default) */
    SD16PRE_VOLTAGE = DEFAULT_V_PRESCALE_FACTOR;
    #if defined(SD16CONF1_FUDGE)
    SD16CONF1 = SD16CONF1_FUDGE;
    #endif

    #if !defined(ESP_SUPPORT)  &&  defined(SINGLE_PHASE)
    chan1.metrology.current.in_phase_correction[0].sd16_preloaded_offset = 0;
        #if defined(NEUTRAL_MONITOR_SUPPORT)
    chan1.metrology.neutral.in_phase_correction[0].sd16_preloaded_offset = 0;
        #endif
    #endif
    /*
     * \note
     * Please note, the oversampling ratio should be the same
     * for all channels. Default is 256.
     */
}

    #if defined(LIMP_MODE_SUPPORT)
static __inline__ void init_analog_front_end_limp(void)
{
        #if defined(__MSP430_HAS_ESP430E__)
    ESPCTL &= ~ESPEN;
        #endif
    SD16CCTL_VOLTAGE &= ~SD16SC;
    SD16CCTL_LIVE &= ~SD16SC;
        #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    SD16CCTL_NEUTRAL &= ~SD16SC;
        #endif
    SD16CTL = SD16SSEL_1  /* Clock is SMCLK */
            | SD16DIV_3   /* Divide by 8 => ADC clock: 1.048576MHz */
            | SD16REFON   /* Use internal reference */
            | SD16LP;
    SD16INCTL_LIVE = SD16INCH_CURRENT | CURRENT_LIVE_GAIN;                  /* Set gain for channel 0 (I1) */
    SD16CCTL_LIVE = SD16OSR_32 | SD16DF | SD16SNGL | SD16GRP | SD16IE;      /* Set oversampling ratio to 32 */
    SD16PRE_LIVE = 0;
    #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    SD16INCTL_NEUTRAL = SD16INCH_CURRENT | CURRENT_NEUTRAL_GAIN;            /* Set gain for channel 1 (I2) */
    SD16CCTL_NEUTRAL = SD16OSR_32 | SD16DF | SD16SNGL | SD16GRP | SD16IE;   /* Set oversampling ratio to 32 */
    SD16PRE_NEUTRAL = 0;
    #endif
    SD16INCTL_VOLTAGE = SD16INCH_0 | VOLTAGE_GAIN;                          /* Set gain for channel 2 (V) */
    SD16CCTL_VOLTAGE = SD16OSR_32 | SD16DF | SD16SNGL | SD16IE;             /* Set oversampling ratio to 32 */
    SD16PRE_VOLTAGE = DEFAULT_V_PRESCALE_FACTOR;
    #if defined(SD16CONF0_FUDGE)
    SD16CONF0 = SD16CONF0_FUDGE;
    #endif
    #if defined(SD16CONF1_FUDGE)
    SD16CONF1 = SD16CONF1_FUDGE;
    #endif
    chan1.metrology.current.in_phase_correction[0].sd16_preloaded_offset = 0;
        #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    chan1.metrology.neutral.in_phase_correction[0].sd16_preloaded_offset = 0;
        #endif
}
    #endif

 void disable_analog_front_end(void)//static __inline__
{
    #if defined(__MSP430_HAS_ESP430E__)
    ESPCTL &= ~ESPEN;
    #endif

    SD16INCTL_VOLTAGE = 0;
    SD16CCTL_VOLTAGE = 0;
    SD16PRE_VOLTAGE = DEFAULT_V_PRESCALE_FACTOR;

    SD16INCTL_LIVE = 0;
    SD16CCTL_LIVE = 0;
    SD16PRE_LIVE = 0;

    #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    SD16INCTL_NEUTRAL = 0;
    SD16CCTL_NEUTRAL = 0;
    SD16PRE_NEUTRAL = 0;
    #endif

    SD16CTL = 0;
    #if defined(SD16CONF0_FUDGE)
    SD16CONF0 = SD16CONF0_FUDGE;
    #endif
    #if defined(SD16CONF1_FUDGE)
    SD16CONF1 = SD16CONF1_FUDGE;
    #endif

    #if !defined(ESP_SUPPORT)  &&  defined(SINGLE_PHASE)
    chan1.metrology.current.in_phase_correction[0].sd16_preloaded_offset = 0;
        #if defined(NEUTRAL_MONITOR_SUPPORT)
    chan1.metrology.neutral.in_phase_correction[0].sd16_preloaded_offset = 0;
        #endif
    #endif
}
#endif

#if defined(__MSP430_HAS_SD24_A2__)  ||  defined(__MSP430_HAS_SD24_A3__)

#define SD24CONF0_FUDGE     0x70
#define SD24CONF1_FUDGE     0x48    //0x38

/*
 * Analog front-end initialization routine.
 *
 * Configures the sigma-delta ADC module as analog front-end for
 * a tamper-resistant meter using a current transformer and a
 * shunt as current sensors (see configuration of channel 0 and 1).
 */
static __inline__ void init_analog_front_end_normal(void)
{
    /*
     * The general configuration of the analog front-end,
     * that applies to all channels: clock selection (SMCLK) and divider
     * settings (depending on SMCLK frequency) and reference voltage
     * selections.
     */

    SD16CCTL_VOLTAGE &= ~SD24SC;
    SD16CCTL_LIVE &= ~SD24SC;
    #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    SD16CCTL_NEUTRAL &= ~SD24SC;
    #endif

#if MCLK_DEF == 16
    SD24CTL = SD24SSEL_1  /* Clock is SMCLK */
            | SD24DIV_4   /* Divide by 16 => ADC clock: 1.048576MHz */
            | SD24REFON;  /* Use internal reference */
#endif
#if MCLK_DEF == 8
    SD24CTL = SD24SSEL_1  /* Clock is SMCLK */
            | SD24DIV_3   /* Divide by 8 => ADC clock: 1.048576MHz */
            | SD24REFON;  /* Use internal reference */
#endif

    SD16INCTL_LIVE = SD16INCH_CURRENT | CURRENT_LIVE_GAIN;          /* Set gain for channel 0 (I1) */
    SD16CCTL_LIVE = SD24OSR_256 | SD24DF | SD24GRP | SD24IE;        /* Set oversampling ratio to 256 (default) */
    SD16PRE_LIVE = 0;

        #if defined(NEUTRAL_MONITOR_SUPPORT)
    SD16INCTL_NEUTRAL = SD16INCH_CURRENT | CURRENT_NEUTRAL_GAIN;    /* Set gain for channel 1 (I2) */
    SD16CCTL_NEUTRAL = SD24OSR_256 | SD24DF | SD24GRP | SD24IE;     /* Set oversampling ratio to 256 (default) */
    SD16PRE_NEUTRAL = 0;
        #endif

    /* Configure analog front-end channel 2 - Voltage */
    SD16INCTL_VOLTAGE = SD24INCH_0 | VOLTAGE_GAIN;                  /* Set gain for channel 2 (V) */
    SD16CCTL_VOLTAGE = SD24OSR_256 | SD24DF | SD24SC | SD24IE;      /* Set oversampling ratio to 256 (default) */
    SD16PRE_VOLTAGE = DEFAULT_V_PRESCALE_FACTOR;
    #if defined(SD24CONF1_FUDGE)
    SD24CONF1 = SD24CONF1_FUDGE;
    #endif

    #if !defined(ESP_SUPPORT)  &&  defined(SINGLE_PHASE)
    chan1.metrology.current.in_phase_correction[0].sd16_preloaded_offset = 0;
        #if defined(NEUTRAL_MONITOR_SUPPORT)
    chan1.metrology.neutral.in_phase_correction[0].sd16_preloaded_offset = 0;
        #endif
    #endif
    /*
     * \note
     * Please note, the oversampling ratio should be the same
     * for all channels. Default is 256.
     */
}

    #if defined(LIMP_MODE_SUPPORT)
static __inline__ void init_analog_front_end_limp(void)
{
        #if defined(__MSP430_HAS_ESP430E__)
    ESPCTL &= ~ESPEN;
        #endif
    SD24CCTL_VOLTAGE &= ~SD24SC;
    SD24CCTL_LIVE &= ~SD24SC;
        #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    SD24CCTL_NEUTRAL &= ~SD24SC;
        #endif
#if MCLK_DEF == 16
    SD24CTL = SD24SSEL_1  /* Clock is SMCLK */
            | SD24DIV_3   /* Divide by 16 => ADC clock: 1.048576MHz */
            | SD24REFON   /* Use internal reference */
            | SD24LP;
#endif
#if MCLK_DEF == 8
    SD24CTL = SD24SSEL_1  /* Clock is SMCLK */
            | SD24DIV_3   /* Divide by 8 => ADC clock: 1.048576MHz */
            | SD24REFON   /* Use internal reference */
            | SD24LP;
#endif
    SD24INCTL_LIVE = SD24INCH_CURRENT | CURRENT_LIVE_GAIN;                  /* Set gain for channel 0 (I1) */
    SD24CCTL_LIVE = SD24OSR_32 | SD24DF | SD24SNGL | SD24GRP | SD24IE;      /* Set oversampling ratio to 32 */
    SD24PRE_LIVE = 0;
    #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    SD24INCTL_NEUTRAL = SD24INCH_CURRENT | CURRENT_NEUTRAL_GAIN;            /* Set gain for channel 1 (I2) */
    SD24CCTL_NEUTRAL = SD24OSR_32 | SD24DF | SD24SNGL | SD24GRP | SD24IE;   /* Set oversampling ratio to 32 */
    SD24PRE_NEUTRAL = 0;
    #endif
    SD24INCTL_VOLTAGE = SD24INCH_0 | VOLTAGE_GAIN;                          /* Set gain for channel 2 (V) */
    SD24CCTL_VOLTAGE = SD24OSR_32 | SD24DF | SD24SNGL | SD24IE;             /* Set oversampling ratio to 32 */
    SD24PRE_VOLTAGE = 0;
    #if defined(SD24CONF0_FUDGE)
    SD24CONF0 = SD24CONF0_FUDGE;
    #endif
    #if defined(SD24CONF1_FUDGE)
    SD24CONF1 = SD24CONF1_FUDGE;
    #endif
    chan1.metrology.current.in_phase_correction[0].sd16_preloaded_offset = 0;
        #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    chan1.metrology.neutral.in_phase_correction[0].sd16_preloaded_offset = 0;
        #endif
}
    #endif

void disable_analog_front_end(void)//static __inline__ 
{
    #if defined(__MSP430_HAS_ESP430E__)
    ESPCTL &= ~ESPEN;
    #endif

    SD16INCTL_VOLTAGE = 0;
    SD16CCTL_VOLTAGE = 0;
    SD16PRE_VOLTAGE = DEFAULT_V_PRESCALE_FACTOR;

    SD16INCTL_LIVE = 0;
    SD16CCTL_LIVE = 0;
    SD16PRE_LIVE = 0;

    #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    SD16INCTL_NEUTRAL = 0;
    SD16CCTL_NEUTRAL = 0;
    SD16PRE_NEUTRAL = 0;
    #endif

    SD24CTL = 0;
    #if defined(SD24CONF0_FUDGE)
    SD24CONF0 = SD24CONF0_FUDGE;
    #endif
    #if defined(SD24CONF1_FUDGE)
    SD24CONF1 = SD24CONF1_FUDGE;
    #endif

    #if !defined(ESP_SUPPORT)  &&  defined(SINGLE_PHASE)
    chan1.metrology.current.in_phase_correction[0].sd16_preloaded_offset = 0;
        #if defined(NEUTRAL_MONITOR_SUPPORT)
    chan1.metrology.neutral.in_phase_correction[0].sd16_preloaded_offset = 0;
        #endif
    #endif
}
#endif

#if defined(__MSP430_HAS_SD16_A3__)  ||  defined(__MSP430_HAS_SD16_A4__)
#error TBD
#endif

#if defined (__MSP430_HAS_SD24_B__)
//#define SD16CONF0_FUDGE     0x70
//#define SD16CONF1_FUDGE     0x48    //0x38

/*
 * Analog front-end initialization routine.
 *
 * Configures the sigma-delta ADC module as analog front-end for
 * a tamper-resistant meter using a current transformer and a
 * shunt as current sensors (see configuration of channel 0 and 1).
 */
static __inline__ void init_analog_front_end_normal(void)
{
    /*
     * First it makes sure that the Embedded Signal Processing is
     * disabled, otherwise it will not be possible to modify the
     * SD16 registers.
     */
    #if defined(__MSP430_HAS_ESP430E__)
    ESPCTL &= ~ESPEN;
    #endif

    /*
     * The general configuration of the analog front-end,
     * that applies to all channels: clock selection (SMCLK) and divider
     * settings (depending on SMCLK frequency) and reference voltage
     * selections.
     */

    REFCTL0 = REFMSTR + REFVSEL_1 + REFON;         /* Enabale Reference = 2.0V */

    //SD16CCTL_VOLTAGE &= ~SD24SC;
    //SD16CCTL_LIVE &= ~SD24SC;
    SD24BCTL1 &= ~SD24GRP0SC;                                        /* SD16SC */
    #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
//    SD16CCTL_NEUTRAL &= ~SD24SC;
    #endif

#if MCLK_DEF == 16
    SD24BCTL0 = SD24SSEL__SMCLK  /* Clock is SMCLK */
            | SD24PDIV_3    /* Divide by 16 => ADC clock: 1.048576MHz */
            | SD24DIV0
            | SD24REFS;     /* Use internal reference */
#endif
#if MCLK_DEF == 8
    SD24BCTL0 = SD24SSEL__SMCLK  /* Clock is SMCLK */
            | SD24PDIV_3    /* Divide by 8 => ADC clock: 1.048576MHz */
            | SD24REFS;     /* Use internal reference */
#endif

    //MM24 Take away SD24ALGN Part
    SD16INCTL_LIVE =   CURRENT_LIVE_GAIN;          /* Set gain for channel 0 (I1) */
    SD16CCTL_LIVE = SD24DF_1  | SD24SCS_4;                /* Set oversampling ratio to 256 (default) */
    SD16BOSR_LIVE = 256 - 1;                                        /* Set oversampling ratio to 256 (default) */
///    SD16BIE |= SD16IE_LIVE;
    SD16PRE_LIVE = 0;

        #if defined(NEUTRAL_MONITOR_SUPPORT)
    SD16INCTL_NEUTRAL =   CURRENT_NEUTRAL_GAIN;    /* Set gain for channel 1 (I2) */
    SD16CCTL_NEUTRAL = SD24DF_1 | SD24SCS_4;             /* Set oversampling ratio to 256 (default) */
    SD16BOSR_NEUTRAL = 256 -1;                                      /* Set oversampling ratio to 256 (default) */
///    SD16BIE |= SD16IE_NEUTRAL;
    SD16PRE_NEUTRAL = 0;
        #endif

    /* Configure analog front-end channel 2 - Voltage */
    SD16INCTL_VOLTAGE =   VOLTAGE_GAIN;                  /* Set gain for channel 2 (V) */
    SD16CCTL_VOLTAGE = SD24DF_1 | SD24ALGN | SD24SCS_4;             /* Set oversampling ratio to 256 (default) */
    SD16BOSR_VOLTAGE = 256 -1;                                      /* Set oversampling ratio to 256 (default) */
    SD24BCTL1 |= SD24GRP0SC;                                        /* SD16SC */
    SET_SD16IE_VOLTAGE;
#ifdef with_DMA
  extern int16_t dma_adc_buffer[6];

  SD24BCTL1_H = SD24DMA1_H;

  DMACTL0 = DMA0TSEL__SD24IFG;
  DMA0SA = (unsigned short) &SD24BMEML0;
  DMA0DA = (unsigned short) dma_adc_buffer;
  DMA0SZ = 6;
  DMA0CTL = DMASWDW + DMASRCINCR_3 + DMADSTINCR_3 + DMADT_5 + DMAEN + DMAIE; // word, inc dest, repeated block, Enable, Enable IE
#else
    //SET_SD16IE_VOLTAGE;
#endif
    SD16PRE_VOLTAGE = 51;//DEFAULT_V_PRESCALE_FACTOR;
//    #if defined(SD16CONF1_FUDGE)
//    SD16CONF1 = SD16CONF1_FUDGE;
//    #endif

    #if !defined(ESP_SUPPORT)  &&  defined(SINGLE_PHASE)
    chan1.metrology.current.in_phase_correction[0].sd16_preloaded_offset = 0;
        #if defined(NEUTRAL_MONITOR_SUPPORT)
    chan1.metrology.neutral.in_phase_correction[0].sd16_preloaded_offset = 0;
        #endif
    #endif
    /*
     * \note
     * Please note, the oversampling ratio should be the same
     * for all channels. Default is 256.
     */
    
    //SD24BIE=0x1;
    //SD24BCTL1 |= SD24GRP0SC; Changed
}

    #if defined(LIMP_MODE_SUPPORT)
static __inline__ void init_analog_front_end_limp(void)
{
        #if defined(__MSP430_HAS_ESP430E__)
    ESPCTL &= ~ESPEN;
        #endif
    SD16CCTL_VOLTAGE &= ~SD16SC;
    SD16CCTL_LIVE &= ~SD16SC;
        #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    SD16CCTL_NEUTRAL &= ~SD16SC;
        #endif
    SD16CTL = SD16SSEL_1  /* Clock is SMCLK */
            | SD16DIV_3   /* Divide by 8 => ADC clock: 1.048576MHz */
            | SD16REFON   /* Use internal reference */
            | SD16LP;
    SD16INCTL_LIVE = SD16INCH_CURRENT | CURRENT_LIVE_GAIN;                  /* Set gain for channel 0 (I1) */
    SD16CCTL_LIVE = SD16OSR_32 | SD16DF | SD16SNGL | SD16GRP | SD16IE;      /* Set oversampling ratio to 32 */
    SD16PRE_LIVE = 0;
    #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    SD16INCTL_NEUTRAL = SD16INCH_CURRENT | CURRENT_NEUTRAL_GAIN;            /* Set gain for channel 1 (I2) */
    SD16CCTL_NEUTRAL = SD16OSR_32 | SD16DF | SD16SNGL | SD16GRP | SD16IE;   /* Set oversampling ratio to 32 */
    SD16PRE_NEUTRAL = 0;
    #endif
    SD16INCTL_VOLTAGE = SD16INCH_0 | VOLTAGE_GAIN;                          /* Set gain for channel 2 (V) */
    SD16CCTL_VOLTAGE = SD16OSR_32 | SD16DF | SD16SNGL | SD16IE;             /* Set oversampling ratio to 32 */
    SD16PRE_VOLTAGE = DEFAULT_V_PRESCALE_FACTOR;
    #if defined(SD16CONF0_FUDGE)
    SD16CONF0 = SD16CONF0_FUDGE;
    #endif
    #if defined(SD16CONF1_FUDGE)
    SD16CONF1 = SD16CONF1_FUDGE;
    #endif
    chan1.metrology.current.in_phase_correction[0].sd16_preloaded_offset = 0;
        #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    chan1.metrology.neutral.in_phase_correction[0].sd16_preloaded_offset = 0;
        #endif
}
    #endif

void disable_analog_front_end(void)//static __inline__ 
{
    #if defined(__MSP430_HAS_ESP430E__)
    ESPCTL &= ~ESPEN;
    #endif

    SD16INCTL_VOLTAGE = 0;
    SD16CCTL_VOLTAGE = 0;
    SD16PRE_VOLTAGE = DEFAULT_V_PRESCALE_FACTOR;

    SD16INCTL_LIVE = 0;
    SD16CCTL_LIVE = 0;
    SD16PRE_LIVE = 0;

    #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
    SD16INCTL_NEUTRAL = 0;
    SD16CCTL_NEUTRAL = 0;
    SD16PRE_NEUTRAL = 0;
    #endif

    SD24BCTL0 = 0;
    #if defined(SD16CONF0_FUDGE)
    SD16CONF0 = SD16CONF0_FUDGE;
    #endif
    #if defined(SD16CONF1_FUDGE)
    SD16CONF1 = SD16CONF1_FUDGE;
    #endif

    #if !defined(ESP_SUPPORT)  &&  defined(SINGLE_PHASE)
    chan1.metrology.current.in_phase_correction[0].sd16_preloaded_offset = 0;
        #if defined(NEUTRAL_MONITOR_SUPPORT)
    chan1.metrology.neutral.in_phase_correction[0].sd16_preloaded_offset = 0;
        #endif
    #endif
}
#endif

#if defined(__MSP430_HAS_SD16_A6__)  ||  defined(__MSP430_HAS_SD16_A7__)
static __inline__ void init_analog_front_end_normal(void)
{
    int i;

    /*
     * The general configurations of the analog front-end,
     * that applies to all channels: clock selection (SMCLK) and divider
     * settings (depending on SMCLK frequency) and reference voltage
     * selections.
     */

    #if defined(SINGLE_PHASE)
    SD16CCTL_VOLTAGE &= ~SD16SC;
    SD16CCTL_LIVE &= ~SD16SC;
    #else
    SD16CCTL_VOLTAGE_1 &= ~SD16SC;
    SD16CCTL_VOLTAGE_2 &= ~SD16SC;
    SD16CCTL_VOLTAGE_3 &= ~SD16SC;
    SD16CCTL_CURRENT_1 &= ~SD16SC;
    SD16CCTL_CURRENT_2 &= ~SD16SC;
    SD16CCTL_CURRENT_3 &= ~SD16SC;
    #endif
    #if defined(__MSP430_HAS_SD16_A7__)
    SD16CCTL_NEUTRAL &= ~SD16SC;
    #endif

    SD16CTL = SD16SSEL_1  /* Clock is SMCLK */
            | SD16XDIV_2  /* Divide by 16 => ADC clock: 1.048576MHz */
            | SD16REFON;  /* Use internal reference */

    #if defined(SINGLE_PHASE)
    SD16INCTL_LIVE = SD16INCH_CURRENT | CURRENT_PHASE_GAIN;        /* Set gain for channel 1 */
    SD16CCTL_LIVE = SD16OSR_256 | SD16DF | SD16GRP | SD16IE;       /* Set oversampling ratio to 256 (default) */
    SD16PRE_LIVE = 0;

    #else
    SD16INCTL_CURRENT_1 = SD16INCH_CURRENT | CURRENT_PHASE_GAIN;        /* Set gain for channel 1 */
    SD16CCTL_CURRENT_1 = SD16OSR_256 | SD16DF | SD16GRP | SD16IE;       /* Set oversampling ratio to 256 (default) */
    SD16PRE_CURRENT_1 = 0;

    SD16INCTL_CURRENT_2 = SD16INCH_CURRENT | CURRENT_PHASE_GAIN;        /* Set gain for channel 2 */
    SD16CCTL_CURRENT_2 = SD16OSR_256 | SD16DF | SD16GRP | SD16IE;       /* Set oversampling ratio to 256 (default) */
    SD16PRE_CURRENT_2 = 0;

    SD16INCTL_CURRENT_3 = SD16INCH_CURRENT | CURRENT_PHASE_GAIN;        /* Set gain for channel 3 */
    SD16CCTL_CURRENT_3 = SD16OSR_256 | SD16DF | SD16GRP | SD16IE;       /* Set oversampling ratio to 256 (default) */
    SD16PRE_CURRENT_3 = 0;
    #endif

    #if defined(__MSP430_HAS_SD16_A7__)
    SD16INCTL_NEUTRAL = SD16INCH_CURRENT | CURRENT_NEUTRAL_GAIN;        /* Set gain for channel neutral */
    SD16CCTL_NEUTRAL = SD16OSR_256 | SD16DF | SD16GRP; // | SD16IE;         /* Set oversampling ratio to 256 (default) */
    SD16PRE_NEUTRAL = 0;
    #endif

    /* Configure analog front-end channel 2 - Voltage */
    #if defined(SINGLE_PHASE)
    SD16INCTL_VOLTAGE = SD16INCH_0 | VOLTAGE_GAIN;
    SD16CCTL_VOLTAGE = SD16OSR_256 | SD16DF | SD16GRP | SD16IE;
    SD16PRE_VOLTAGE = DEFAULT_V_PRESCALE_FACTOR;

    #else
    SD16INCTL_VOLTAGE_1 = SD16INCH_0 | VOLTAGE_GAIN;
    SD16CCTL_VOLTAGE_1 = SD16OSR_256 | SD16DF | SD16GRP | SD16IE;
    SD16PRE_VOLTAGE_1 = 0;

    SD16INCTL_VOLTAGE_2 = SD16INCH_0 | VOLTAGE_GAIN;
    SD16CCTL_VOLTAGE_2 = SD16OSR_256 | SD16DF | SD16GRP; // | SD16IE;
    SD16PRE_VOLTAGE_2 = 0;

    SD16INCTL_VOLTAGE_3 = SD16INCH_0 | VOLTAGE_GAIN;
    SD16CCTL_VOLTAGE_3 = SD16OSR_256 | SD16DF | SD16GRP; // | SD16IE;
    SD16PRE_VOLTAGE_3 = 0;
    #endif

    #if defined(__MSP430_HAS_SD16_A3__)
    SD16CCTL2 |= SD16SC;
    #elif defined(__MSP430_HAS_SD16_A4__)
    SD16CCTL3 |= SD16SC;
    #elif defined(__MSP430_HAS_SD16_A6__)
    SD16CCTL5 |= SD16SC;
    #elif defined(__MSP430_HAS_SD16_A7__)
    SD16CCTL6 |= SD16SC;
    #endif

    #if defined(SINGLE_PHASE)
        chan1.metrology.current.in_phase_correction[0].sd16_preloaded_offset = 0;
    #else
    for (i = 0;  i < NUM_PHASES;  i++)
        chan[i].metrology.current.in_phase_correction[0].sd16_preloaded_offset = 0;
    #endif
}

    #if defined(LIMP_MODE_SUPPORT)
static __inline__ void init_analog_front_end_limp(void)
{
}
    #endif

void disable_analog_front_end(void)//static __inline__ 
{
    int i;
    #if defined(SINGLE_PHASE)

    SD16INCTL_VOLTAGE = 0;
    SD16CCTL_VOLTAGE = 0;
    SD16PRE_VOLTAGE = DEFAULT_V_PRESCALE_FACTOR;

    SD16INCTL_LIVE = 0;
    SD16CCTL_LIVE = 0;
    SD16PRE_LIVE = 0;

    #else
    SD16INCTL_VOLTAGE_1 = 0;
    SD16CCTL_VOLTAGE_1 = 0;
    SD16PRE_VOLTAGE_1 = 0;

    SD16INCTL_VOLTAGE_2 = 0;
    SD16CCTL_VOLTAGE_2 = 0;
    SD16PRE_VOLTAGE_2 = 0;

    SD16INCTL_VOLTAGE_3 = 0;
    SD16CCTL_VOLTAGE_3 = 0;
    SD16PRE_VOLTAGE_3 = 0;

    SD16INCTL_CURRENT_1 = 0;
    SD16CCTL_CURRENT_1 = 0;
    SD16PRE_CURRENT_1 = 0;

    SD16INCTL_CURRENT_2 = 0;
    SD16CCTL_CURRENT_2 = 0;
    SD16PRE_CURRENT_2 = 0;

    SD16INCTL_CURRENT_3 = 0;
    SD16CCTL_CURRENT_3 = 0;
    SD16PRE_CURRENT_3 = 0;
    #endif
    #if defined(__MSP430_HAS_SD16_A7__)
    SD16INCTL_NEUTRAL = 0;
    SD16CCTL_NEUTRAL = 0;
    SD16PRE_NEUTRAL = 0;
    #endif

    SD16CTL = 0;
    #if defined(SD16CONF0_FUDGE)
    SD16CONF0 = SD16CONF0_FUDGE;
    #endif
    #if defined(SD16CONF1_FUDGE)
    SD16CONF1 = SD16CONF1_FUDGE;
    #endif

    #if defined(SINGLE_PHASE)
        chan1.metrology.current.in_phase_correction[0].sd16_preloaded_offset = 0;
    #else
    for (i = 0;  i < NUM_PHASES;  i++)
        chan[i].metrology.current.in_phase_correction[0].sd16_preloaded_offset = 0;
    #endif
}
#endif

#if defined(__MSP430__)
extern rtc_t rtc;
extern uint8_t OperatingMode;
#include "hal_UCS.h"
#include "hal_pmm.h"
void system_setup(void)
{
    WDTCTL = (WDTCTL & 0xFF) | WDTPW | WDTHOLD;

    SetVCore(2);
    LFXT_Start(XT1DRIVE_3);

    Init_FLL_Settle(MCLK_DEF*8388608/8/1000, MCLK_DEF*32768*32/32768);
    /* There seems no benefit in waiting for the FLL to settle at this point. */
    UCSCTL8 &=~MODOSCREQEN & ~SMCLKREQEN & ~MCLKREQEN & ~ACLKREQEN;

    #if defined (__MSP430_HAS_AUX_SUPPLY__)

          PMMCTL0_H = PMMPW_H;    
         // SVSMHCTL|=SVSMHRRL_5|SVSHRVL_1|SVMHE|SVSHE; //SVM monitoring level,set referance voltage for switching to auxilary supplay , 
                                //Interrupt is generated when the supplay falls below this voltage,
                                //Handel this interrupt to detect power failer 
          
          //PMMRIE =0;//&= ~SVSHPE;
          AUXCTL0 = AUXKEY ;
         // AUXCTL1  |= AUX1MD & ~AUX1OK;// | AUX2MD & ~AUX2OK | AUX0MD | AUX0OK; //Auxilary supplay one is dissabled, set AUX1OK = 0;
         // AUXCTL2  |= AUX0LVL_7|AUX2LVL_7|AUXMR_2;
          AUX3CHCTL = AUXCHKEY | AUXCHC_1 | AUXCHV_1 | AUXCHEN;//comment this for the board Enable Charger for AUX3 to enable RTC
          //configure non maskable Auxilary interrupt here 
         // AUXIE    |= AUXSWGIE|AUX2SWIE|AUX0SWIE|AUX2DRPIE&~AUXMONIE;//|AUXMONIE; //use maskable interrupt for auxilary switching 
          PMMCTL0_H = 0;  

          AUXCTL0 = 0;
    #endif
        
    

    #if defined(__MSP430_HAS_RTC_C__)
#ifdef LOCKBAK
   while(BAKCTL & LOCKBAK){                // unlock Backup Subsystem
      BAKCTL &= ~LOCKBAK;
    }
#endif


    /* We want a real watchdog function, but it doesn't have to be fast. */
    /* Use the longest timer - 1s */
        #if defined(USE_WATCHDOG)
    kick_watchdog();    /* Set the watchdog timer to exactly 1s */
        #else
    WDTCTL = (WDTCTL & 0xFF) | WDTPW | WDTHOLD;
        #endif
    #else
    #if defined(__MSP430_HAS_BT__)  ||  defined(__MSP430_HAS_BT_RTC__)
    /* Basic timer setup */
    /* Set ticker to 32768/(256*256) */
        #if defined(__MSP430_HAS_BT__)
    BTCTL = BT_fLCD_DIV64 | BT_fCLK2_DIV128 | BT_fCLK2_ACLK_DIV256;
        #else
    BTCTL = BT_fCLK2_DIV128 | BT_fCLK2_ACLK_DIV256;
        #endif
    /* Enable the 1 second counter interrupt */
    IE2 |= BTIE;

    /* We want a real watchdog function, but it doesn't have to be fast. */
    /* Use the longest timer - 1s */
        #if defined(USE_WATCHDOG)
    kick_watchdog();    /* Set the watchdog timer to exactly 1s */
        #else
    WDTCTL = (WDTCTL & 0xFF) | WDTPW | WDTHOLD;
        #endif
    #else
    IE1 |= WDTIE;               /* Enable the WDT interrupt */
    #endif
    #endif

//#if defined(P8OUT_INIT)
//    P8OUT = P8OUT_INIT;
//    #endif
//    #if defined(P8DIR_INIT)
//    P8DIR = P8DIR_INIT;
//    #endif
//    #if defined(P8SEL_INIT)
//    P8SEL = P8SEL_INIT;
//    #endif
//    #if defined(P8REN_INIT)
//    P8REN = P8REN_INIT;
//#endif



    #if defined(PJOUT_INIT)
    PJOUT = PJOUT_INIT;
    #endif
    #if defined(PJDIR_INIT)
    PJDIR = PJDIR_INIT;
    #endif
    #if defined(PJSEL_INIT)
    PJSEL = PJSEL_INIT;
    #endif
    #if defined(PJREN_INIT)
    PJREN = PJREN_INIT;
    #endif



//    samples_per_second = SAMPLES_PER_10_SECONDS/10;


    
    
       
    #if defined( __HAS_SD_ADC__)
      disable_analog_front_end();
    #endif
    
    meter_status &= ~(STATUS_REVERSED | STATUS_EARTHED | STATUS_PHASE_VOLTAGE_OK);

    #if defined(PER_PHASE_ACTIVE_ENERGY_SUPPORT)
    phase_active_energy_pulse_end();
    #endif

    #if !defined(ESP_SUPPORT)

      /* Prime the DC estimates for quick settling */
        //phase->metrology.current.I_dc_estimate[0] = phase_nv->current.initial_dc_estimate;
        phase->metrology.current.I_endstops = ENDSTOP_HITS_FOR_OVERLOAD;
        dc_filter_current_init(phase->metrology.current.I_dc_estimate[0], phase_nv->current.initial_dc_estimate);
        dc_filter_voltage_init(phase->metrology.V_dc_estimate, phase_nv->initial_v_dc_estimate);
        #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
        //MM24
        //phase->metrology.neutral.I_dc_estimate[0] = phase_nv->current.initial_dc_estimate;
        //neutral.metrology.current.I_endstops = ENDSTOP_HITS_FOR_OVERLOAD;
        phase->metrology.neutral.I_endstops = ENDSTOP_HITS_FOR_OVERLOAD;
        #endif
        #if GAIN_STAGES > 1
        phase->metrology.current.I_dc_estimate[1] = phase_nv->current.initial_dc_estimate;
            #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
        phase->metrology.neutral.I_dc_estimate[1] = phase_nv->current.initial_dc_estimate;
            #endif
        #endif
        #if defined(LIMP_MODE_SUPPORT)
        //phase->metrology.V_dc_estimate[0] = phase_nv->initial_v_dc_estimate;
        //phase->metrology.V_dc_estimate[1] = phase_nv->initial_v_limp_dc_estimate;
        #else
        //phase->metrology.V_dc_estimate = phase_nv->initial_v_dc_estimate;
        #endif
        
        phase->metrology.V_endstops = ENDSTOP_HITS_FOR_OVERLOAD;
        
        #if defined(MAINS_FREQUENCY_SUPPORT)
        phase->metrology.mains_period = ((SAMPLES_PER_10_SECONDS*6554)/MAINS_NOMINAL_FREQUENCY) << 8;
        #endif
    #endif

    
    
    init_rtc();

    custom_initialisation();

   // _EINT();


    kick_watchdog();
    //switch_to_normal_mode();

}
#else
void system_setup(void)
{
    #if !defined(SINGLE_PHASE)
    struct phase_parms_s *phase;
    int j;
    #endif

    #if !defined(SINGLE_PHASE)
    phase = chan;
    for (j = 0;  j < NUM_PHASES;  j++)
    {
    #endif
        /* Prime the DC estimates for quick settling */
    #if defined(NEUTRAL_MONITOR_SUPPORT)
        phase->metrology.neutral.I_dc_estimate = phase_nv->current.initial_dc_estimate;
    #endif
        phase->metrology.current.I_dc_estimate = phase_nv->current.initial_dc_estimate;
        phase->metrology.V_dc_estimate[0] = phase_nv->initial_v_dc_estimate;
        //phase->metrology.V_dc_estimate[1] = phase_nv->initial_v_limp_dc_estimate;
    #if defined(MAINS_FREQUENCY_SUPPORT)
        phase->mains_period = ((SAMPLES_PER_10_SECONDS*6554)/MAINS_NOMINAL_FREQUENCY) << 8;
    #endif
    #if !defined(SINGLE_PHASE)
        phase++;
    }
    #endif

    samples_per_second = SAMPLES_PER_10_SECONDS/10;
    #if defined(TEMPERATURE_SUPPORT)
    temperature = 0;
    #endif
}
#endif

#if defined(__MSP430__)
void switch_to_normal_mode(void)
{    
        /* Switch to full speed, full power mode */
    meter_status |= STATUS_PHASE_VOLTAGE_OK;
//    set_normal_indicator();

    #if defined(__HAS_SD_ADC__)
    _DINT();
    init_analog_front_end_normal();
    #endif

    samples_per_second = SAMPLES_PER_10_SECONDS/10;
    _EINT();
    
    //restore energy log saved during power faile
////    restorePowerFailEnergyLog(phase); 
//    validation_arg_t validation_arg;
//    validation_arg.validate = 0;
//    read_from_eeprom(phase,(uint8_t *)0,restorePowerFailEnergyLog,&validation_arg); 
//    //custom_power_restore_handler();
    operating_mode = OPERATING_MODE_NORMAL;
    
}

    #if defined(LIMP_MODE_SUPPORT)
void switch_to_limp_mode(void)
{
    /* Switch to minimum consumption, current measurement only mode */

    meter_status &= ~(STATUS_REVERSED | STATUS_PHASE_VOLTAGE_OK);
    clr_normal_indicator();
    clr_reverse_current_indicator();

        #if defined(__MSP430_HAS_ADC12__)
    /* Change the ADC reference to Vcc */
    _DINT();

    /* Must disable conversion while reprogramming the ADC */
    ADC12CTL0 &= ~ENC;

    /* Turn the Vref off and ADC on */
    ADC12CTL0 = MSC | REF2_5V | ADC12ON | SHT0_2 | SHT1_2;

            #if defined(SINGLE_PHASE)
    ADC12MCTL0 = SREF_0 | AGND_INPUT;
    ADC12MCTL1 = SREF_0 | NEUTRAL_CURRENT_INPUT;
    ADC12MCTL2 = SREF_0 | AGND_INPUT;
    ADC12MCTL3 = SREF_0 | LIVE_CURRENT_INPUT;
    ADC12MCTL4 = SREF_0 | AGND_INPUT;
                #if defined(BATTERY_MONITOR_SUPPORT)
    ADC12MCTL5 = SREF_0 | VOLTAGE_INPUT;
    ADC12MCTL6 = EOS | SREF_0 | BATTERY_INPUT;
                #else
    ADC12MCTL5 = EOS | SREF_0 | VOLTAGE_INPUT;
                #endif
    /* Skip the temperature, or the reference switches on! */
            #else
    ADC12MCTL0 = SREF_0 | LIVE_CURRENT_INPUT_1;
    ADC12MCTL1 = SREF_0 | AGND_INPUT;
    ADC12MCTL2 = SREF_0 | VOLTAGE_INPUT_1;
    ADC12MCTL3 = SREF_0 | LIVE_CURRENT_INPUT_2;
    ADC12MCTL4 = SREF_0 | AGND_INPUT;
    ADC12MCTL5 = SREF_0 | VOLTAGE_INPUT_2;
    ADC12MCTL6 = SREF_0 | LIVE_CURRENT_INPUT_3;
    ADC12MCTL7 = SREF_0 | AGND_INPUT;
                #if defined(NEUTRAL_CURRENT_INPUT)
    ADC12MCTL8 = SREF_0 | VOLTAGE_INPUT_3;
    ADC12MCTL9 = EOS | SREF_0 | NEUTRAL_CURRENT_INPUT;
                #else
    ADC12MCTL8 = EOS | SREF_0 | VOLTAGE_INPUT_3;
                #endif
    /* Skip the temperature, or the reference switches on! */
            #endif
    ADC12CTL0 |= ENC;

            #if defined(__MSP430_HAS_TA3__)
    /* Switch to a lower sampling rate. */
    TAR = 0;
    TACCR0 = (SAMPLE_PERIOD*LIMP_SAMPLING_RATIO) - 1;
    TACCR1 = (SAMPLE_PERIOD*LIMP_SAMPLING_RATIO) - 3;
    TACCR2 = (SAMPLE_PERIOD*LIMP_SAMPLING_RATIO) - 4;
    TACCTL0 = CCIE;
    TACCTL1 = OUTMOD_3;
    TACTL = TACLR | MC_1 | TASSEL_1;
    /* Enable the interrupt routine which re-enables the ADC */
    TACCTL2 = CCIE;
            #endif
        #endif

        #if defined(__HAS_SD_ADC__)
            #if defined(__MSP430_HAS_TA3__)
    /* Enable the TIMER_A0 interrupt */
    TACTL = TACLR | MC_1 | TASSEL_1;
    TACCTL0 = CCIE;
            #endif
    _DINT();
    init_analog_front_end_limp();
        #endif

    samples_per_second = LIMP_SAMPLES_PER_10_SECONDS/10;

        #if defined(IEC1107_SUPPORT)
    disable_ir_receiver();
        #endif

    _EINT();

        #if defined(PWM_DITHERING_SUPPORT)
    /* Disable dithering, by disabling Timer B */
    TBCTL = TBCLR | TBSSEL_2;
        #endif
    operating_mode = OPERATING_MODE_LIMP;
}
    #endif

    #if defined(POWER_DOWN_SUPPORT)
void switch_to_powerfail_mode(void)
{
    operating_mode = OPERATING_MODE_POWERFAIL;

    /* Note that a power down occured */
    meter_status |= POWER_DOWN;

    /* Turn off all the LEDs. */
    meter_status &= ~(STATUS_REVERSED | STATUS_EARTHED | STATUS_PHASE_VOLTAGE_OK);
    clr_normal_indicator();
    clr_reverse_current_indicator();
    clr_earthed_indicator();
        #if defined(total_active_energy_pulse_end)
    total_active_energy_pulse_end();
        #endif
        #if defined(total_reactive_energy_pulse_end)
    total_reactive_energy_pulse_end();
        #endif

    /* Make the EEPROM signals inputs, and rely on pullups. */
    disable_eeprom_port();

    /* Shut down the LCD */
    custom_lcd_sleep_handler();

        #if defined(__MSP430_HAS_TA3__)  &&  (defined(__MSP430_HAS_ADC12__)  ||  defined(__MSP430_SD_ADC__))
    /* Disable the TIMER_A0 interrupt */
    TACTL = 0;
    TACCTL0 = 0;
    /* Disable the interrupt routine which re-enables the ADC */
    TACCTL2 = 0;
        #endif

        #if defined(__MSP430_HAS_ADC12__)
    /* Now the interrupts have stopped it should be safe to power
       down the ADC. */
    ADC12CTL0 &= ~ENC;
    ADC12CTL0 &= ~(REFON | REF2_5V | ADC12ON);
        #endif
        #if defined(__HAS_SD_ADC__)
    disable_analog_front_end();
        #endif
        #if defined(PWM_DITHERING_SUPPORT)
    /* Disable dithering, by disabling Timer B */
    TBCTL = TBCLR | TBSSEL_2;
        #endif
        #if defined(IEC1107_SUPPORT)  ||  defined(SERIAL_CALIBRATION_SUPPORT)  ||  defined(SERIAL_CALIBRATION_REF_SUPPORT)
    /* Disable the serial port. */
        #if defined(__MSP430_HAS_UART0__)
    U0ME &= ~(UTXE0 | URXE0);
        #elif defined(__MSP430_HAS_USCI_AB0__)
        #elif defined(__MSP430_HAS_USCI_A0__)
        #elif defined(__MSP430_HAS_EUSCI_A0__)
        #endif
            #if defined(IEC1107_SUPPORT)
    disable_ir_receiver();
            #endif
        #endif

        #if defined(BATTERY_MONITOR_SUPPORT)
    /* Battery sensing control pin */
    P3DIR &= ~(BIT1);
    P3OUT |= (BIT1);
        #endif

        #if defined(__MSP430_HAS_FLLPLUS__)  ||  defined(__MSP430_HAS_FLLPLUS_SMALL__)
    /* Slow the clock to 1MHz as quickly as possible. The FLL will not be active
       in LPM3, so switch it off now, and force the FLL's RC oscillator to
       about 1MHz. The exact frequency is not critical. */
    _BIS_SR(SCG0);                  /* switch off FLL locking */
    SCFI0 = FLLD_1;
    SCFQCTL = SCFI0_LOW | SCFQ_M;
    SCFI0 = 0x0;
    SCFI1 = 0x37;
        #endif
        #if defined(__MSP430_HAS_SVS__)
    /* At 1MHz it is safe to turn off the SVS, and rely on the brownout
       detector. Now the meter can survive on a very weak battery. */
    SVSCTL = 0;
        #endif
    custom_power_fail_handler();

    /* ******************** LOW POWER STATE ************************** */
    /* Go to LPM3 mode and exit only when power comes back on. The timer
       interrupt that ticks every second should be checking for power
       restored while we sit here. When it sees the unregulated supply
       at a healthy voltage, it will wake us up. */
    _BIS_SR(LPM3_bits);

    /* Waking up from power down mode */
        #if defined(__MSP430_HAS_SVS__)
    /* Before we go to high speed we need to make sure the supply voltage is
       adequate. If there is an SVS we can use that. There should be no wait
       at this point, since we should only have been woken up if the supply
       is healthy. However, it seems better to be cautious. */
    SVSCTL |= (SVSON | 0x60);
    /* Wait for adequate voltage to run at full speed */
    while ((SVSCTL & SVSOP))
        /* dummy loop */;
    /* The voltage should now be OK to run the CPU at full speed. Now it should
       be OK to use the SVS as a reset source. */
    SVSCTL |= PORON;
        #endif

        #if defined(__MSP430_HAS_FLLPLUS__)  ||  defined(__MSP430_HAS_FLLPLUS_SMALL__)
    /* Speed up the clock to high speed. */
    SCFI0 = FN_3 | FLLD_4;
    SCFQCTL = SCFQCTL_HIGH;
    /* There seems no good reason to wait until the FLL has settled at this point. */
        #endif

    /* Take control of the EEPROM signals again. */
    enable_eeprom_port();

    /* Enable the serial port */
        #if defined(IEC1107_SUPPORT)  ||  defined(SERIAL_CALIBRATION_SUPPORT)  ||  defined(SERIAL_CALIBRATION_REF_SUPPORT)
        #if defined(__MSP430_HAS_UART0__)
            #if defined(SERIAL_CALIBRATION_REF_SUPPORT)
    U0ME |= (UTXE0 | URXE0);
            #elif defined(SERIAL_CALIBRATION_SUPPORT)
    U0ME |= URXE0;
            #else
    U0ME |= UTXE0;
            #endif
        #elif defined(__MSP430_HAS_USCI_AB0__)
        #elif defined(__MSP430_HAS_USCI_A0__)
        #elif defined(__MSP430_HAS_EUSCI_A0__)
        #endif
        #endif

        #if defined(__MSP430_HAS_TA3__)  &&  (defined(__MSP430_HAS_ADC12__)  || defined(__HAS_SD_ADC__))
    /* Enable the TIMER_A0 interrupt */
    TACTL = TACLR | MC_1 | TASSEL_1;
    TACCTL0 = CCIE;
        #endif

    kick_watchdog();

        #if defined(LOSE_FRACTIONAL_PULSE_AT_POWER_ON)
            #if defined(PER_PHASE_ACTIVE_ENERGY_SUPPORT)
//    phase->power_counter = 0;
            #endif
            #if defined(TOTAL_ACTIVE_ENERGY_SUPPORT)
//    total_power_counter = 0;
                #if TOTAL_ENERGY_PULSES_PER_KW_HOUR < 1000
    extra_total_power_counter = 0;
                #endif
            #endif
            #if defined(TOTAL_REACTIVE_ENERGY_SUPPORT)
    total_reactive_power_counter = 0;
                #if TOTAL_ENERGY_PULSES_PER_KW_HOUR < 1000
    extra_total_reactive_power_counter = 0;
                #endif
            #endif
        #endif

        #if defined(BATTERY_MONITOR_SUPPORT)
    if (battery_countdown)
    {
        battery_countdown = 1000;
        /* Battery sensing control pin */
        P3DIR |= (BIT1);
        P3OUT &= ~(BIT1);
    }
        #endif
    /* Come out of power down in limp mode, as we don't know
       if there is sufficent power available to driver the meter
       at full speed. We will soon switch to normal mode if a
       voltage signal is available. */
    /* Limp mode will fire up the ADC again. */
        #if defined(LIMP_MODE_SUPPORT)
    switch_to_limp_mode();
        #else
    switch_to_normal_mode();
        #endif
    custom_power_restore_handler();
}
    #endif
#else
void switch_to_normal_mode(void)
{
}

    #if defined(LIMP_MODE_SUPPORT)
void switch_to_limp_mode(void)
{
}
    #endif

    #if defined(POWER_DOWN_SUPPORT)
void switch_to_powerfail_mode(void)
{
}
    #endif
#endif

#if defined(CORRECTED_RTC_SUPPORT)  &&  defined(__MSP430_HAS_TA3__)
int32_t assess_rtc_speed(void)
{
    int32_t period;
    uint16_t this_capture;
    uint16_t last_capture;
    uint16_t step;
    int32_t counter;
    int limit;

    /* The fast clock should be an exact multiple of the crystal clock, once the FLL has
        settled. If we capture many cycles of an accurate external 32768Hz clock, using
        timer A (or B), we can measure the speed difference between the MSP430's crystal
        and the external clock in a reasonable time. */
    /* The SM clock should be running at 244*32768Hz at this time. */
    _DINT();
    /* Change timer A to running fast, and sampling the external 32768Hz reference. */
    P2SEL |= BIT0;
    TACCR0 = 0xFFFF;
    TACCTL0 = CAP | CCIS_0 | CM_1;
    TACCTL2 = CAP | CCIS_0 | CM_1 | SCS;
    TACTL = TACLR | MC_2 | TASSEL_2;    /* start TIMER_A up mode, SMCLK as input clock */
    period = 0;
    last_capture = TACCR2;
    limit = -1;
    TACCTL2 &= ~CCIFG;
    for (counter = 0;  counter < 32768*5 + 1;  counter++)
    {
        limit = 1000;
        while (!(TACCTL2 & CCIFG))
        {
            if (--limit <= 0)
                break;
        }
        if (limit <= 0)
            break;
        TACCTL2 &= ~CCIFG;
        this_capture = TACCR2;
        step = this_capture - last_capture;
        last_capture = this_capture;
        /* Skip the first sample, as it will be meaningless */
        if (counter)
        {
    #if 0
            if (step < (244 - 5)  ||  step > (244 + 5))
            {
                limit = -2;
                break;
            }
    #endif
            period += step;
        }
        kick_watchdog();
    }
    if (limit <= 0)
        period = limit;
    #if defined(__MSP430_HAS_ADC12__)
    /* Change timer A back to controlling the ADC sampling interval, and the ADC on/off timing. */
    /* CCR0 determines the sample period */
    TACCR0 = SAMPLE_PERIOD - 1;
    TACCR1 = SAMPLE_PERIOD - 3;
    TACCR2 = SAMPLE_PERIOD - 4;
    TACCTL1 = OUTMOD_3;
    #endif
    TACTL = TACLR | MC_1 | TASSEL_1;
    TACCTL0 = CCIE;
    P2SEL &= ~BIT0;
    _EINT();
    return  period;
}
#endif

#if !defined(ESP_SUPPORT)
int align_hardware_with_calibration_data(void)
{
#if !defined(SINGLE_PHASE)
    int ch;
    static struct phase_parms_s *phase;
    static struct phase_nv_parms_s const *phase_nv;
#endif

    disable_analog_front_end();
    init_analog_front_end_normal();

    #if !defined(SINGLE_PHASE)
    for (ch = 0;  ch < NUM_PHASES;  ch++)
    {
        phase = &chan[ch];
        phase_nv = &nv_parms.seg_a.s.chan[ch];
    #endif
    #if defined(__MSP430_HAS_ADC12__)  ||  defined(__MSP430_HAS_ADC10__)
        set_phase_correction(&phase->metrology.current.in_phase_correction[0], phase_nv->current.phase_correction[0]);
        #if GAIN_STAGES > 1
        set_phase_correction(&phase->metrology.current.in_phase_correction[1], phase_nv->current.phase_correction[1]);
        #endif
        #if defined(SINGLE_PHASE)  &&  defined(NEUTRAL_MONITOR_SUPPORT)
        set_phase_correction(&phase->neutral.in_phase_correction[0], nv_parms.seg_a.s.neutral.phase_correction[0]);
            #if GAIN_STAGES > 1
        set_phase_correction(&phase->neutral.in_phase_correction[1], nv_parms.seg_a.s.neutral.phase_correction[1]);
            #endif
        #endif
    #elif !defined(ESP_SUPPORT)
        #if defined(SINGLE_PHASE)
        set_sd16_phase_correction(&phase->metrology.current.in_phase_correction[0], 0, phase_nv->current.phase_correction[0]);
            #if defined(NEUTRAL_MONITOR_SUPPORT)
        set_sd16_phase_correction(&phase->metrology.neutral.in_phase_correction[0], 1, nv_parms.seg_a.s.neutral.phase_correction[0]);
            #endif
        #else
        set_sd16_phase_correction(&phase->metrology.current.in_phase_correction[0], ch, phase_nv->current.phase_correction[0]);
        #endif
    #endif
    #if !defined(SINGLE_PHASE)
    }
    #endif
    return 0;
}
#endif
