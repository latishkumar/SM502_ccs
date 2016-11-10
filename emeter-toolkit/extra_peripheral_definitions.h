#define __inline__ inline

#if !defined(SFR_32BIT)
#define SFR_32BIT(address)          extern volatile unsigned long int address
#endif

#if !defined(SFR_64BIT)
#define SFR_64BIT(address)          extern volatile unsigned long long int address
#endif

#if defined(__MSP430_HAS_MPY32__)  ||  defined(__MSP430_HAS_MPY32__)
/* Byte, 16 bit word and 32 bit word access to the result register of the 16 bit multiplier */
SFR_8BIT(RES16_8[4]);                       /* 32x32-bit 8 bit result */
SFR_16BIT(RES16_16[2]);                     /* 32x32-bit 16 bit result */
SFR_32BIT(RES16_32);                        /* 32x32-bit 32 bit result */
#endif

#if defined(__MSP430_HAS_MPY32__)
/* Byte, 16 bit word, 32 bit word and 64 bit word access to the registers of the 32 bit multiplier */
SFR_8BIT(MPY8[4]);                          /* 32x32-bit unsigned multiply 8 bit argument 1 */
SFR_16BIT(MPY16[2]);                        /* 32x32-bit unsigned multiply 16 bit argument 1 */
SFR_32BIT(MPY32);                           /* 32x32-bit unsigned multiply 32 bit argument 1 */

SFR_8BIT(MPYS8[4]);                         /* 32x32-bit signed multiply 8 bit argument 1 */
SFR_16BIT(MPYS16[2]);                       /* 32x32-bit signed multiply 16 bit argument 1 */
SFR_32BIT(MPYS32);                          /* 32x32-bit signed multiply 32 bit argument 1 */

SFR_8BIT(MAC8[4]);                          /* 32x32-bit unsigned multiply accumulate 8 bit argument 1 */
SFR_16BIT(MAC16[2]);                        /* 32x32-bit unsigned multiply accumulate 16 bit argument 1 */
SFR_32BIT(MAC32);                           /* 32x32-bit unsigned multiply accumulate 32 bit argument 1 */

SFR_8BIT(MACS8[4]);                         /* 32x32-bit signed multiply accumulate 8 bit argument 1 */
SFR_16BIT(MACS16[2]);                       /* 32x32-bit signed multiply accumulate 16 bit argument 1 */
SFR_32BIT(MACS32);                          /* 32x32-bit signed multiply accumulate 32 bit argument 1 */

SFR_8BIT(OP2_8[4]);                         /* 32x32-bit 8 bit argument 2 */
SFR_16BIT(OP2_16[2]);                       /* 32x32-bit 16 bit argument 2 */
SFR_32BIT(OP2_32X);                         /* 32x32-bit 32 bit argument 2 */

SFR_8BIT(RES8[8]);                          /* 32x32-bit 8 bit result */
SFR_16BIT(RES16[4]);                        /* 32x32-bit 16 bit result */
SFR_32BIT(RES32[2]);                        /* 32x32-bit 32 bit result */
SFR_64BIT(RES64);                           /* 32x32-bit 64 bit result */
#endif

#if defined(__MSP430_HAS_SD24_B__)
SFR_8BIT(SD24BMEM0_8[4]);                   /* SD24B 0 8 bit result */
SFR_16BIT(SD24BMEM0_16[2]);                 /* SD24B 0 16 bit result */
SFR_32BIT(SD24BMEM0_32);                    /* SD24B 0 32 bit result */

SFR_8BIT(SD24BMEM1_8[4]);                   /* SD24B 1 8 bit result */
SFR_16BIT(SD24BMEM1_16[2]);                 /* SD24B 1 16 bit result */
SFR_32BIT(SD24BMEM1_32);                    /* SD24B 1 32 bit result */

SFR_8BIT(SD24BMEM2_8[4]);                   /* SD24B 2 8 bit result */
SFR_16BIT(SD24BMEM2_16[2]);                 /* SD24B 2 16 bit result */
SFR_32BIT(SD24BMEM2_32);                    /* SD24B 2 32 bit result */
#endif
