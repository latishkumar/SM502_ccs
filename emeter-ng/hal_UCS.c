//*******************************************************************************
//  Provides Functions to Initialize the UCS/FLL and clock sources
//    File: hal_ucs.c
//
//    Texas Instruments
//
//    Version 1.2
//    11/24/09
//
//    V1.0  Initial Version
//    V1.1  Added timeout function
//    V1.1  Added parameter for XTDrive
//*******************************************************************************

/* ***********************************************************
* THIS PROGRAM IS PROVIDED "AS IS". TI MAKES NO WARRANTIES OR
* REPRESENTATIONS, EITHER EXPRESS, IMPLIED OR STATUTORY,
* INCLUDING ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
* FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
* COMPLETENESS OF RESPONSES, RESULTS AND LACK OF NEGLIGENCE.
* TI DISCLAIMS ANY WARRANTY OF TITLE, QUIET ENJOYMENT, QUIET
* POSSESSION, AND NON-INFRINGEMENT OF ANY THIRD PARTY
* INTELLECTUAL PROPERTY RIGHTS WITH REGARD TO THE PROGRAM OR
* YOUR USE OF THE PROGRAM.
*
* IN NO EVENT SHALL TI BE LIABLE FOR ANY SPECIAL, INCIDENTAL,
* CONSEQUENTIAL OR INDIRECT DAMAGES, HOWEVER CAUSED, ON ANY
* THEORY OF LIABILITY AND WHETHER OR NOT TI HAS BEEN ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGES, ARISING IN ANY WAY OUT
* OF THIS AGREEMENT, THE PROGRAM, OR YOUR USE OF THE PROGRAM.
* EXCLUDED DAMAGES INCLUDE, BUT ARE NOT LIMITED TO, COST OF
* REMOVAL OR REINSTALLATION, COMPUTER TIME, LABOR COSTS, LOSS
* OF GOODWILL, LOSS OF PROFITS, LOSS OF SAVINGS, OR LOSS OF
* USE OR INTERRUPTION OF BUSINESS. IN NO EVENT WILL TI'S
* AGGREGATE LIABILITY UNDER THIS AGREEMENT OR ARISING OUT OF
* YOUR USE OF THE PROGRAM EXCEED FIVE HUNDRED DOLLARS
* (U.S.$500).
*
* Unless otherwise stated, the Program written and copyrighted
* by Texas Instruments is distributed as "freeware".  You may,
* only under TI's copyright in the Program, use and modify the
* Program without any charge or restriction.  You may
* distribute to third parties, provided that you transfer a
* copy of this license to the third party and the third party
* agrees to these terms by its first use of the Program. You
* must reproduce the copyright notice and any other legend of
* ownership on each copy or partial copy, of the Program.
*
* You acknowledge and agree that the Program contains
* copyrighted material, trade secrets and other TI proprietary
* information and is protected by copyright laws,
* international copyright treaties, and trade secret laws, as
* well as other intellectual property laws.  To protect TI's
* rights in the Program, you agree not to decompile, reverse
* engineer, disassemble or otherwise translate any object code
* versions of the Program to a human-readable form.  You agree
* that in no event will you alter, remove or destroy any
* copyright notice included in the Program.  TI reserves all
* rights not specifically granted under this license. Except
* as specifically provided herein, nothing in this agreement
* shall be construed as conferring by implication, estoppel,
* or otherwise, upon you, any license or other right under any
* TI patents, copyrights or trade secrets.
*
* You may not use the Program in non-TI devices.
* ********************************************************* */


#include "msp430.h"
#include "hal_UCS.h"

//************************************************************************
// Check and define required Defines
//************************************************************************

#ifndef XT1LFOFFG               // Defines if not available in header file
#define XT1LFOFFG 0
#endif
#ifndef XT1HFOFFG               // Defines if not available in header file
#define XT1HFOFFG 0
#endif
#ifndef XT2OFFG                 // Defines if not available in header file
#define XT2OFFG 0
#endif
#ifndef XTS                    // Defines if not available in header file
#define XTS 0
#endif

//====================================================================
/**
 * Startup routine for 32kHz Cristal on LFXT1
 *
*/
void LFXT_Start(unsigned int xtdrive)
{
#ifdef _ZEBU_
#else
  UCSCTL6_L |= XT1DRIVE1_L+XT1DRIVE0_L; // Highest drive setting for XT1 startup

  while (SFRIFG1 & OFIFG) {   // check OFIFG fault flag
    UCSCTL7 &= ~(DCOFFG+XT1LFOFFG+XT1HFOFFG+XT2OFFG); // Clear OSC flaut Flags fault flags
    SFRIFG1 &= ~OFIFG;        // Clear OFIFG fault flag
  }
  UCSCTL6 = (UCSCTL6 & ~(XT1DRIVE_3)) |(xtdrive); // set Drive mode
#endif
}

//====================================================================
/**
 * Startup routine for 32kHz Cristal on LFXT1 with timeout counter
 *
*/
unsigned int LFXT_Start_Timeout(unsigned int xtdrive, unsigned int timeout)
{
#ifdef _ZEBU_
#else
  UCSCTL6_L |= XT1DRIVE1_L+XT1DRIVE0_L; // Highest drive setting for XT1 startup

  while ((SFRIFG1 & OFIFG) && timeout--){   // check OFIFG fault flag
    UCSCTL7 &= ~(DCOFFG+XT1LFOFFG+XT1HFOFFG+XT2OFFG); // Clear OSC flaut Flags fault flags
    SFRIFG1 &= ~OFIFG;        // Clear OFIFG fault flag
  }
  UCSCTL6 = (UCSCTL6 & ~(XT1DRIVE_3)) |(xtdrive); // set Drive mode
#endif
  if (timeout)
    return (UCS_STATUS_OK);
  else
    return (UCS_STATUS_ERROR);
}


//====================================================================
/**
 * Startup routine for  XT1
 *
*/
void XT1_Start(unsigned int xtdrive)
{
#ifdef _ZEBU_
#else
  UCSCTL6 &= ~(XT1OFF & XT1DRIVE_3);  // enalbe XT1
  UCSCTL6 |= (XTS & xtdrive);         // enalbe XT1 and set XT1Drive

  while (SFRIFG1 & OFIFG) {   // check OFIFG fault flag
    UCSCTL7 &= ~(DCOFFG+XT1LFOFFG+XT1HFOFFG+XT2OFFG); // Clear OSC flaut Flags
    SFRIFG1 &= ~OFIFG;        // Clear OFIFG fault flag
  }
#endif
}

//====================================================================
/**
 * Startup routine for XT1 with timeout counter
 *
*/
unsigned int XT1_Start_Timeout(unsigned int xtdrive, unsigned int timeout)
{
  UCSCTL6 &= ~(XT1OFF & XT1DRIVE_3);  // enalbe XT1
  UCSCTL6 |= (XTS & xtdrive);         // enalbe XT1 and set XT1Drive

  while ((SFRIFG1 & OFIFG) && timeout--) {   // check OFIFG fault flag
    UCSCTL7 &= ~(DCOFFG+XT1LFOFFG+XT1HFOFFG+XT2OFFG); // Clear OSC flaut Flags
    SFRIFG1 &= ~OFIFG;        // Clear OFIFG fault flag
  }
  if (timeout)
    return (UCS_STATUS_OK);
  else
    return (UCS_STATUS_ERROR);
}

//====================================================================
/**
 * Use  XT1 in Bypasss mode
 *
*/
void XT1_Bypass(void)
{
  UCSCTL6 = XT1BYPASS;

  while (SFRIFG1 & OFIFG) {   // check OFIFG fault flag
    UCSCTL7 &= ~(DCOFFG+XT1LFOFFG+XT1HFOFFG+XT2OFFG); // Clear OSC flaut Flags
    SFRIFG1 &= ~OFIFG;        // Clear OFIFG fault flag
  }
}

//====================================================================
/**
 * Startup routine for  XT2
 *
*/
void XT2_Start(unsigned int xtdrive)
{
  UCSCTL6 &= ~(XT2OFF & XT1DRIVE_3);  // enalbe XT2
  UCSCTL6 |= (xtdrive);               // Set XT2Drive

  while (SFRIFG1 & OFIFG) {   // check OFIFG fault flag
    UCSCTL7 &= ~(DCOFFG+XT1LFOFFG+XT1HFOFFG+XT2OFFG); // Clear OSC flaut Flags
    SFRIFG1 &= ~OFIFG;        // Clear OFIFG fault flag
  }

}

//====================================================================
/**
 * Startup routine for XT2 with timeout counter
 *
*/
unsigned int XT2_Start_Timeout(unsigned int xtdrive, unsigned int timeout)
{
  UCSCTL6 &= ~(XT2OFF & XT1DRIVE_3); // enalbe XT2
  UCSCTL6 |= (xtdrive);             // Set XT2Drive

  while ((SFRIFG1 & OFIFG) && timeout--){   // check OFIFG fault flag
    UCSCTL7 &= ~(DCOFFG+XT1LFOFFG+XT1HFOFFG+XT2OFFG); // Clear OSC flaut Flags
    SFRIFG1 &= ~OFIFG;        // Clear OFIFG fault flag
  }
  if (timeout)
    return (UCS_STATUS_OK);
  else
    return (UCS_STATUS_ERROR);
}

//====================================================================
/**
 * Use XT2 in Bypasss mode
 *
*/
void XT2_Bypass(void)
{
#ifdef XT2BYPASS              // on devices without XT2 this function will be empty
  UCSCTL6 |= XT2BYPASS;

  while (SFRIFG1 & OFIFG) {   // check OFIFG fault flag
    UCSCTL7 &= ~(DCOFFG+XT1LFOFFG+XT1HFOFFG+XT2OFFG); // Clear OSC flaut Flags
    SFRIFG1 &= ~OFIFG;        // Clear OFIFG fault flag
  }
#endif
}

//====================================================================
/**
  * Initializes FLL of the UCS
  *
  * \param fsystem  required system frequency (MCLK) in kHz
  * \param ratio    ratio between fsystem and FLLREFCLK
  */
void Init_FLL(unsigned int fsystem, const unsigned int ratio)
{
#ifdef _ZEBU_
	UCSCTL1= DCORSEL_5 ;


#else

  unsigned int d, dco_div_bits;
  unsigned int mode = 0;

  // save actual state of FLL loop control
  unsigned short globalInterruptState = __get_SR_register() & SCG0;
  __bic_SR_register(SCG0);      // Disable FLL loop control

  d = ratio;
  dco_div_bits = FLLD__2;        // Have at least a divider of 2
  if (fsystem > 16000){
    d >>= 1 ;
    mode = 1;
  }
  else
    fsystem <<= 1;               // fsystem = fsystem * 2

  while (d > 512)
  {
    dco_div_bits = dco_div_bits + FLLD0;  // set next higher div level
    d >>= 1;
  }

  UCSCTL0 = 0x000;               // Set DCO to lowest Tap

  UCSCTL2 &= ~(0x3FF);           // Reset FN bits
  UCSCTL2 = dco_div_bits | (d - 1);

  if (fsystem <= 630)            //           fsystem < 0.63MHz
	UCSCTL1= DCORSEL_0 ;
  else if (fsystem <  1250)      // 0.63MHz < fsystem < 1.25MHz
	UCSCTL1= DCORSEL_1 ;
  else if (fsystem <  2500)      // 1.25MHz < fsystem <  2.5MHz
	UCSCTL1= DCORSEL_2 ;
  else if (fsystem <  5000)      // 2.5MHz  < fsystem <    5MHz
	UCSCTL1= DCORSEL_3 ;
  else if (fsystem <  10000)     // 5MHz    < fsystem <   10MHz
	UCSCTL1= DCORSEL_4 ;
  else if (fsystem <  20000)     // 10MHz   < fsystem <   20MHz
	UCSCTL1= DCORSEL_5 ;
  else if (fsystem <  40000)     // 20MHz   < fsystem <   40MHz
	UCSCTL1= DCORSEL_6 ;
  else
	UCSCTL1= DCORSEL_7 ;

  do {                               // check OFIFG fault flag
    UCSCTL7 &= ~(DCOFFG+XT1LFOFFG+XT2OFFG);     // XT1HFOFFG+ Clear OSC fault Flags
    SFRIFG1 &= ~OFIFG;                                    // Clear OFIFG fault flag
  }while (SFRIFG1 & OFIFG);

  if (mode == 1)                           				  // fsystem > 16000
    SELECT_MCLK_SMCLK(SELM__DCOCLK + SELS__DCOCLK);       // select DCOCLK
  else
    SELECT_MCLK_SMCLK(SELM__DCOCLKDIV + SELS__DCOCLKDIV); // Select DCODIVCLK

  __bis_SR_register(globalInterruptState);                // restore previous state
#endif

} // End of fll_init()

//====================================================================
/**
  * Initializes FLL of the UCS and wait till settled
  *
  * \param fsystem  required system frequency (MCLK) in kHz
  * \param ratio    ratio between fsystem and FLLREFCLK
  */
void Init_FLL_Settle(unsigned int fsystem, const unsigned int ratio)
{
  volatile unsigned int x = ratio * 10;       // we have 32 steps in the DCO / loop takes at least three cycles
                                              // (int)(32/3) = 10
  Init_FLL(fsystem, ratio);
  //while (x--);
}
