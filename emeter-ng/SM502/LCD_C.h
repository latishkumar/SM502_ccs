

#ifndef LCD_C_H
#define LCD_C_H

#include <msp430.h>
#include <stdint.h>

extern const uint8_t LCD_CHAR[]; 


//#define LCD_REV_2             /*[M.G] Added this definition */
#define LCD_REV_3

#ifdef LCD_REV_2   //LCD Revision 2

  #define D8_A(x)  ((x == 0) ? (LCDM1 &= ~BIT0):(LCDM1 |= BIT0) )
  #define D8_B(x)  ((x == 0) ? (LCDM2 &= ~BIT1):(LCDM2 |= BIT1) )
  #define D8_C(x)  ((x == 0) ? (LCDM2 &= ~BIT3):(LCDM2 |= BIT3) )
  #define D8_D(x)  ((x == 0) ? (LCDM1 &= ~BIT3):(LCDM1 |= BIT3) )
  #define D8_E(x)  ((x == 0) ? (LCDM1 &= ~BIT2):(LCDM1 |= BIT2) )
  #define D8_F(x)  ((x == 0) ? (LCDM1 &= ~BIT1):(LCDM1 |= BIT1) )
  #define D8_G(x)  ((x == 0) ? (LCDM2 &= ~BIT2):(LCDM2 |= BIT2) )

  #define D8(x) ( D8_A(((LCD_CHAR[x]>>6)&0x01)),\
                  D8_B(((LCD_CHAR[x]>>5)&0x01)),\
                  D8_C(((LCD_CHAR[x]>>4)&0x01)),\
                  D8_D(((LCD_CHAR[x]>>3)&0x01)),\
                  D8_E(((LCD_CHAR[x]>>2)&0x01)),\
                  D8_F(((LCD_CHAR[x]>>1)&0x01)),\
                  D8_G((LCD_CHAR[x]&0x01))     )

  #define D7_A(x)  ((x == 0) ? (LCDM3 &= ~BIT0):(LCDM3 |= BIT0) )
  #define D7_B(x)  ((x == 0) ? (LCDM4 &= ~BIT1):(LCDM4 |= BIT1) )
  #define D7_C(x)  ((x == 0) ? (LCDM4 &= ~BIT3):(LCDM4 |= BIT3) )
  #define D7_D(x)  ((x == 0) ? (LCDM3 &= ~BIT3):(LCDM3 |= BIT3) )
  #define D7_E(x)  ((x == 0) ? (LCDM3 &= ~BIT2):(LCDM3 |= BIT2) )
  #define D7_F(x)  ((x == 0) ? (LCDM3 &= ~BIT1):(LCDM3 |= BIT1) )
  #define D7_G(x)  ((x == 0) ? (LCDM4 &= ~BIT2):(LCDM4 |= BIT2) )

  #define D7(x) ( D7_A(((LCD_CHAR[x]>>6)&0x01)),\
                  D7_B(((LCD_CHAR[x]>>5)&0x01)),\
                  D7_C(((LCD_CHAR[x]>>4)&0x01)),\
                  D7_D(((LCD_CHAR[x]>>3)&0x01)),\
                  D7_E(((LCD_CHAR[x]>>2)&0x01)),\
                  D7_F(((LCD_CHAR[x]>>1)&0x01)),\
                  D7_G((LCD_CHAR[x]&0x01))     )


  #define D6_A(x)  ((x == 0) ? (LCDM5 &= ~BIT0):(LCDM5 |= BIT0) )
  #define D6_B(x)  ((x == 0) ? (LCDM6 &= ~BIT1):(LCDM6 |= BIT1) )
  #define D6_C(x)  ((x == 0) ? (LCDM6 &= ~BIT3):(LCDM6 |= BIT3) )
  #define D6_D(x)  ((x == 0) ? (LCDM5 &= ~BIT3):(LCDM5 |= BIT3) )
  #define D6_E(x)  ((x == 0) ? (LCDM5 &= ~BIT2):(LCDM5 |= BIT2) )
  #define D6_F(x)  ((x == 0) ? (LCDM5 &= ~BIT1):(LCDM5 |= BIT1) )
  #define D6_G(x)  ((x == 0) ? (LCDM6 &= ~BIT2):(LCDM6 |= BIT2) )


  #define D6(x) ( D6_A(((LCD_CHAR[x]>>6)&0x01)),\
                  D6_B(((LCD_CHAR[x]>>5)&0x01)),\
                  D6_C(((LCD_CHAR[x]>>4)&0x01)),\
                  D6_D(((LCD_CHAR[x]>>3)&0x01)),\
                  D6_E(((LCD_CHAR[x]>>2)&0x01)),\
                  D6_F(((LCD_CHAR[x]>>1)&0x01)),\
                  D6_G((LCD_CHAR[x]&0x01))     )


  #define D5_A(x)  ((x == 0) ? (LCDM7 &= ~BIT0):(LCDM7 |= BIT0) )
  #define D5_B(x)  ((x == 0) ? (LCDM8 &= ~BIT1):(LCDM8 |= BIT1) )
  #define D5_C(x)  ((x == 0) ? (LCDM8 &= ~BIT3):(LCDM8 |= BIT3) )
  #define D5_D(x)  ((x == 0) ? (LCDM7 &= ~BIT3):(LCDM7 |= BIT3) )
  #define D5_E(x)  ((x == 0) ? (LCDM7 &= ~BIT2):(LCDM7 |= BIT2) )
  #define D5_F(x)  ((x == 0) ? (LCDM7 &= ~BIT1):(LCDM7 |= BIT1) )
  #define D5_G(x)  ((x == 0) ? (LCDM8 &= ~BIT2):(LCDM8 |= BIT2) )

  #define D5(x) ( D5_A(((LCD_CHAR[x]>>6)&0x01)),\
                  D5_B(((LCD_CHAR[x]>>5)&0x01)),\
                  D5_C(((LCD_CHAR[x]>>4)&0x01)),\
                  D5_D(((LCD_CHAR[x]>>3)&0x01)),\
                  D5_E(((LCD_CHAR[x]>>2)&0x01)),\
                  D5_F(((LCD_CHAR[x]>>1)&0x01)),\
                  D5_G((LCD_CHAR[x]&0x01))     )



  #define D4_A(x)  ((x == 0) ? (LCDM9 &= ~BIT0):(LCDM9 |= BIT0) )
  #define D4_B(x)  ((x == 0) ? (LCDM10 &= ~BIT1):(LCDM10 |= BIT1) )
  #define D4_C(x)  ((x == 0) ? (LCDM10 &= ~BIT3):(LCDM10 |= BIT3) )
  #define D4_D(x)  ((x == 0) ? (LCDM9 &= ~BIT3):(LCDM9 |= BIT3) )
  #define D4_E(x)  ((x == 0) ? (LCDM9 &= ~BIT2):(LCDM9 |= BIT2) )
  #define D4_F(x)  ((x == 0) ? (LCDM9 &= ~BIT1):(LCDM9 |= BIT1) )
  #define D4_G(x)  ((x == 0) ? (LCDM10 &= ~BIT2):(LCDM10 |= BIT2) )

  #define D4(x) ( D4_A(((LCD_CHAR[x]>>6)&0x01)),\
                  D4_B(((LCD_CHAR[x]>>5)&0x01)),\
                  D4_C(((LCD_CHAR[x]>>4)&0x01)),\
                  D4_D(((LCD_CHAR[x]>>3)&0x01)),\
                  D4_E(((LCD_CHAR[x]>>2)&0x01)),\
                  D4_F(((LCD_CHAR[x]>>1)&0x01)),\
                  D4_G((LCD_CHAR[x]&0x01))     )


  #define D3_A(x)  ((x == 0) ? (LCDM11 &= ~BIT0):(LCDM11 |= BIT0) )
  #define D3_B(x)  ((x == 0) ? (LCDM12 &= ~BIT1):(LCDM12 |= BIT1) )
  #define D3_C(x)  ((x == 0) ? (LCDM12 &= ~BIT3):(LCDM12 |= BIT3) )
  #define D3_D(x)  ((x == 0) ? (LCDM11 &= ~BIT3):(LCDM11 |= BIT3) )
  #define D3_E(x)  ((x == 0) ? (LCDM11 &= ~BIT2):(LCDM11 |= BIT2) )
  #define D3_F(x)  ((x == 0) ? (LCDM11 &= ~BIT1):(LCDM11 |= BIT1) )
  #define D3_G(x)  ((x == 0) ? (LCDM12 &= ~BIT2):(LCDM12 |= BIT2) )

  #define D3(x) ( D3_A(((LCD_CHAR[x]>>6)&0x01)),\
                  D3_B(((LCD_CHAR[x]>>5)&0x01)),\
                  D3_C(((LCD_CHAR[x]>>4)&0x01)),\
                  D3_D(((LCD_CHAR[x]>>3)&0x01)),\
                  D3_E(((LCD_CHAR[x]>>2)&0x01)),\
                  D3_F(((LCD_CHAR[x]>>1)&0x01)),\
                  D3_G((LCD_CHAR[x]&0x01))     )


  #define D2_A(x)  ((x == 0) ? (LCDM13 &= ~BIT0):(LCDM13 |= BIT0) )
  #define D2_B(x)  ((x == 0) ? (LCDM14 &= ~BIT1):(LCDM14 |= BIT1) )
  #define D2_C(x)  ((x == 0) ? (LCDM14 &= ~BIT3):(LCDM14 |= BIT3) )
  #define D2_D(x)  ((x == 0) ? (LCDM13 &= ~BIT3):(LCDM13 |= BIT3) )
  #define D2_E(x)  ((x == 0) ? (LCDM13 &= ~BIT2):(LCDM13 |= BIT2) )
  #define D2_F(x)  ((x == 0) ? (LCDM13 &= ~BIT1):(LCDM13 |= BIT1) )
  #define D2_G(x)  ((x == 0) ? (LCDM14 &= ~BIT2):(LCDM14 |= BIT2) )

  #define D2(x) ( D2_A(((LCD_CHAR[x]>>6)&0x01)),\
                  D2_B(((LCD_CHAR[x]>>5)&0x01)),\
                  D2_C(((LCD_CHAR[x]>>4)&0x01)),\
                  D2_D(((LCD_CHAR[x]>>3)&0x01)),\
                  D2_E(((LCD_CHAR[x]>>2)&0x01)),\
                  D2_F(((LCD_CHAR[x]>>1)&0x01)),\
                  D2_G((LCD_CHAR[x]&0x01))     )



  #define D1_A(x)  ((x == 0) ?( LCDM15 &= ~BIT0):(LCDM15 |= BIT0) )
  #define D1_B(x)  ((x == 0) ? (LCDM16 &= ~BIT1):(LCDM16 |= BIT1) )
  #define D1_C(x)  ((x == 0) ? (LCDM16 &= ~BIT3):(LCDM16 |= BIT3) )
  #define D1_D(x)  ((x == 0) ? (LCDM15 &= ~BIT3):(LCDM15 |= BIT3) )
  #define D1_E(x)  ((x == 0) ? (LCDM15 &= ~BIT2):(LCDM15 |= BIT2) )
  #define D1_F(x)  ((x == 0) ? (LCDM15 &= ~BIT1):(LCDM15 |= BIT1) )
  #define D1_G(x)  ((x == 0) ? (LCDM16 &= ~BIT2):(LCDM16 |= BIT2) )

  //void D1(uint8_t x);
   

  #define D1(x) ( D1_A(((LCD_CHAR[x]>>6)&0x01)),\
                  D1_B(((LCD_CHAR[x]>>5)&0x01)),\
                  D1_C(((LCD_CHAR[x]>>4)&0x01)),\
                  D1_D(((LCD_CHAR[x]>>3)&0x01)),\
                  D1_E(((LCD_CHAR[x]>>2)&0x01)),\
                  D1_F(((LCD_CHAR[x]>>1)&0x01)),\
                  D1_G(((LCD_CHAR[x])&0x01))    )




  #define YH_A(x)  ((x == 0) ? (LCDM40 &= ~BIT5):(LCDM40 |= BIT5) )
  #define YH_B(x)  ((x == 0) ? (LCDM39 &= ~BIT4):(LCDM39 |= BIT4) )
  #define YH_C(x)  ((x == 0) ? (LCDM39 &= ~BIT2):(LCDM39 |= BIT2) )
  #define YH_D(x)  ((x == 0) ? (LCDM40 &= ~BIT2):(LCDM40 |= BIT2) )
  #define YH_E(x)  ((x == 0) ? (LCDM40 &= ~BIT3):(LCDM40 |= BIT3) )
  #define YH_F(x)  ((x == 0) ? (LCDM40 &= ~BIT4):(LCDM40 |= BIT4) )
  #define YH_G(x)  ((x == 0) ? (LCDM39 &= ~BIT3):(LCDM39 |= BIT3) )

  #define YH(x) ( YH_A(((LCD_CHAR[x]>>6)&0x01)),\
                  YH_B(((LCD_CHAR[x]>>5)&0x01)),\
                  YH_C(((LCD_CHAR[x]>>4)&0x01)),\
                  YH_D(((LCD_CHAR[x]>>3)&0x01)),\
                  YH_E(((LCD_CHAR[x]>>2)&0x01)),\
                  YH_F(((LCD_CHAR[x]>>1)&0x01)),\
                  YH_G((LCD_CHAR[x]&0x01))    )


  #define YL_A(x)  ((x == 0) ? (LCDM38 &= ~BIT5):(LCDM38 |= BIT5) )
  #define YL_B(x)  ((x == 0) ? (LCDM37 &= ~BIT4):(LCDM37 |= BIT4) )
  #define YL_C(x)  ((x == 0) ? (LCDM37 &= ~BIT2):(LCDM37 |= BIT2) )
  #define YL_D(x)  ((x == 0) ? (LCDM38 &= ~BIT2):(LCDM38 |= BIT2) )
  #define YL_E(x)  ((x == 0) ? (LCDM38 &= ~BIT3):(LCDM38 |= BIT3) )
  #define YL_F(x)  ((x == 0) ? (LCDM38 &= ~BIT4):(LCDM38 |= BIT4) )
  #define YL_G(x)  ((x == 0) ? (LCDM37 &= ~BIT3):(LCDM37 |= BIT3) )

  #define YL(x) ( YL_A(((LCD_CHAR[x]>>6)&0x01)),\
                  YL_B(((LCD_CHAR[x]>>5)&0x01)),\
                  YL_C(((LCD_CHAR[x]>>4)&0x01)),\
                  YL_D(((LCD_CHAR[x]>>3)&0x01)),\
                  YL_E(((LCD_CHAR[x]>>2)&0x01)),\
                  YL_F(((LCD_CHAR[x]>>1)&0x01)),\
                  YL_G((LCD_CHAR[x]&0x01))    )


  #define YM_Bar(x) ((x == 1) ? (LCDM37 |= BIT1):(LCDM37 &= ~BIT1))


  #define MH_A(x)  ((x == 0) ? (LCDM36 &= ~BIT5):(LCDM36 |= BIT5) )
  #define MH_B(x)  ((x == 0) ? (LCDM35 &= ~BIT4):(LCDM35 |= BIT4) )
  #define MH_C(x)  ((x == 0) ? (LCDM35 &= ~BIT2):(LCDM35 |= BIT2) )
  #define MH_D(x)  ((x == 0) ? (LCDM36 &= ~BIT2):(LCDM36 |= BIT2) )
  #define MH_E(x)  ((x == 0) ? (LCDM36 &= ~BIT3):(LCDM36 |= BIT3) )
  #define MH_F(x)  ((x == 0) ? (LCDM36 &= ~BIT4):(LCDM36 |= BIT4) )
  #define MH_G(x)  ((x == 0) ? (LCDM35 &= ~BIT3):(LCDM35 |= BIT3) )

  #define MH(x) ( MH_A(((LCD_CHAR[x]>>6)&0x01)),\
                  MH_B(((LCD_CHAR[x]>>5)&0x01)),\
                  MH_C(((LCD_CHAR[x]>>4)&0x01)),\
                  MH_D(((LCD_CHAR[x]>>3)&0x01)),\
                  MH_E(((LCD_CHAR[x]>>2)&0x01)),\
                  MH_F(((LCD_CHAR[x]>>1)&0x01)),\
                  MH_G((LCD_CHAR[x]&0x01))    )

  #define ML_A(x)  ((x == 0) ? (LCDM34 &= ~BIT5):(LCDM34 |= BIT5) )
  #define ML_B(x)  ((x == 0) ? (LCDM33 &= ~BIT4):(LCDM33 |= BIT4) )
  #define ML_C(x)  ((x == 0) ? (LCDM33 &= ~BIT2):(LCDM33 |= BIT2) )
  #define ML_D(x)  ((x == 0) ? (LCDM34 &= ~BIT2):(LCDM34 |= BIT2) )
  #define ML_E(x)  ((x == 0) ? (LCDM34 &= ~BIT3):(LCDM34 |= BIT3) )
  #define ML_F(x)  ((x == 0) ? (LCDM34 &= ~BIT4):(LCDM34 |= BIT4) )
  #define ML_G(x)  ((x == 0) ? (LCDM33 &= ~BIT3):(LCDM33 |= BIT3) )

  #define ML(x) ( ML_A(((LCD_CHAR[x]>>6)&0x01)),\
                  ML_B(((LCD_CHAR[x]>>5)&0x01)),\
                  ML_C(((LCD_CHAR[x]>>4)&0x01)),\
                  ML_D(((LCD_CHAR[x]>>3)&0x01)),\
                  ML_E(((LCD_CHAR[x]>>2)&0x01)),\
                  ML_F(((LCD_CHAR[x]>>1)&0x01)),\
                  ML_G((LCD_CHAR[x]&0x01))    )


  #define MD_Bar(x) ( (x == 1) ? (LCDM33 |= BIT1):(LCDM33 &= ~BIT1))


  #define DH_A(x)  ((x == 0) ? (LCDM32 &= ~BIT5):(LCDM32 |= BIT5) )
  #define DH_B(x)  ((x == 0) ? (LCDM31 &= ~BIT4):(LCDM31 |= BIT4) )
  #define DH_C(x)  ((x == 0) ? (LCDM31 &= ~BIT2):(LCDM31 |= BIT2) )
  #define DH_D(x)  ((x == 0) ? (LCDM32 &= ~BIT2):(LCDM32 |= BIT2) )
  #define DH_E(x)  ((x == 0) ? (LCDM32 &= ~BIT3):(LCDM32 |= BIT3) )
  #define DH_F(x)  ((x == 0) ? (LCDM32 &= ~BIT4):(LCDM32 |= BIT4) )
  #define DH_G(x)  ((x == 0) ? (LCDM31 &= ~BIT3):(LCDM31 |= BIT3) )

  #define DH(x) ( DH_A(((LCD_CHAR[x]>>6)&0x01)),\
                  DH_B(((LCD_CHAR[x]>>5)&0x01)),\
                  DH_C(((LCD_CHAR[x]>>4)&0x01)),\
                  DH_D(((LCD_CHAR[x]>>3)&0x01)),\
                  DH_E(((LCD_CHAR[x]>>2)&0x01)),\
                  DH_F(((LCD_CHAR[x]>>1)&0x01)),\
                  DH_G((LCD_CHAR[x]&0x01))    )

  #define DL_A(x)  ((x == 0) ? (LCDM30 &= ~BIT5):(LCDM30 |= BIT5) )
  #define DL_B(x)  ((x == 0) ? (LCDM29 &= ~BIT4):(LCDM29 |= BIT4) )
  #define DL_C(x)  ((x == 0) ? (LCDM29 &= ~BIT2):(LCDM29 |= BIT2) )
  #define DL_D(x)  ((x == 0) ? (LCDM30 &= ~BIT2):(LCDM30 |= BIT2) )
  #define DL_E(x)  ((x == 0) ? (LCDM30 &= ~BIT3):(LCDM30 |= BIT3) )
  #define DL_F(x)  ((x == 0) ? (LCDM30 &= ~BIT4):(LCDM30 |= BIT4) )
  #define DL_G(x)  ((x == 0) ? (LCDM29 &= ~BIT3):(LCDM29 |= BIT3) )

  #define DL(x) ( DL_A(((LCD_CHAR[x]>>6)&0x01)),\
                  DL_B(((LCD_CHAR[x]>>5)&0x01)),\
                  DL_C(((LCD_CHAR[x]>>4)&0x01)),\
                  DL_D(((LCD_CHAR[x]>>3)&0x01)),\
                  DL_E(((LCD_CHAR[x]>>2)&0x01)),\
                  DL_F(((LCD_CHAR[x]>>1)&0x01)),\
                  DL_G((LCD_CHAR[x]&0x01))    )

  #define Tariff(x) ( (x == 1)?(LCDM29 |= BIT1):(LCDM29 &= ~BIT1))


  #define TH_A(x)  ((x == 0) ? (LCDM28 &= ~BIT5):(LCDM28 |= BIT5) )
  #define TH_B(x)  ((x == 0) ? (LCDM27 &= ~BIT4):(LCDM27 |= BIT4) )
  #define TH_C(x)  ((x == 0) ? (LCDM27 &= ~BIT2):(LCDM27 |= BIT2) )
  #define TH_D(x)  ((x == 0) ? (LCDM28 &= ~BIT2):(LCDM28 |= BIT2) )
  #define TH_E(x)  ((x == 0) ? (LCDM28 &= ~BIT3):(LCDM28 |= BIT3) )
  #define TH_F(x)  ((x == 0) ? (LCDM28 &= ~BIT4):(LCDM28 |= BIT4) )
  #define TH_G(x)  ((x == 0) ? (LCDM27 &= ~BIT3):(LCDM27 |= BIT3) )

  #define TH(x) ( TH_A(((LCD_CHAR[x]>>6)&0x01)),\
                  TH_B(((LCD_CHAR[x]>>5)&0x01)),\
                  TH_C(((LCD_CHAR[x]>>4)&0x01)),\
                  TH_D(((LCD_CHAR[x]>>3)&0x01)),\
                  TH_E(((LCD_CHAR[x]>>2)&0x01)),\
                  TH_F(((LCD_CHAR[x]>>1)&0x01)),\
                  TH_G((LCD_CHAR[x]&0x01))    )



  #define TL_A(x)  ((x == 0) ? (LCDM26 &= ~BIT5):(LCDM26 |= BIT5) )
  #define TL_B(x)  ((x == 0) ? (LCDM25 &= ~BIT4):(LCDM25 |= BIT4) )
  #define TL_C(x)  ((x == 0) ? (LCDM25 &= ~BIT2):(LCDM25 |= BIT2) )
  #define TL_D(x)  ((x == 0) ? (LCDM26 &= ~BIT2):(LCDM26 |= BIT2) )
  #define TL_E(x)  ((x == 0) ? (LCDM26 &= ~BIT3):(LCDM26 |= BIT3) )
  #define TL_F(x)  ((x == 0) ? (LCDM26 &= ~BIT4):(LCDM26 |= BIT4) )
  #define TL_G(x)  ((x == 0) ? (LCDM25 &= ~BIT3):(LCDM25 |= BIT3) )

  #define TL(x) ( TL_A(((LCD_CHAR[x]>>6)&0x01)),\
                  TL_B(((LCD_CHAR[x]>>5)&0x01)),\
                  TL_C(((LCD_CHAR[x]>>4)&0x01)),\
                  TL_D(((LCD_CHAR[x]>>3)&0x01)),\
                  TL_E(((LCD_CHAR[x]>>2)&0x01)),\
                  TL_F(((LCD_CHAR[x]>>1)&0x01)),\
                  TL_G((LCD_CHAR[x]&0x01))    )



  #define Batt_L7(x) ((x == 1)? (LCDM24 |= BIT1):(LCDM24 &= ~BIT1))
  #define Batt_L6(x) ((x == 1)? (LCDM24 |= BIT0):(LCDM24 &= ~BIT0))
  #define Batt_L5(x) ((x == 1)? (LCDM27 |= BIT1):(LCDM27 &= ~BIT1))
  #define Batt_L4(x) ((x == 1)? (LCDM26 |= BIT0):(LCDM26 &= ~BIT0))

  #define Battery(x) ( ((x >= 7)? (Batt_L7(1)):(Batt_L7(0))),\
                       ((x >= 6)? (Batt_L6(1)):(Batt_L6(0))),\
                       ((x >= 5)? (Batt_L5(1)):(Batt_L5(0))),\
                       ((x >= 4)? (Batt_L4(1)):(Batt_L4(0))) )
                      

  #define HH_A(x)  ((x == 0) ? (LCDM24 &= ~BIT5):(LCDM24 |= BIT5) )
  #define HH_B(x)  ((x == 0) ? (LCDM23 &= ~BIT4):(LCDM23 |= BIT4) )
  #define HH_C(x)  ((x == 0) ? (LCDM23 &= ~BIT2):(LCDM23 |= BIT2) )
  #define HH_D(x)  ((x == 0) ? (LCDM24 &= ~BIT2):(LCDM24 |= BIT2) )
  #define HH_E(x)  ((x == 0) ? (LCDM24 &= ~BIT3):(LCDM24 |= BIT3) )
  #define HH_F(x)  ((x == 0) ? (LCDM24 &= ~BIT4):(LCDM24 |= BIT4) )
  #define HH_G(x)  ((x == 0) ? (LCDM23 &= ~BIT3):(LCDM23 |= BIT3) )

  #define HH(x) ( HH_A(((LCD_CHAR[x]>>6)&0x01)),\
                  HH_B(((LCD_CHAR[x]>>5)&0x01)),\
                  HH_C(((LCD_CHAR[x]>>4)&0x01)),\
                  HH_D(((LCD_CHAR[x]>>3)&0x01)),\
                  HH_E(((LCD_CHAR[x]>>2)&0x01)),\
                  HH_F(((LCD_CHAR[x]>>1)&0x01)),\
                  HH_G((LCD_CHAR[x]&0x01))    )


  #define HL_A(x)  ((x == 0) ? (LCDM22 &= ~BIT5):(LCDM22 |= BIT5) )
  #define HL_B(x)  ((x == 0) ? (LCDM21 &= ~BIT4):(LCDM21 |= BIT4) )
  #define HL_C(x)  ((x == 0) ? (LCDM21 &= ~BIT2):(LCDM21 |= BIT2) )
  #define HL_D(x)  ((x == 0) ? (LCDM22 &= ~BIT2):(LCDM22 |= BIT2) )
  #define HL_E(x)  ((x == 0) ? (LCDM22 &= ~BIT3):(LCDM22 |= BIT3) )
  #define HL_F(x)  ((x == 0) ? (LCDM22 &= ~BIT4):(LCDM22 |= BIT4) )
  #define HL_G(x)  ((x == 0) ? (LCDM21 &= ~BIT3):(LCDM21 |= BIT3) )

  #define HL(x) ( HL_A(((LCD_CHAR[x]>>6)&0x01)),\
                  HL_B(((LCD_CHAR[x]>>5)&0x01)),\
                  HL_C(((LCD_CHAR[x]>>4)&0x01)),\
                  HL_D(((LCD_CHAR[x]>>3)&0x01)),\
                  HL_E(((LCD_CHAR[x]>>2)&0x01)),\
                  HL_F(((LCD_CHAR[x]>>1)&0x01)),\
                  HL_G((LCD_CHAR[x]&0x01))    )




  #define Sec(x) ( (x==1)?(LCDM21 |= BIT5):(LCDM21 &= ~BIT5))



  #define MinH_A(x)  ((x == 0) ? (LCDM20 &= ~BIT5):(LCDM20 |= BIT5) )
  #define MinH_B(x)  ((x == 0) ? (LCDM19 &= ~BIT4):(LCDM19 |= BIT4) )
  #define MinH_C(x)  ((x == 0) ? (LCDM19 &= ~BIT2):(LCDM19 |= BIT2) )
  #define MinH_D(x)  ((x == 0) ? (LCDM20 &= ~BIT2):(LCDM20 |= BIT2) )
  #define MinH_E(x)  ((x == 0) ? (LCDM20 &= ~BIT3):(LCDM20 |= BIT3) )
  #define MinH_F(x)  ((x == 0) ? (LCDM20 &= ~BIT4):(LCDM20 |= BIT4) )
  #define MinH_G(x)  ((x == 0) ? (LCDM19 &= ~BIT3):(LCDM19 |= BIT3) )

  #define MinH(x) ( MinH_A(((LCD_CHAR[x]>>6)&0x01)),\
                    MinH_B(((LCD_CHAR[x]>>5)&0x01)),\
                    MinH_C(((LCD_CHAR[x]>>4)&0x01)),\
                    MinH_D(((LCD_CHAR[x]>>3)&0x01)),\
                    MinH_E(((LCD_CHAR[x]>>2)&0x01)),\
                    MinH_F(((LCD_CHAR[x]>>1)&0x01)),\
                    MinH_G((LCD_CHAR[x]&0x01))    )


  #define MinL_A(x)  ((x == 0) ? (LCDM18 &= ~BIT5):(LCDM18 |= BIT5) )
  #define MinL_B(x)  ((x == 0) ? (LCDM17 &= ~BIT4):(LCDM17 |= BIT4) )
  #define MinL_C(x)  ((x == 0) ? (LCDM17 &= ~BIT2):(LCDM17 |= BIT2) )
  #define MinL_D(x)  ((x == 0) ? (LCDM18 &= ~BIT2):(LCDM18 |= BIT2) )
  #define MinL_E(x)  ((x == 0) ? (LCDM18 &= ~BIT3):(LCDM18 |= BIT3) )
  #define MinL_F(x)  ((x == 0) ? (LCDM18 &= ~BIT4):(LCDM18 |= BIT4) )
  #define MinL_G(x)  ((x == 0) ? (LCDM17 &= ~BIT3):(LCDM17 |= BIT3) )

  #define MinL(x) (MinL_A(((LCD_CHAR[x]>>6)&0x01)),\
                   MinL_B(((LCD_CHAR[x]>>5)&0x01)),\
                   MinL_C(((LCD_CHAR[x]>>4)&0x01)),\
                   MinL_D(((LCD_CHAR[x]>>3)&0x01)),\
                   MinL_E(((LCD_CHAR[x]>>2)&0x01)),\
                   MinL_F(((LCD_CHAR[x]>>1)&0x01)),\
                   MinL_G((LCD_CHAR[x]&0x01))    )


  #define Relay(x)  ( (x==1)? (LCDM1|=BIT5,LCDM2|=BIT5 ,\
                              LCDM3|=BIT5,LCDM1&=~BIT4):\
                              ((LCDM1|=(BIT5 | BIT4)) ,\
                              LCDM3|=BIT5,\
                              LCDM2&=~BIT5 ))
                                     
                                                           
                      


  #define USB(x)   (((x==1)? (LCDM4 |=BIT5):(LCDM4&=~BIT5)))


  #define RS232(x)   ((x==1)? (LCDM5  |=  BIT5):(LCDM5  &=  ~BIT5))


  #define PB_10(x)  ((x == 1)? (LCDM9  |= BIT5):(LCDM9  &= ~BIT5))
  #define PB_20(x)  ((x == 1)? (LCDM12 |= BIT5):(LCDM12 &= ~BIT5))
  #define PB_30(x)  ((x == 1)? (LCDM13 |= BIT5):(LCDM13 &= ~BIT5))
  #define PB_40(x)  ((x == 1)? (LCDM13 |= BIT4):(LCDM13 &= ~BIT4))
  #define PB_50(x)  ((x == 1)? (LCDM14 |= BIT5):(LCDM14 &= ~BIT5))
  #define PB_60(x)  ((x == 1)? (LCDM15 |= BIT5):(LCDM15 &= ~BIT5))
  #define PB_70(x)  ((x == 1)? (LCDM16 |= BIT5):(LCDM16 &= ~BIT5))
  #define PB_80(x)  ((x == 1)? (LCDM17 |= BIT5):(LCDM17 &= ~BIT5))
  #define PB_90(x)  ((x == 1)? (LCDM19 |= BIT5):(LCDM19 &= ~BIT5))
  #define PB_100(x) ((x == 1)? (LCDM23 |= BIT5):(LCDM23 &= ~BIT5))


    
  #define PB(x) ( ((x >= 0) ? PB_10(1):PB_10(0)),\
                  ((x >= 10) ? PB_20(1):PB_20(0)),\
                  ((x >= 20) ? PB_30(1):PB_30(0)),\
                  ((x >= 30) ? PB_40(1):PB_40(0)),\
                  ((x >= 40) ? PB_50(1):PB_50(0)),\
                  ((x >= 50) ? PB_60(1):PB_60(0)),\
                  ((x >= 60) ? PB_70(1):PB_70(0)),\
                  ((x >= 70) ? PB_80(1):PB_80(0)),\
                  ((x >= 80) ? PB_90(1):PB_90(0)),\
                  ((x >= 90) ? PB_100(1):PB_100(0))\
                )

  #define LTamper(x) ((x==1)?(LCDM6 |=BIT4):(LCDM6 &=~BIT4) )

  #define UTamper(x) ((x==1)?(LCDM6 |=BIT5):(LCDM6 &=~BIT5) )


  #define MTamper(x) ((x==1)?(LCDM8 |=BIT5):(LCDM8 &=~BIT5) )


  #define NTamper(x) ((x==1)?(LCDM9 |=BIT4):(LCDM9 &=~BIT4) )


  #define showImportIndicator (LCDM25 |= BIT1, LCDM25 &=~ BIT0)
  #define showExportIndicator (LCDM25 |= BIT0, LCDM25 &= ~BIT1)


  #define EC (LCDM30 |= BIT1, LCDM31 &=  ~BIT1)
  #define GC (LCDM31 |= BIT1, LCDM30 &= ~BIT1)





  #define EN_Avarage(x)  ((x==1)?(LCDM34 |= BIT1):(LCDM34 &= ~BIT1))

  #define AM_Avarage(x)  ((x==1)?(LCDM3 |= BIT4):(LCDM3 &= ~BIT4))

  #define EN_Active(x)  ((x==1)?(LCDM34 |= BIT0):(LCDM34 &= ~BIT0))

  #define AM_Active(x)  ()//((x==1)?(LCDM4 |= BIT4):(LCDM4 &= ~BIT4))

  #define EN_Low(x)  ((x==1)?(LCDM32 |= BIT1):(LCDM32 &= ~BIT1))

  #define AM_Low(x)  ((x==1)?(LCDM4 |= BIT4):(LCDM4 &= ~BIT4))




  #define EN_Under(x)  (EN_Low(x))

  #define AM_Under(x)  (AM_Low(x))



  #define EN_Reactive(x)  (((x==1)?(LCDM35 |= BIT0):(LCDM35 &= ~BIT0)),EN_Active(x))

  #define AM_Reactive(x)  ()//((x==1)?(LCDM5 |= BIT5):(LCDM5 &= ~BIT5))


  #define EN_Balance(x)  ((x==1)?(LCDM12 |= BIT0 ):(LCDM12 &= ~BIT0))

  #define AM_Balance(x)  ((x==1)?(LCDM7 |= BIT4 ):(LCDM7 &= ~BIT4))

  #define EN_Power(x)  ((x==1)?(LCDM28 |= BIT0  ):(LCDM28 &= ~BIT0))

  #define AM_Power(x)  ((x==1)?(LCDM5 |= BIT4  ):(LCDM5 &= ~BIT4))

  #define EN_Energy(x)  ((x==1)?(LCDM39 |= BIT0):(LCDM39 &= ~BIT0))

  #define AM_Energy(x)  ((x==1)?(LCDM7 |= BIT5):(LCDM7 &= ~BIT5))


  #define EN_Error(x)  ((x==1)?(LCDM26 |= BIT1):(LCDM26 &= ~BIT1))

  #define AM_Error(x)  ((x==1)?(LCDM11 |= BIT5):(LCDM11 &= ~BIT5))


  #define EN_Interruption(x)  ((x==1)?(LCDM35 |= BIT1):(LCDM35 &= ~BIT1))

  #define AM_Interruption(x)  ((x==1)?(LCDM2 |= BIT4):(LCDM2 &= ~BIT4))


  #define EN_CUM(x)  ((x==1)?(LCDM28 |= BIT1):(LCDM28 &= ~BIT1))

  #define AM_CUM(x)  ((x==1)?(LCDM10 |= BIT5):(LCDM10 &= ~BIT5))


  #define EN_DEM(x)  ((x==1)?(LCDM39 |= BIT1):(LCDM39 &= ~BIT1))

  #define AM_DEM(x)  ((x==1)?(LCDM25 |= BIT5):(LCDM25 &= ~BIT5))

  #define Voltage(x) ((x==1)?(LCDM4 |= BIT0):(LCDM4 &= ~BIT0))

  #define Current(x) ((x==1)?(LCDM6 |= BIT0):(LCDM6 &= ~BIT0))

  #define Frequency(x) ((x==1)?(LCDM40 |= BIT1):(LCDM40 &= ~BIT1))

  #define PF(x) ((x==1)?(LCDM2 |= BIT0):(LCDM2 &= ~BIT0))

  #define EN_MAX(x) ((x==1)?(LCDM27 |= BIT0):(LCDM27 &= ~BIT0))

  #define AM_MAX(x) ((x==1)?(LCDM11 |= BIT4):(LCDM11 &= ~BIT4))

  #define EN_Over(x)  (EN_MAX(x))

  #define AM_Over(x)  (AM_MAX(x))


  #define EN_DAY(x) ((x==1)?(LCDM18 |= BIT1):(LCDM18 &= ~BIT1))

  #define AM_DAY(x) ((x==1)?(LCDM17 |= BIT0):(LCDM17 &= ~BIT0))

  #define EN_Birr(x) ((x==1)?(LCDM22 |= BIT0):(LCDM22 &= ~BIT0))

  #define AM_Birr(x) ((x==1)?(LCDM16 |= BIT4):(LCDM16 &= ~BIT4))

  #define EN_MIN(x) ((x==1)?(LCDM15 |= BIT4):(LCDM15 &= ~BIT4))
  #define AM_MIN(x) ((x==1)?(LCDM17 |= BIT1):(LCDM17 &= ~BIT1))

  #define Point1(x) ((x==1)?(LCDM8 |= BIT4):(LCDM8 &= ~BIT4))
  #define Point2(x) ((x==1)?(LCDM10 |= BIT4):(LCDM10 &= ~BIT4))
  #define Point3(x) ((x==1)?(LCDM12 |= BIT4):(LCDM12 &= ~BIT4))
  #define Point4(x) ((x==1)?(LCDM14 |= BIT4):(LCDM14 &= ~BIT4))

  #define UNIT_K(x) ((x==1)?(LCDM14 |= BIT0):(LCDM14 &= ~BIT0))
  #define UINT_V(x) ((x==1)?(LCDM23 |= BIT1):(LCDM23 &= ~BIT1))
  #define UNIT_WForwardSlash(x) ((x==1)?(LCDM22 |= BIT1):(LCDM22 &= ~BIT1))
  #define UNIT_WBackSlash2(x) ((x==1)?(LCDM21 |= BIT1):(LCDM21 &= ~BIT1))
  #define UNIT_ABackSlash(x) ((x==1)?(LCDM20 |= BIT0):(LCDM20 &= ~BIT0))
  #define UNIT_R(x) ((x==1)?(LCDM20 |= BIT1):(LCDM20 &= ~BIT1))
  #define UNIT_H(x) ((x==1)?(LCDM19 |= BIT1):(LCDM19 &= ~BIT1))
  #define UNIT_Z(x) ((x==1)?(LCDM19 |= BIT0):(LCDM19 &= ~BIT0))

  #define UNIT_None   ( UNIT_K(0),UINT_V(0), UNIT_WForwardSlash(0),UNIT_WBackSlash2(0),UNIT_ABackSlash(0),\
                        UNIT_R(0),UNIT_H(0),UNIT_Z(0) )
  #define UNIT_All    ( UNIT_K(1),UINT_V(1), UNIT_WForwardSlash(1),UNIT_WBackSlash2(1),UNIT_ABackSlash(1),\
                        UNIT_R(1),UNIT_H(1),UNIT_Z(1) )

  #define UINT_Amp    ( UNIT_K(0),UINT_V(0), UNIT_WForwardSlash(0),UNIT_WBackSlash2(1),UNIT_ABackSlash(1),\
                      UNIT_R(0),UNIT_H(0),UNIT_Z(0) )
  #define UINT_Volt   ( UNIT_K(0),UINT_V(1), UNIT_WForwardSlash(0),UNIT_WBackSlash2(0),UNIT_ABackSlash(0),\
                      UNIT_R(0),UNIT_H(0),UNIT_Z(0) )
  #define UINT_KV     ( UNIT_K(1),UINT_V(1), UNIT_WForwardSlash(0),UNIT_WBackSlash2(0),UNIT_ABackSlash(0),\
                      UNIT_R(0),UNIT_H(0),UNIT_Z(0) )
  #define UINT_HZ     ( UNIT_K(0),UINT_V(0), UNIT_WForwardSlash(0),UNIT_WBackSlash2(0),UNIT_ABackSlash(0),\
                      UNIT_R(0),UNIT_H(1),UNIT_Z(1) )
  #define UINT_KHZ    ( UNIT_K(1),UINT_V(0), UNIT_WForwardSlash(0),UNIT_WBackSlash2(0),UNIT_ABackSlash(0),\
                      UNIT_R(0),UNIT_H(1),UNIT_Z(1) )
  #define UNIT_W      ( UNIT_K(0),UINT_V(1), UNIT_WForwardSlash(1),UNIT_WBackSlash2(1),UNIT_ABackSlash(0),\
                      UNIT_R(0),UNIT_H(0),UNIT_Z(0) )
  #define UNIT_KW     ( UNIT_K(1),UINT_V(1), UNIT_WForwardSlash(1),UNIT_WBackSlash2(1),UNIT_ABackSlash(0),\
                      UNIT_R(0),UNIT_H(0),UNIT_Z(0) )
  #define UINT_VAr     ( UNIT_K(0),UINT_V(1), UNIT_WForwardSlash(0),UNIT_WBackSlash2(1),UNIT_ABackSlash(1),\
                      UNIT_R(1),UNIT_H(0),UNIT_Z(0) )
  #define UINT_KVA    ( UNIT_K(1),UINT_V(1), UNIT_WForwardSlash(0),UNIT_WBackSlash2(1),UNIT_ABackSlash(1),\
                      UNIT_R(0),UNIT_H(0),UNIT_Z(0) )
  #define UINT_WH    ( UNIT_K(0),UINT_V(1), UNIT_WForwardSlash(1),UNIT_WBackSlash2(1),UNIT_ABackSlash(0),\
                      UNIT_R(0),UNIT_H(1),UNIT_Z(0) )
  #define UINT_KWH    ( UNIT_K(1),UINT_V(1), UNIT_WForwardSlash(1),UNIT_WBackSlash2(1),UNIT_ABackSlash(0),\
                      UNIT_R(0),UNIT_H(1),UNIT_Z(0) )
  #define UNIT_VARH  ( UNIT_K(0),UINT_V(1), UNIT_WForwardSlash(0),UNIT_WBackSlash2(1),UNIT_ABackSlash(1),\
                      UNIT_R(1),UNIT_H(1),UNIT_Z(0) )
  #define UNIT_KVARH  (  UNIT_K(1),UINT_V(1), UNIT_WForwardSlash(0),UNIT_WBackSlash2(1),UNIT_ABackSlash(1),\
                      UNIT_R(1),UNIT_H(1),UNIT_Z(0) )

#elif defined(LCD_REV_1)  //LCD Revision One


  /* 
     x  1 ==> on 
     x  0 ==> off
  */

  #define D8_A(x)  ((x == 0) ? (LCDM1 &= ~BIT0):(LCDM1 |= BIT0) )
  #define D8_B(x)  ((x == 0) ? (LCDM2 &= ~BIT0):(LCDM2 |= BIT0) )
  #define D8_C(x)  ((x == 0) ? (LCDM2 &= ~BIT2):(LCDM2 |= BIT2) )
  #define D8_D(x)  ((x == 0) ? (LCDM1 &= ~BIT3):(LCDM1 |= BIT3) )
  #define D8_E(x)  ((x == 0) ? (LCDM1 &= ~BIT2):(LCDM1 |= BIT2) )
  #define D8_F(x)  ((x == 0) ? (LCDM1 &= ~BIT1):(LCDM1 |= BIT1) )
  #define D8_G(x)  ((x == 0) ? (LCDM2 &= ~BIT1):(LCDM2 |= BIT1) )

  #define D8(x) ( D8_A(((LCD_CHAR[x]>>6)&0x01)),\
                  D8_B(((LCD_CHAR[x]>>5)&0x01)),\
                  D8_C(((LCD_CHAR[x]>>4)&0x01)),\
                  D8_D(((LCD_CHAR[x]>>3)&0x01)),\
                  D8_E(((LCD_CHAR[x]>>2)&0x01)),\
                  D8_F(((LCD_CHAR[x]>>1)&0x01)),\
                  D8_G((LCD_CHAR[x]&0x01))     )

  #define D7_A(x)  ((x == 0) ? (LCDM3 &= ~BIT0):(LCDM3 |= BIT0) )
  #define D7_B(x)  ((x == 0) ? (LCDM4 &= ~BIT0):(LCDM4 |= BIT0) )
  #define D7_C(x)  ((x == 0) ? (LCDM4 &= ~BIT2):(LCDM4 |= BIT2) )
  #define D7_D(x)  ((x == 0) ? (LCDM3 &= ~BIT3):(LCDM3 |= BIT3) )
  #define D7_E(x)  ((x == 0) ? (LCDM3 &= ~BIT2):(LCDM3 |= BIT2) )
  #define D7_F(x)  ((x == 0) ? (LCDM3 &= ~BIT1):(LCDM3 |= BIT1) )
  #define D7_G(x)  ((x == 0) ? (LCDM4 &= ~BIT1):(LCDM4 |= BIT1) )

  #define D7(x) ( D7_A(((LCD_CHAR[x]>>6)&0x01)),\
                  D7_B(((LCD_CHAR[x]>>5)&0x01)),\
                  D7_C(((LCD_CHAR[x]>>4)&0x01)),\
                  D7_D(((LCD_CHAR[x]>>3)&0x01)),\
                  D7_E(((LCD_CHAR[x]>>2)&0x01)),\
                  D7_F(((LCD_CHAR[x]>>1)&0x01)),\
                  D7_G((LCD_CHAR[x]&0x01))     )


  #define D6_A(x)  ((x == 0) ? (LCDM5 &= ~BIT0):(LCDM5 |= BIT0) )
  #define D6_B(x)  ((x == 0) ? (LCDM6 &= ~BIT0):(LCDM6 |= BIT0) )
  #define D6_C(x)  ((x == 0) ? (LCDM6 &= ~BIT2):(LCDM6 |= BIT2) )
  #define D6_D(x)  ((x == 0) ? (LCDM5 &= ~BIT3):(LCDM5 |= BIT3) )
  #define D6_E(x)  ((x == 0) ? (LCDM5 &= ~BIT2):(LCDM5 |= BIT2) )
  #define D6_F(x)  ((x == 0) ? (LCDM5 &= ~BIT1):(LCDM5 |= BIT1) )
  #define D6_G(x)  ((x == 0) ? (LCDM6 &= ~BIT1):(LCDM6 |= BIT1) )


  #define D6(x) ( D6_A(((LCD_CHAR[x]>>6)&0x01)),\
                  D6_B(((LCD_CHAR[x]>>5)&0x01)),\
                  D6_C(((LCD_CHAR[x]>>4)&0x01)),\
                  D6_D(((LCD_CHAR[x]>>3)&0x01)),\
                  D6_E(((LCD_CHAR[x]>>2)&0x01)),\
                  D6_F(((LCD_CHAR[x]>>1)&0x01)),\
                  D6_G((LCD_CHAR[x]&0x01))     )


  #define D5_A(x)  ((x == 0) ? (LCDM7 &= ~BIT0):(LCDM7 |= BIT0) )
  #define D5_B(x)  ((x == 0) ? (LCDM8 &= ~BIT0):(LCDM8 |= BIT0) )
  #define D5_C(x)  ((x == 0) ? (LCDM8 &= ~BIT2):(LCDM8 |= BIT2) )
  #define D5_D(x)  ((x == 0) ? (LCDM7 &= ~BIT3):(LCDM7 |= BIT3) )
  #define D5_E(x)  ((x == 0) ? (LCDM7 &= ~BIT2):(LCDM7 |= BIT2) )
  #define D5_F(x)  ((x == 0) ? (LCDM7 &= ~BIT1):(LCDM7 |= BIT1) )
  #define D5_G(x)  ((x == 0) ? (LCDM8 &= ~BIT1):(LCDM8 |= BIT1) )

  #define D5(x) ( D5_A(((LCD_CHAR[x]>>6)&0x01)),\
                  D5_B(((LCD_CHAR[x]>>5)&0x01)),\
                  D5_C(((LCD_CHAR[x]>>4)&0x01)),\
                  D5_D(((LCD_CHAR[x]>>3)&0x01)),\
                  D5_E(((LCD_CHAR[x]>>2)&0x01)),\
                  D5_F(((LCD_CHAR[x]>>1)&0x01)),\
                  D5_G((LCD_CHAR[x]&0x01))     )



  #define D4_A(x)  ((x == 0) ? (LCDM9 &= ~BIT0):(LCDM9 |= BIT0) )
  #define D4_B(x)  ((x == 0) ? (LCDM10 &= ~BIT0):(LCDM10 |= BIT0) )
  #define D4_C(x)  ((x == 0) ? (LCDM10 &= ~BIT2):(LCDM10 |= BIT2) )
  #define D4_D(x)  ((x == 0) ? (LCDM9 &= ~BIT3):(LCDM9 |= BIT3) )
  #define D4_E(x)  ((x == 0) ? (LCDM9 &= ~BIT2):(LCDM9 |= BIT2) )
  #define D4_F(x)  ((x == 0) ? (LCDM9 &= ~BIT1):(LCDM9 |= BIT1) )
  #define D4_G(x)  ((x == 0) ? (LCDM10 &= ~BIT1):(LCDM10 |= BIT1) )

  #define D4(x) ( D4_A(((LCD_CHAR[x]>>6)&0x01)),\
                  D4_B(((LCD_CHAR[x]>>5)&0x01)),\
                  D4_C(((LCD_CHAR[x]>>4)&0x01)),\
                  D4_D(((LCD_CHAR[x]>>3)&0x01)),\
                  D4_E(((LCD_CHAR[x]>>2)&0x01)),\
                  D4_F(((LCD_CHAR[x]>>1)&0x01)),\
                  D4_G((LCD_CHAR[x]&0x01))     )


  #define D3_A(x)  ((x == 0) ? (LCDM11 &= ~BIT0):(LCDM11 |= BIT0) )
  #define D3_B(x)  ((x == 0) ? (LCDM12 &= ~BIT0):(LCDM12 |= BIT0) )
  #define D3_C(x)  ((x == 0) ? (LCDM12 &= ~BIT2):(LCDM12 |= BIT2) )
  #define D3_D(x)  ((x == 0) ? (LCDM11 &= ~BIT3):(LCDM11 |= BIT3) )
  #define D3_E(x)  ((x == 0) ? (LCDM11 &= ~BIT2):(LCDM11 |= BIT2) )
  #define D3_F(x)  ((x == 0) ? (LCDM11 &= ~BIT1):(LCDM11 |= BIT1) )
  #define D3_G(x)  ((x == 0) ? (LCDM12 &= ~BIT1):(LCDM12 |= BIT1) )

  #define D3(x) ( D3_A(((LCD_CHAR[x]>>6)&0x01)),\
                  D3_B(((LCD_CHAR[x]>>5)&0x01)),\
                  D3_C(((LCD_CHAR[x]>>4)&0x01)),\
                  D3_D(((LCD_CHAR[x]>>3)&0x01)),\
                  D3_E(((LCD_CHAR[x]>>2)&0x01)),\
                  D3_F(((LCD_CHAR[x]>>1)&0x01)),\
                  D3_G((LCD_CHAR[x]&0x01))     )


  #define D2_A(x)  ((x == 0) ? (LCDM13 &= ~BIT0):(LCDM13 |= BIT0) )
  #define D2_B(x)  ((x == 0) ? (LCDM14 &= ~BIT0):(LCDM14 |= BIT0) )
  #define D2_C(x)  ((x == 0) ? (LCDM14 &= ~BIT2):(LCDM14 |= BIT2) )
  #define D2_D(x)  ((x == 0) ? (LCDM13 &= ~BIT3):(LCDM13 |= BIT3) )
  #define D2_E(x)  ((x == 0) ? (LCDM13 &= ~BIT2):(LCDM13 |= BIT2) )
  #define D2_F(x)  ((x == 0) ? (LCDM13 &= ~BIT1):(LCDM13 |= BIT1) )
  #define D2_G(x)  ((x == 0) ? (LCDM14 &= ~BIT1):(LCDM14 |= BIT1) )

  #define D2(x) ( D2_A(((LCD_CHAR[x]>>6)&0x01)),\
                  D2_B(((LCD_CHAR[x]>>5)&0x01)),\
                  D2_C(((LCD_CHAR[x]>>4)&0x01)),\
                  D2_D(((LCD_CHAR[x]>>3)&0x01)),\
                  D2_E(((LCD_CHAR[x]>>2)&0x01)),\
                  D2_F(((LCD_CHAR[x]>>1)&0x01)),\
                  D2_G((LCD_CHAR[x]&0x01))     )



  #define D1_A(x)  ((x == 0) ?( LCDM15 &= ~BIT0):(LCDM15 |= BIT0) )
  #define D1_B(x)  ((x == 0) ? (LCDM16 &= ~BIT0):(LCDM16 |= BIT0) )
  #define D1_C(x)  ((x == 0) ? (LCDM16 &= ~BIT2):(LCDM16 |= BIT2) )
  #define D1_D(x)  ((x == 0) ? (LCDM15 &= ~BIT3):(LCDM15 |= BIT3) )
  #define D1_E(x)  ((x == 0) ? (LCDM15 &= ~BIT2):(LCDM15 |= BIT2) )
  #define D1_F(x)  ((x == 0) ? (LCDM15 &= ~BIT1):(LCDM15 |= BIT1) )
  #define D1_G(x)  ((x == 0) ? (LCDM16 &= ~BIT1):(LCDM16 |= BIT1) )

  //void D1(uint8_t x);
   

  #define D1(x) ( D1_A(((LCD_CHAR[x]>>6)&0x01)),\
                  D1_B(((LCD_CHAR[x]>>5)&0x01)),\
                  D1_C(((LCD_CHAR[x]>>4)&0x01)),\
                  D1_D(((LCD_CHAR[x]>>3)&0x01)),\
                  D1_E(((LCD_CHAR[x]>>2)&0x01)),\
                  D1_F(((LCD_CHAR[x]>>1)&0x01)),\
                  D1_G(((LCD_CHAR[x])&0x01))    )


  #define YH_A(x)  ((x == 0) ? (LCDM40 &= ~BIT5):(LCDM40 |= BIT5) )
  #define YH_B(x)  ((x == 0) ? (LCDM39 &= ~BIT5):(LCDM39 |= BIT5) )
  #define YH_C(x)  ((x == 0) ? (LCDM39 &= ~BIT3):(LCDM39 |= BIT3) )
  #define YH_D(x)  ((x == 0) ? (LCDM40 &= ~BIT2):(LCDM40 |= BIT2) )
  #define YH_E(x)  ((x == 0) ? (LCDM40 &= ~BIT3):(LCDM40 |= BIT3) )
  #define YH_F(x)  ((x == 0) ? (LCDM40 &= ~BIT4):(LCDM40 |= BIT4) )
  #define YH_G(x)  ((x == 0) ? (LCDM39 &= ~BIT4):(LCDM39 |= BIT4) )

  #define YH(x) ( YH_A(((LCD_CHAR[x]>>6)&0x01)),\
                  YH_B(((LCD_CHAR[x]>>5)&0x01)),\
                  YH_C(((LCD_CHAR[x]>>4)&0x01)),\
                  YH_D(((LCD_CHAR[x]>>3)&0x01)),\
                  YH_E(((LCD_CHAR[x]>>2)&0x01)),\
                  YH_F(((LCD_CHAR[x]>>1)&0x01)),\
                  YH_G((LCD_CHAR[x]&0x01))    )


  #define YL_A(x)  ((x == 0) ? (LCDM38 &= ~BIT5):(LCDM38 |= BIT5) )
  #define YL_B(x)  ((x == 0) ? (LCDM37 &= ~BIT5):(LCDM37 |= BIT5) )
  #define YL_C(x)  ((x == 0) ? (LCDM37 &= ~BIT3):(LCDM37 |= BIT3) )
  #define YL_D(x)  ((x == 0) ? (LCDM38 &= ~BIT2):(LCDM38 |= BIT2) )
  #define YL_E(x)  ((x == 0) ? (LCDM38 &= ~BIT3):(LCDM38 |= BIT3) )
  #define YL_F(x)  ((x == 0) ? (LCDM38 &= ~BIT4):(LCDM38 |= BIT4) )
  #define YL_G(x)  ((x == 0) ? (LCDM37 &= ~BIT4):(LCDM37 |= BIT4) )

  #define YL(x) ( YL_A(((LCD_CHAR[x]>>6)&0x01)),\
                  YL_B(((LCD_CHAR[x]>>5)&0x01)),\
                  YL_C(((LCD_CHAR[x]>>4)&0x01)),\
                  YL_D(((LCD_CHAR[x]>>3)&0x01)),\
                  YL_E(((LCD_CHAR[x]>>2)&0x01)),\
                  YL_F(((LCD_CHAR[x]>>1)&0x01)),\
                  YL_G((LCD_CHAR[x]&0x01))    )


  #define YM_Bar(x) ((x == 1) ? (LCDM37 |= BIT2):(LCDM37 &= ~BIT2))


  #define MH_A(x)  ((x == 0) ? (LCDM36 &= ~BIT5):(LCDM36 |= BIT5) )
  #define MH_B(x)  ((x == 0) ? (LCDM35 &= ~BIT5):(LCDM35 |= BIT5) )
  #define MH_C(x)  ((x == 0) ? (LCDM35 &= ~BIT3):(LCDM35 |= BIT3) )
  #define MH_D(x)  ((x == 0) ? (LCDM36 &= ~BIT2):(LCDM36 |= BIT2) )
  #define MH_E(x)  ((x == 0) ? (LCDM36 &= ~BIT3):(LCDM36 |= BIT3) )
  #define MH_F(x)  ((x == 0) ? (LCDM36 &= ~BIT4):(LCDM36 |= BIT4) )
  #define MH_G(x)  ((x == 0) ? (LCDM35 &= ~BIT4):(LCDM35 |= BIT4) )

  #define MH(x) ( MH_A(((LCD_CHAR[x]>>6)&0x01)),\
                  MH_B(((LCD_CHAR[x]>>5)&0x01)),\
                  MH_C(((LCD_CHAR[x]>>4)&0x01)),\
                  MH_D(((LCD_CHAR[x]>>3)&0x01)),\
                  MH_E(((LCD_CHAR[x]>>2)&0x01)),\
                  MH_F(((LCD_CHAR[x]>>1)&0x01)),\
                  MH_G((LCD_CHAR[x]&0x01))    )

  #define ML_A(x)  ((x == 0) ? (LCDM34 &= ~BIT5):(LCDM34 |= BIT5) )
  #define ML_B(x)  ((x == 0) ? (LCDM33 &= ~BIT5):(LCDM33 |= BIT5) )
  #define ML_C(x)  ((x == 0) ? (LCDM33 &= ~BIT3):(LCDM33 |= BIT3) )
  #define ML_D(x)  ((x == 0) ? (LCDM34 &= ~BIT2):(LCDM34 |= BIT2) )
  #define ML_E(x)  ((x == 0) ? (LCDM34 &= ~BIT3):(LCDM34 |= BIT3) )
  #define ML_F(x)  ((x == 0) ? (LCDM34 &= ~BIT4):(LCDM34 |= BIT4) )
  #define ML_G(x)  ((x == 0) ? (LCDM33 &= ~BIT4):(LCDM33 |= BIT4) )

  #define ML(x) ( ML_A(((LCD_CHAR[x]>>6)&0x01)),\
                  ML_B(((LCD_CHAR[x]>>5)&0x01)),\
                  ML_C(((LCD_CHAR[x]>>4)&0x01)),\
                  ML_D(((LCD_CHAR[x]>>3)&0x01)),\
                  ML_E(((LCD_CHAR[x]>>2)&0x01)),\
                  ML_F(((LCD_CHAR[x]>>1)&0x01)),\
                  ML_G((LCD_CHAR[x]&0x01))    )


  #define MD_Bar(x) ( (x == 1) ? (LCDM33 |= BIT2):(LCDM33 &= ~BIT2))


  #define DH_A(x)  ((x == 0) ? (LCDM32 &= ~BIT5):(LCDM32 |= BIT5) )
  #define DH_B(x)  ((x == 0) ? (LCDM31 &= ~BIT5):(LCDM31 |= BIT5) )
  #define DH_C(x)  ((x == 0) ? (LCDM31 &= ~BIT3):(LCDM31 |= BIT3) )
  #define DH_D(x)  ((x == 0) ? (LCDM32 &= ~BIT2):(LCDM32 |= BIT2) )
  #define DH_E(x)  ((x == 0) ? (LCDM32 &= ~BIT3):(LCDM32 |= BIT3) )
  #define DH_F(x)  ((x == 0) ? (LCDM32 &= ~BIT4):(LCDM32 |= BIT4) )
  #define DH_G(x)  ((x == 0) ? (LCDM31 &= ~BIT4):(LCDM31 |= BIT4) )

  #define DH(x) ( DH_A(((LCD_CHAR[x]>>6)&0x01)),\
                  DH_B(((LCD_CHAR[x]>>5)&0x01)),\
                  DH_C(((LCD_CHAR[x]>>4)&0x01)),\
                  DH_D(((LCD_CHAR[x]>>3)&0x01)),\
                  DH_E(((LCD_CHAR[x]>>2)&0x01)),\
                  DH_F(((LCD_CHAR[x]>>1)&0x01)),\
                  DH_G((LCD_CHAR[x]&0x01))    )

  #define DL_A(x)  ((x == 0) ? (LCDM30 &= ~BIT5):(LCDM30 |= BIT5) )
  #define DL_B(x)  ((x == 0) ? (LCDM29 &= ~BIT5):(LCDM29 |= BIT5) )
  #define DL_C(x)  ((x == 0) ? (LCDM29 &= ~BIT3):(LCDM29 |= BIT3) )
  #define DL_D(x)  ((x == 0) ? (LCDM30 &= ~BIT2):(LCDM30 |= BIT2) )
  #define DL_E(x)  ((x == 0) ? (LCDM30 &= ~BIT3):(LCDM30 |= BIT3) )
  #define DL_F(x)  ((x == 0) ? (LCDM30 &= ~BIT4):(LCDM30 |= BIT4) )
  #define DL_G(x)  ((x == 0) ? (LCDM29 &= ~BIT4):(LCDM29 |= BIT4) )

  #define DL(x) ( DL_A(((LCD_CHAR[x]>>6)&0x01)),\
                  DL_B(((LCD_CHAR[x]>>5)&0x01)),\
                  DL_C(((LCD_CHAR[x]>>4)&0x01)),\
                  DL_D(((LCD_CHAR[x]>>3)&0x01)),\
                  DL_E(((LCD_CHAR[x]>>2)&0x01)),\
                  DL_F(((LCD_CHAR[x]>>1)&0x01)),\
                  DL_G((LCD_CHAR[x]&0x01))    )


  #define Tariff(x) ( (x == 1)?(LCDM29 |= BIT0):(LCDM29 &= ~BIT0))


  #define TH_A(x)  ((x == 0) ? (LCDM28 &= ~BIT5):(LCDM28 |= BIT5) )
  #define TH_B(x)  ((x == 0) ? (LCDM27 &= ~BIT5):(LCDM27 |= BIT5) )
  #define TH_C(x)  ((x == 0) ? (LCDM27 &= ~BIT3):(LCDM27 |= BIT3) )
  #define TH_D(x)  ((x == 0) ? (LCDM28 &= ~BIT2):(LCDM28 |= BIT2) )
  #define TH_E(x)  ((x == 0) ? (LCDM28 &= ~BIT3):(LCDM28 |= BIT3) )
  #define TH_F(x)  ((x == 0) ? (LCDM28 &= ~BIT4):(LCDM28 |= BIT4) )
  #define TH_G(x)  ((x == 0) ? (LCDM27 &= ~BIT4):(LCDM27 |= BIT4) )

  #define TH(x) ( TH_A(((LCD_CHAR[x]>>6)&0x01)),\
                  TH_B(((LCD_CHAR[x]>>5)&0x01)),\
                  TH_C(((LCD_CHAR[x]>>4)&0x01)),\
                  TH_D(((LCD_CHAR[x]>>3)&0x01)),\
                  TH_E(((LCD_CHAR[x]>>2)&0x01)),\
                  TH_F(((LCD_CHAR[x]>>1)&0x01)),\
                  TH_G((LCD_CHAR[x]&0x01))    )



  #define TL_A(x)  ((x == 0) ? (LCDM26 &= ~BIT5):(LCDM26 |= BIT5) )
  #define TL_B(x)  ((x == 0) ? (LCDM25 &= ~BIT5):(LCDM25 |= BIT5) )
  #define TL_C(x)  ((x == 0) ? (LCDM25 &= ~BIT3):(LCDM25 |= BIT3) )
  #define TL_D(x)  ((x == 0) ? (LCDM26 &= ~BIT2):(LCDM26 |= BIT2) )
  #define TL_E(x)  ((x == 0) ? (LCDM26 &= ~BIT3):(LCDM26 |= BIT3) )
  #define TL_F(x)  ((x == 0) ? (LCDM26 &= ~BIT4):(LCDM26 |= BIT4) )
  #define TL_G(x)  ((x == 0) ? (LCDM25 &= ~BIT4):(LCDM25 |= BIT4) )

  #define TL(x) ( TL_A(((LCD_CHAR[x]>>6)&0x01)),\
                  TL_B(((LCD_CHAR[x]>>5)&0x01)),\
                  TL_C(((LCD_CHAR[x]>>4)&0x01)),\
                  TL_D(((LCD_CHAR[x]>>3)&0x01)),\
                  TL_E(((LCD_CHAR[x]>>2)&0x01)),\
                  TL_F(((LCD_CHAR[x]>>1)&0x01)),\
                  TL_G((LCD_CHAR[x]&0x01))    )



  #define Batt_L7(x) ((x == 1)? (LCDM24 |= BIT1):(LCDM24 &= ~BIT1))
  #define Batt_L6(x) ((x == 1)? (LCDM25 |= BIT0):(LCDM25 &= ~BIT0))
  #define Batt_L5(x) ((x == 1)? (LCDM25 |= BIT1):(LCDM25 &= ~BIT1))
  #define Batt_L4(x) ((x == 1)? (LCDM25 |= BIT2):(LCDM25 &= ~BIT2))

  #define Battery(x) ( ((x >= 7)? (Batt_L7(1)):(Batt_L7(0))),\
                       ((x >= 6)? (Batt_L6(1)):(Batt_L6(0))),\
                       ((x >= 5)? (Batt_L5(1)):(Batt_L5(0))),\
                       ((x >= 4)? (Batt_L4(1)):(Batt_L4(0))) )
                      

  #define HH_A(x)  ((x == 0) ? (LCDM24 &= ~BIT5):(LCDM24 |= BIT5) )
  #define HH_B(x)  ((x == 0) ? (LCDM23 &= ~BIT5):(LCDM23 |= BIT5) )
  #define HH_C(x)  ((x == 0) ? (LCDM23 &= ~BIT3):(LCDM23 |= BIT3) )
  #define HH_D(x)  ((x == 0) ? (LCDM24 &= ~BIT2):(LCDM24 |= BIT2) )
  #define HH_E(x)  ((x == 0) ? (LCDM24 &= ~BIT3):(LCDM24 |= BIT3) )
  #define HH_F(x)  ((x == 0) ? (LCDM24 &= ~BIT4):(LCDM24 |= BIT4) )
  #define HH_G(x)  ((x == 0) ? (LCDM23 &= ~BIT4):(LCDM23 |= BIT4) )

  #define HH(x) ( HH_A(((LCD_CHAR[x]>>6)&0x01)),\
                  HH_B(((LCD_CHAR[x]>>5)&0x01)),\
                  HH_C(((LCD_CHAR[x]>>4)&0x01)),\
                  HH_D(((LCD_CHAR[x]>>3)&0x01)),\
                  HH_E(((LCD_CHAR[x]>>2)&0x01)),\
                  HH_F(((LCD_CHAR[x]>>1)&0x01)),\
                  HH_G((LCD_CHAR[x]&0x01))    )


  #define HL_A(x)  ((x == 0) ? (LCDM22 &= ~BIT5):(LCDM22 |= BIT5) )
  #define HL_B(x)  ((x == 0) ? (LCDM21 &= ~BIT5):(LCDM21 |= BIT5) )
  #define HL_C(x)  ((x == 0) ? (LCDM21 &= ~BIT3):(LCDM21 |= BIT3) )
  #define HL_D(x)  ((x == 0) ? (LCDM22 &= ~BIT2):(LCDM22 |= BIT2) )
  #define HL_E(x)  ((x == 0) ? (LCDM22 &= ~BIT3):(LCDM22 |= BIT3) )
  #define HL_F(x)  ((x == 0) ? (LCDM22 &= ~BIT4):(LCDM22 |= BIT4) )
  #define HL_G(x)  ((x == 0) ? (LCDM21 &= ~BIT4):(LCDM21 |= BIT4) )

  #define HL(x) ( HL_A(((LCD_CHAR[x]>>6)&0x01)),\
                  HL_B(((LCD_CHAR[x]>>5)&0x01)),\
                  HL_C(((LCD_CHAR[x]>>4)&0x01)),\
                  HL_D(((LCD_CHAR[x]>>3)&0x01)),\
                  HL_E(((LCD_CHAR[x]>>2)&0x01)),\
                  HL_F(((LCD_CHAR[x]>>1)&0x01)),\
                  HL_G((LCD_CHAR[x]&0x01))    )




  #define Sec(x) ( (x==1)?(LCDM21 |= BIT2):(LCDM21 &= ~BIT2))



  #define MinH_A(x)  ((x == 0) ? (LCDM20 &= ~BIT5):(LCDM20 |= BIT5) )
  #define MinH_B(x)  ((x == 0) ? (LCDM19 &= ~BIT5):(LCDM19 |= BIT5) )
  #define MinH_C(x)  ((x == 0) ? (LCDM19 &= ~BIT3):(LCDM19 |= BIT3) )
  #define MinH_D(x)  ((x == 0) ? (LCDM20 &= ~BIT2):(LCDM20 |= BIT2) )
  #define MinH_E(x)  ((x == 0) ? (LCDM20 &= ~BIT3):(LCDM20 |= BIT3) )
  #define MinH_F(x)  ((x == 0) ? (LCDM20 &= ~BIT4):(LCDM20 |= BIT4) )
  #define MinH_G(x)  ((x == 0) ? (LCDM19 &= ~BIT4):(LCDM19 |= BIT4) )

  #define MinH(x) ( MinH_A(((LCD_CHAR[x]>>6)&0x01)),\
                    MinH_B(((LCD_CHAR[x]>>5)&0x01)),\
                    MinH_C(((LCD_CHAR[x]>>4)&0x01)),\
                    MinH_D(((LCD_CHAR[x]>>3)&0x01)),\
                    MinH_E(((LCD_CHAR[x]>>2)&0x01)),\
                    MinH_F(((LCD_CHAR[x]>>1)&0x01)),\
                    MinH_G((LCD_CHAR[x]&0x01))    )


  #define MinL_A(x)  ((x == 0) ? (LCDM18 &= ~BIT5):(LCDM18 |= BIT5) )
  #define MinL_B(x)  ((x == 0) ? (LCDM17 &= ~BIT5):(LCDM17 |= BIT5) )
  #define MinL_C(x)  ((x == 0) ? (LCDM17 &= ~BIT3):(LCDM17 |= BIT3) )
  #define MinL_D(x)  ((x == 0) ? (LCDM18 &= ~BIT2):(LCDM18 |= BIT2) )
  #define MinL_E(x)  ((x == 0) ? (LCDM18 &= ~BIT3):(LCDM18 |= BIT3) )
  #define MinL_F(x)  ((x == 0) ? (LCDM18 &= ~BIT4):(LCDM18 |= BIT4) )
  #define MinL_G(x)  ((x == 0) ? (LCDM17 &= ~BIT4):(LCDM17 |= BIT4) )

  #define MinL(x) (MinL_A(((LCD_CHAR[x]>>6)&0x01)),\
                   MinL_B(((LCD_CHAR[x]>>5)&0x01)),\
                   MinL_C(((LCD_CHAR[x]>>4)&0x01)),\
                   MinL_D(((LCD_CHAR[x]>>3)&0x01)),\
                   MinL_E(((LCD_CHAR[x]>>2)&0x01)),\
                   MinL_F(((LCD_CHAR[x]>>1)&0x01)),\
                   MinL_G((LCD_CHAR[x]&0x01))    )

  #define Relay(x)  ( (x==1)? (LCDM1|=BIT5,LCDM1&=~BIT4 ,\
                              LCDM3|=BIT4,\
                       LCDM2|=BIT5):\
                     ((LCDM1|=(BIT5 | BIT4)) ,\
                     LCDM3|=BIT4,\
                     LCDM2&=~BIT5 ))
                                     
                                                           
                      


  #define USB(x)   (((x==1)? (LCDM3 |=BIT5):(LCDM3&=~BIT5)))


  #define RS232(x)   ((x==1)? (LCDM4  |=  BIT5):(LCDM4  &=  ~BIT5))


  #define PB_10(x)  ((x == 1)? (LCDM9  |= BIT5):(LCDM9  &= ~BIT5))
  #define PB_20(x)  ((x == 1)? (LCDM10 |= BIT4):(LCDM10 &= ~BIT4))
  #define PB_30(x)  ((x == 1)? (LCDM11 |= BIT4):(LCDM11 &= ~BIT4))
  #define PB_40(x)  ((x == 1)? (LCDM12 |= BIT4):(LCDM12 &= ~BIT4))
  #define PB_50(x)  ((x == 1)? (LCDM13 |= BIT4):(LCDM13 &= ~BIT4))
  #define PB_60(x)  ((x == 1)? (LCDM14 |= BIT4):(LCDM14 &= ~BIT4))
  #define PB_70(x)  ((x == 1)? (LCDM15 |= BIT4):(LCDM15 &= ~BIT4))
  #define PB_80(x)  ((x == 1)? (LCDM15 |= BIT5):(LCDM15 &= ~BIT5))
  #define PB_90(x)  ((x == 1)? (LCDM16 |= BIT4):(LCDM16 &= ~BIT4))
  #define PB_100(x) ((x == 1)? (LCDM16 |= BIT5):(LCDM16 &= ~BIT5))


    
  #define PB(x) ( ((x >= 0) ? PB_10(1):PB_10(0)),\
                  ((x >= 10) ? PB_20(1):PB_20(0)),\
                  ((x >= 20) ? PB_30(1):PB_30(0)),\
                  ((x >= 30) ? PB_40(1):PB_40(0)),\
                  ((x >= 40) ? PB_50(1):PB_50(0)),\
                  ((x >= 50) ? PB_60(1):PB_60(0)),\
                  ((x >= 60) ? PB_70(1):PB_70(0)),\
                  ((x >= 70) ? PB_80(1):PB_80(0)),\
                  ((x >= 80) ? PB_90(1):PB_90(0)),\
                  ((x >= 90) ? PB_100(1):PB_100(0))\
                )

  #define LTamper(x) ((x==1)?(LCDM39 |=BIT0):(LCDM39 &=~BIT0) )

  #define UTamper(x) ((x==1)?(LCDM39 |=BIT1):(LCDM39 &=~BIT1) )


  #define MTamper(x) ((x==1)?(LCDM40 |=BIT1):(LCDM40 &=~BIT1) )


  #define NTamper(x) ((x==1)?(LCDM40 |=BIT0):(LCDM40 &=~BIT0) )


  #define showImportIndicator (LCDM23 |= BIT2, LCDM23 &=~ BIT1)
  #define showExportIndicator (LCDM23 &= ~BIT2, LCDM23 |= BIT1)


  #define EC (LCDM29 |= BIT2, LCDM29 &=  ~BIT1)
  #define GC (LCDM29 &= ~BIT2, LCDM29 |= BIT1)


  #define EN_Over(x)  ((x==1)?(LCDM34 |= BIT1):(LCDM34 &= ~BIT1))

  #define AM_Over(x)  ((x==1)?(LCDM2 |= BIT3):(LCDM2 &= ~BIT3))

  #define EN_Under(x)  ((x==1)?(LCDM37 |= BIT1):(LCDM37 &= ~BIT1))

  #define AM_Under(x)  ((x==1)?(LCDM2 |= BIT4):(LCDM2 &= ~BIT4))

  #define EN_Avarage(x)  ((x==1)?(LCDM34 |= BIT0):(LCDM34 &= ~BIT0))

  #define AM_Avarage(x)  ((x==1)?(LCDM4 |= BIT3):(LCDM4 &= ~BIT3))

  #define EN_Active(x)  ((x==1)?(LCDM36 |= BIT1):(LCDM36 &= ~BIT1))

  #define AM_Active(x)  ((x==1)?(LCDM4 |= BIT4):(LCDM4 &= ~BIT4))

  #define EN_Low(x)  ((x==1)?(LCDM33 |= BIT1):(LCDM33 &= ~BIT1))

  #define AM_Low(x)  ((x==1)?(LCDM5 |= BIT4):(LCDM5 &= ~BIT4))

  #define EN_Reactive(x)  ((x==1)?(LCDM35 |= BIT1):(LCDM35 &= ~BIT1))

  #define AM_Reactive(x)  ((x==1)?(LCDM5 |= BIT5):(LCDM5 &= ~BIT5))


  #define EN_Balance(x)  ((x==1)?(LCDM33 |= BIT0 ):(LCDM33 &= ~BIT0))

  #define AM_Balance(x)  ((x==1)?(LCDM6 |= BIT4 ):(LCDM6 &= ~BIT4))

  #define EN_Power(x)  ((x==1)?(LCDM32 |= BIT1  ):(LCDM32 &= ~BIT1))

  #define AM_Power(x)  ((x==1)?(LCDM6 |= BIT5  ):(LCDM6 &= ~BIT5))

  #define EN_Energy(x)  ((x==1)?(LCDM31 |= BIT1):(LCDM31 &= ~BIT1))

  #define AM_Energy(x)  ((x==1)?(LCDM7 |= BIT5):(LCDM7 &= ~BIT5))


  #define EN_Error(x)  ((x==1)?(LCDM31 |= BIT0):(LCDM31 &= ~BIT0))

  #define AM_Error(x)  ((x==1)?(LCDM7 |= BIT4):(LCDM7 &= ~BIT4))


  #define EN_Interruption(x)  ((x==1)?(LCDM30 |= BIT0):(LCDM30 &= ~BIT0))

  #define AM_Interruption(x)  ((x==1)?(LCDM8 |= BIT4):(LCDM8 &= ~BIT4))


  #define EN_CUM(x)  ((x==1)?(LCDM36 |= BIT0):(LCDM36 &= ~BIT0))

  #define AM_CUM(x)  ((x==1)?(LCDM8 |= BIT5):(LCDM8 &= ~BIT5))


  #define EN_DEM(x)  ((x==1)?(LCDM32 |= BIT0):(LCDM32 &= ~BIT0))

  #define AM_DEM(x)  ((x==1)?(LCDM9 |= BIT4):(LCDM9 &= ~BIT4))

  #define Voltage(x) ((x==1)?(LCDM38 |= BIT1):(LCDM38 &= ~BIT1))

  #define Current(x) ((x==1)?(LCDM38 |= BIT0):(LCDM38 &= ~BIT0))

  #define Frequency(x) ((x==1)?(LCDM37 |= BIT0):(LCDM37 &= ~BIT0))

  #define PF(x) ((x==1)?(LCDM30 |= BIT1):(LCDM30 &= ~BIT1))

  #define EN_MAX(x) ((x==1)?(LCDM35 |= BIT0):(LCDM35 &= ~BIT0))

  #define AM_MAX(x) ((x==1)?(LCDM2 |= BIT3):(LCDM2 &= ~BIT3))

  #define EN_DAY(x) ((x==1)?(LCDM22 |= BIT1):(LCDM22 &= ~BIT1))

  #define AM_DAY(x) ((x==1)?(LCDM20 |= BIT1):(LCDM20 &= ~BIT1))

  #define EN_Birr(x) ((x==1)?(LCDM22 |= BIT0):(LCDM22 &= ~BIT0))

  #define AM_Birr(x) ((x==1)?(LCDM21 |= BIT1):(LCDM21 &= ~BIT1))

  #define EN_MIN(x) ((x==1)?(LCDM21 |= BIT0):(LCDM21 &= ~BIT0))
  #define AM_MIN(x) ((x==1)?(LCDM16 |= BIT3):(LCDM16 &= ~BIT3))

  #define Point4(x) ((x==1)?(LCDM14 |= BIT3):(LCDM14 &= ~BIT3))
  #define Point3(x) ((x==1)?(LCDM12 |= BIT3):(LCDM12 &= ~BIT3))
  #define Point2(x) ((x==1)?(LCDM10 |= BIT3):(LCDM10 &= ~BIT3))
  #define Point1(x) ((x==1)?(LCDM8 |= BIT3):(LCDM8 &= ~BIT3))

  #define UNIT_K(x) ((x==1)?(LCDM28 |= BIT0):(LCDM28 &= ~BIT0))
  #define UINT_V(x) ((x==1)?(LCDM28 |= BIT1):(LCDM28 &= ~BIT1))
  #define UNIT_WForwardSlash(x) ((x==1)?(LCDM27 |= BIT1):(LCDM27 &= ~BIT1))
  #define UNIT_WBackSlash2(x) ((x==1)?(LCDM27 |= BIT0):(LCDM27 &= ~BIT0))
  #define UNIT_ABackSlash(x) ((x==1)?(LCDM26 |= BIT0):(LCDM26 &= ~BIT0))
  #define UNIT_R(x) ((x==1)?(LCDM26 |= BIT1):(LCDM26 &= ~BIT1))
  #define UNIT_H(x) ((x==1)?(LCDM24 |= BIT0):(LCDM24 &= ~BIT0))
  #define UNIT_Z(x) ((x==1)?(LCDM23 |= BIT0):(LCDM23 &= ~BIT0))

  #define UNIT_None   ( UNIT_K(0),UINT_V(0), UNIT_WForwardSlash(0),UNIT_WBackSlash2(0),UNIT_ABackSlash(0),\
                        UNIT_R(0),UNIT_H(0),UNIT_Z(0) )
  #define UNIT_All    ( UNIT_K(1),UINT_V(1), UNIT_WForwardSlash(1),UNIT_WBackSlash2(1),UNIT_ABackSlash(1),\
                        UNIT_R(1),UNIT_H(1),UNIT_Z(1) )

  #define UINT_Amp    ( UNIT_K(0),UINT_V(0), UNIT_WForwardSlash(0),UNIT_WBackSlash2(1),UNIT_ABackSlash(1),\
                      UNIT_R(0),UNIT_H(0),UNIT_Z(0) )
  #define UINT_Volt   ( UNIT_K(0),UINT_V(1), UNIT_WForwardSlash(0),UNIT_WBackSlash2(0),UNIT_ABackSlash(0),\
                      UNIT_R(0),UNIT_H(0),UNIT_Z(0) )
  #define UINT_KV     ( UNIT_K(1),UINT_V(1), UNIT_WForwardSlash(0),UNIT_WBackSlash2(0),UNIT_ABackSlash(0),\
                      UNIT_R(0),UNIT_H(0),UNIT_Z(0) )
  #define UINT_HZ     ( UNIT_K(0),UINT_V(0), UNIT_WForwardSlash(0),UNIT_WBackSlash2(0),UNIT_ABackSlash(0),\
                      UNIT_R(0),UNIT_H(1),UNIT_Z(1) )
  #define UINT_KHZ    ( UNIT_K(1),UINT_V(0), UNIT_WForwardSlash(0),UNIT_WBackSlash2(0),UNIT_ABackSlash(0),\
                      UNIT_R(0),UNIT_H(1),UNIT_Z(1) )
  #define UNIT_W      ( UNIT_K(0),UINT_V(1), UNIT_WForwardSlash(1),UNIT_WBackSlash2(1),UNIT_ABackSlash(0),\
                      UNIT_R(0),UNIT_H(0),UNIT_Z(0) )
  #define UNIT_KW     ( UNIT_K(1),UINT_V(1), UNIT_WForwardSlash(1),UNIT_WBackSlash2(1),UNIT_ABackSlash(0),\
                      UNIT_R(0),UNIT_H(0),UNIT_Z(0) )
  #define UINT_VAr     ( UNIT_K(0),UINT_V(1), UNIT_WForwardSlash(0),UNIT_WBackSlash2(1),UNIT_ABackSlash(1),\
                      UNIT_R(1),UNIT_H(0),UNIT_Z(0) )
  #define UINT_KVA    ( UNIT_K(1),UINT_V(1), UNIT_WForwardSlash(0),UNIT_WBackSlash2(1),UNIT_ABackSlash(1),\
                      UNIT_R(0),UNIT_H(0),UNIT_Z(0) )
  #define UINT_WH    ( UNIT_K(0),UINT_V(1), UNIT_WForwardSlash(1),UNIT_WBackSlash2(1),UNIT_ABackSlash(0),\
                      UNIT_R(0),UNIT_H(1),UNIT_Z(0) )
  #define UINT_KWH    ( UNIT_K(1),UINT_V(1), UNIT_WForwardSlash(1),UNIT_WBackSlash2(1),UNIT_ABackSlash(0),\
                      UNIT_R(0),UNIT_H(1),UNIT_Z(0) )
  #define UNIT_VARH  ( UNIT_K(0),UINT_V(1), UNIT_WForwardSlash(0),UNIT_WBackSlash2(1),UNIT_ABackSlash(1),\
                      UNIT_R(1),UNIT_H(1),UNIT_Z(0) )
  #define UNIT_KVARH  (  UNIT_K(1),UINT_V(1), UNIT_WForwardSlash(0),UNIT_WBackSlash2(1),UNIT_ABackSlash(1),\
                      UNIT_R(1),UNIT_H(1),UNIT_Z(0) )





#elif defined(LCD_REV_3)   //LCD Revision 3





  #define D8_A(x)  ((x == 0) ? (LCDM1 &= ~BIT0):(LCDM1 |= BIT0) )
  #define D8_B(x)  ((x == 0) ? (LCDM2 &= ~BIT1):(LCDM2 |= BIT1) )
  #define D8_C(x)  ((x == 0) ? (LCDM2 &= ~BIT3):(LCDM2 |= BIT3) )
  #define D8_D(x)  ((x == 0) ? (LCDM1 &= ~BIT3):(LCDM1 |= BIT3) )
  #define D8_E(x)  ((x == 0) ? (LCDM1 &= ~BIT2):(LCDM1 |= BIT2) )
  #define D8_F(x)  ((x == 0) ? (LCDM1 &= ~BIT1):(LCDM1 |= BIT1) )
  #define D8_G(x)  ((x == 0) ? (LCDM2 &= ~BIT2):(LCDM2 |= BIT2) )

  #define D8(x) ( D8_A(((LCD_CHAR[x]>>6)&0x01)),\
                  D8_B(((LCD_CHAR[x]>>5)&0x01)),\
                  D8_C(((LCD_CHAR[x]>>4)&0x01)),\
                  D8_D(((LCD_CHAR[x]>>3)&0x01)),\
                  D8_E(((LCD_CHAR[x]>>2)&0x01)),\
                  D8_F(((LCD_CHAR[x]>>1)&0x01)),\
                  D8_G((LCD_CHAR[x]&0x01))     )

  #define D7_A(x)  ((x == 0) ? (LCDM3 &= ~BIT0):(LCDM3 |= BIT0) )
  #define D7_B(x)  ((x == 0) ? (LCDM4 &= ~BIT1):(LCDM4 |= BIT1) )
  #define D7_C(x)  ((x == 0) ? (LCDM4 &= ~BIT3):(LCDM4 |= BIT3) )
  #define D7_D(x)  ((x == 0) ? (LCDM3 &= ~BIT3):(LCDM3 |= BIT3) )
  #define D7_E(x)  ((x == 0) ? (LCDM3 &= ~BIT2):(LCDM3 |= BIT2) )
  #define D7_F(x)  ((x == 0) ? (LCDM3 &= ~BIT1):(LCDM3 |= BIT1) )
  #define D7_G(x)  ((x == 0) ? (LCDM4 &= ~BIT2):(LCDM4 |= BIT2) )

  #define D7(x) ( D7_A(((LCD_CHAR[x]>>6)&0x01)),\
                  D7_B(((LCD_CHAR[x]>>5)&0x01)),\
                  D7_C(((LCD_CHAR[x]>>4)&0x01)),\
                  D7_D(((LCD_CHAR[x]>>3)&0x01)),\
                  D7_E(((LCD_CHAR[x]>>2)&0x01)),\
                  D7_F(((LCD_CHAR[x]>>1)&0x01)),\
                  D7_G((LCD_CHAR[x]&0x01))     )


  #define D6_A(x)  ((x == 0) ? (LCDM5 &= ~BIT0):(LCDM5 |= BIT0) )
  #define D6_B(x)  ((x == 0) ? (LCDM6 &= ~BIT1):(LCDM6 |= BIT1) )
  #define D6_C(x)  ((x == 0) ? (LCDM6 &= ~BIT3):(LCDM6 |= BIT3) )
  #define D6_D(x)  ((x == 0) ? (LCDM5 &= ~BIT3):(LCDM5 |= BIT3) )
  #define D6_E(x)  ((x == 0) ? (LCDM5 &= ~BIT2):(LCDM5 |= BIT2) )
  #define D6_F(x)  ((x == 0) ? (LCDM5 &= ~BIT1):(LCDM5 |= BIT1) )
  #define D6_G(x)  ((x == 0) ? (LCDM6 &= ~BIT2):(LCDM6 |= BIT2) )


  #define D6(x) ( D6_A(((LCD_CHAR[x]>>6)&0x01)),\
                  D6_B(((LCD_CHAR[x]>>5)&0x01)),\
                  D6_C(((LCD_CHAR[x]>>4)&0x01)),\
                  D6_D(((LCD_CHAR[x]>>3)&0x01)),\
                  D6_E(((LCD_CHAR[x]>>2)&0x01)),\
                  D6_F(((LCD_CHAR[x]>>1)&0x01)),\
                  D6_G((LCD_CHAR[x]&0x01))     )


  #define D5_A(x)  ((x == 0) ? (LCDM7 &= ~BIT0):(LCDM7 |= BIT0) )
  #define D5_B(x)  ((x == 0) ? (LCDM8 &= ~BIT1):(LCDM8 |= BIT1) )
  #define D5_C(x)  ((x == 0) ? (LCDM8 &= ~BIT3):(LCDM8 |= BIT3) )
  #define D5_D(x)  ((x == 0) ? (LCDM7 &= ~BIT3):(LCDM7 |= BIT3) )
  #define D5_E(x)  ((x == 0) ? (LCDM7 &= ~BIT2):(LCDM7 |= BIT2) )
  #define D5_F(x)  ((x == 0) ? (LCDM7 &= ~BIT1):(LCDM7 |= BIT1) )
  #define D5_G(x)  ((x == 0) ? (LCDM8 &= ~BIT2):(LCDM8 |= BIT2) )

  #define D5(x) ( D5_A(((LCD_CHAR[x]>>6)&0x01)),\
                  D5_B(((LCD_CHAR[x]>>5)&0x01)),\
                  D5_C(((LCD_CHAR[x]>>4)&0x01)),\
                  D5_D(((LCD_CHAR[x]>>3)&0x01)),\
                  D5_E(((LCD_CHAR[x]>>2)&0x01)),\
                  D5_F(((LCD_CHAR[x]>>1)&0x01)),\
                  D5_G((LCD_CHAR[x]&0x01))     )



  #define D4_A(x)  ((x == 0) ? (LCDM9 &= ~BIT0):(LCDM9 |= BIT0) )
  #define D4_B(x)  ((x == 0) ? (LCDM10 &= ~BIT1):(LCDM10 |= BIT1) )
  #define D4_C(x)  ((x == 0) ? (LCDM10 &= ~BIT3):(LCDM10 |= BIT3) )
  #define D4_D(x)  ((x == 0) ? (LCDM9 &= ~BIT3):(LCDM9 |= BIT3) )
  #define D4_E(x)  ((x == 0) ? (LCDM9 &= ~BIT2):(LCDM9 |= BIT2) )
  #define D4_F(x)  ((x == 0) ? (LCDM9 &= ~BIT1):(LCDM9 |= BIT1) )
  #define D4_G(x)  ((x == 0) ? (LCDM10 &= ~BIT2):(LCDM10 |= BIT2) )

  #define D4(x) ( D4_A(((LCD_CHAR[x]>>6)&0x01)),\
                  D4_B(((LCD_CHAR[x]>>5)&0x01)),\
                  D4_C(((LCD_CHAR[x]>>4)&0x01)),\
                  D4_D(((LCD_CHAR[x]>>3)&0x01)),\
                  D4_E(((LCD_CHAR[x]>>2)&0x01)),\
                  D4_F(((LCD_CHAR[x]>>1)&0x01)),\
                  D4_G((LCD_CHAR[x]&0x01))     )


  #define D3_A(x)  ((x == 0) ? (LCDM11 &= ~BIT0):(LCDM11 |= BIT0) )
  #define D3_B(x)  ((x == 0) ? (LCDM12 &= ~BIT1):(LCDM12 |= BIT1) )
  #define D3_C(x)  ((x == 0) ? (LCDM12 &= ~BIT3):(LCDM12 |= BIT3) )
  #define D3_D(x)  ((x == 0) ? (LCDM11 &= ~BIT3):(LCDM11 |= BIT3) )
  #define D3_E(x)  ((x == 0) ? (LCDM11 &= ~BIT2):(LCDM11 |= BIT2) )
  #define D3_F(x)  ((x == 0) ? (LCDM11 &= ~BIT1):(LCDM11 |= BIT1) )
  #define D3_G(x)  ((x == 0) ? (LCDM12 &= ~BIT2):(LCDM12 |= BIT2) )

  #define D3(x) ( D3_A(((LCD_CHAR[x]>>6)&0x01)),\
                  D3_B(((LCD_CHAR[x]>>5)&0x01)),\
                  D3_C(((LCD_CHAR[x]>>4)&0x01)),\
                  D3_D(((LCD_CHAR[x]>>3)&0x01)),\
                  D3_E(((LCD_CHAR[x]>>2)&0x01)),\
                  D3_F(((LCD_CHAR[x]>>1)&0x01)),\
                  D3_G((LCD_CHAR[x]&0x01))     )


  #define D2_A(x)  ((x == 0) ? (LCDM13 &= ~BIT0):(LCDM13 |= BIT0) )
  #define D2_B(x)  ((x == 0) ? (LCDM14 &= ~BIT1):(LCDM14 |= BIT1) )
  #define D2_C(x)  ((x == 0) ? (LCDM14 &= ~BIT3):(LCDM14 |= BIT3) )
  #define D2_D(x)  ((x == 0) ? (LCDM13 &= ~BIT3):(LCDM13 |= BIT3) )
  #define D2_E(x)  ((x == 0) ? (LCDM13 &= ~BIT2):(LCDM13 |= BIT2) )
  #define D2_F(x)  ((x == 0) ? (LCDM13 &= ~BIT1):(LCDM13 |= BIT1) )
  #define D2_G(x)  ((x == 0) ? (LCDM14 &= ~BIT2):(LCDM14 |= BIT2) )

  #define D2(x) ( D2_A(((LCD_CHAR[x]>>6)&0x01)),\
                  D2_B(((LCD_CHAR[x]>>5)&0x01)),\
                  D2_C(((LCD_CHAR[x]>>4)&0x01)),\
                  D2_D(((LCD_CHAR[x]>>3)&0x01)),\
                  D2_E(((LCD_CHAR[x]>>2)&0x01)),\
                  D2_F(((LCD_CHAR[x]>>1)&0x01)),\
                  D2_G((LCD_CHAR[x]&0x01))     )



  #define D1_A(x)  ((x == 0) ?( LCDM15 &= ~BIT0):(LCDM15 |= BIT0) )
  #define D1_B(x)  ((x == 0) ? (LCDM16 &= ~BIT1):(LCDM16 |= BIT1) )
  #define D1_C(x)  ((x == 0) ? (LCDM16 &= ~BIT3):(LCDM16 |= BIT3) )
  #define D1_D(x)  ((x == 0) ? (LCDM15 &= ~BIT3):(LCDM15 |= BIT3) )
  #define D1_E(x)  ((x == 0) ? (LCDM15 &= ~BIT2):(LCDM15 |= BIT2) )
  #define D1_F(x)  ((x == 0) ? (LCDM15 &= ~BIT1):(LCDM15 |= BIT1) )
  #define D1_G(x)  ((x == 0) ? (LCDM16 &= ~BIT2):(LCDM16 |= BIT2) )

  //void D1(uint8_t x);
   

  #define D1(x) ( D1_A(((LCD_CHAR[x]>>6)&0x01)),\
                  D1_B(((LCD_CHAR[x]>>5)&0x01)),\
                  D1_C(((LCD_CHAR[x]>>4)&0x01)),\
                  D1_D(((LCD_CHAR[x]>>3)&0x01)),\
                  D1_E(((LCD_CHAR[x]>>2)&0x01)),\
                  D1_F(((LCD_CHAR[x]>>1)&0x01)),\
                  D1_G(((LCD_CHAR[x])&0x01))    )




  #define YH_A(x)  ((x == 0) ? (LCDM32 &= ~BIT5):(LCDM32 |= BIT5) )
  #define YH_B(x)  ((x == 0) ? (LCDM31 &= ~BIT4):(LCDM31 |= BIT4) )
  #define YH_C(x)  ((x == 0) ? (LCDM31 &= ~BIT2):(LCDM31 |= BIT2) )
  #define YH_D(x)  ((x == 0) ? (LCDM32 &= ~BIT2):(LCDM32 |= BIT2) )
  #define YH_E(x)  ((x == 0) ? (LCDM32 &= ~BIT3):(LCDM32 |= BIT3) )
  #define YH_F(x)  ((x == 0) ? (LCDM32 &= ~BIT4):(LCDM32 |= BIT4) )
  #define YH_G(x)  ((x == 0) ? (LCDM31 &= ~BIT3):(LCDM31 |= BIT3) )

  #define YH(x) ( YH_A(((LCD_CHAR[x]>>6)&0x01)),\
                  YH_B(((LCD_CHAR[x]>>5)&0x01)),\
                  YH_C(((LCD_CHAR[x]>>4)&0x01)),\
                  YH_D(((LCD_CHAR[x]>>3)&0x01)),\
                  YH_E(((LCD_CHAR[x]>>2)&0x01)),\
                  YH_F(((LCD_CHAR[x]>>1)&0x01)),\
                  YH_G((LCD_CHAR[x]&0x01))    )


  #define YL_A(x)  ((x == 0) ? (LCDM30 &= ~BIT5):(LCDM30 |= BIT5) )
  #define YL_B(x)  ((x == 0) ? (LCDM29 &= ~BIT4):(LCDM29 |= BIT4) )
  #define YL_C(x)  ((x == 0) ? (LCDM29 &= ~BIT2):(LCDM29 |= BIT2) )
  #define YL_D(x)  ((x == 0) ? (LCDM30 &= ~BIT2):(LCDM30 |= BIT2) )
  #define YL_E(x)  ((x == 0) ? (LCDM30 &= ~BIT3):(LCDM30 |= BIT3) )
  #define YL_F(x)  ((x == 0) ? (LCDM30 &= ~BIT4):(LCDM30 |= BIT4) )
  #define YL_G(x)  ((x == 0) ? (LCDM29 &= ~BIT3):(LCDM29 |= BIT3) )

  #define YL(x) ( YL_A(((LCD_CHAR[x]>>6)&0x01)),\
                  YL_B(((LCD_CHAR[x]>>5)&0x01)),\
                  YL_C(((LCD_CHAR[x]>>4)&0x01)),\
                  YL_D(((LCD_CHAR[x]>>3)&0x01)),\
                  YL_E(((LCD_CHAR[x]>>2)&0x01)),\
                  YL_F(((LCD_CHAR[x]>>1)&0x01)),\
                  YL_G((LCD_CHAR[x]&0x01))    )


  #define YM_Bar(x) ((x == 1) ? (LCDM37 |= BIT1):(LCDM37 &= ~BIT1))


  #define MH_A(x)  ((x == 0) ? (LCDM36 &= ~BIT5):(LCDM36 |= BIT5) )
  #define MH_B(x)  ((x == 0) ? (LCDM35 &= ~BIT4):(LCDM35 |= BIT4) )
  #define MH_C(x)  ((x == 0) ? (LCDM35 &= ~BIT2):(LCDM35 |= BIT2) )
  #define MH_D(x)  ((x == 0) ? (LCDM36 &= ~BIT2):(LCDM36 |= BIT2) )
  #define MH_E(x)  ((x == 0) ? (LCDM36 &= ~BIT3):(LCDM36 |= BIT3) )
  #define MH_F(x)  ((x == 0) ? (LCDM36 &= ~BIT4):(LCDM36 |= BIT4) )
  #define MH_G(x)  ((x == 0) ? (LCDM35 &= ~BIT3):(LCDM35 |= BIT3) )

  #define MH(x) ( MH_A(((LCD_CHAR[x]>>6)&0x01)),\
                  MH_B(((LCD_CHAR[x]>>5)&0x01)),\
                  MH_C(((LCD_CHAR[x]>>4)&0x01)),\
                  MH_D(((LCD_CHAR[x]>>3)&0x01)),\
                  MH_E(((LCD_CHAR[x]>>2)&0x01)),\
                  MH_F(((LCD_CHAR[x]>>1)&0x01)),\
                  MH_G((LCD_CHAR[x]&0x01))    )

  #define ML_A(x)  ((x == 0) ? (LCDM34 &= ~BIT5):(LCDM34 |= BIT5) )
  #define ML_B(x)  ((x == 0) ? (LCDM33 &= ~BIT4):(LCDM33 |= BIT4) )
  #define ML_C(x)  ((x == 0) ? (LCDM33 &= ~BIT2):(LCDM33 |= BIT2) )
  #define ML_D(x)  ((x == 0) ? (LCDM34 &= ~BIT2):(LCDM34 |= BIT2) )
  #define ML_E(x)  ((x == 0) ? (LCDM34 &= ~BIT3):(LCDM34 |= BIT3) )
  #define ML_F(x)  ((x == 0) ? (LCDM34 &= ~BIT4):(LCDM34 |= BIT4) )
  #define ML_G(x)  ((x == 0) ? (LCDM33 &= ~BIT3):(LCDM33 |= BIT3) )

  #define ML(x) ( ML_A(((LCD_CHAR[x]>>6)&0x01)),\
                  ML_B(((LCD_CHAR[x]>>5)&0x01)),\
                  ML_C(((LCD_CHAR[x]>>4)&0x01)),\
                  ML_D(((LCD_CHAR[x]>>3)&0x01)),\
                  ML_E(((LCD_CHAR[x]>>2)&0x01)),\
                  ML_F(((LCD_CHAR[x]>>1)&0x01)),\
                  ML_G((LCD_CHAR[x]&0x01))    )


  #define MD_Bar(x) ( (x == 1) ? (LCDM33 |= BIT1):(LCDM33 &= ~BIT1))


  #define DH_A(x)  ((x == 0) ? (LCDM40 &= ~BIT5):(LCDM40 |= BIT5) )
  #define DH_B(x)  ((x == 0) ? (LCDM39 &= ~BIT4):(LCDM39 |= BIT4) )
  #define DH_C(x)  ((x == 0) ? (LCDM39 &= ~BIT2):(LCDM39 |= BIT2) )
  #define DH_D(x)  ((x == 0) ? (LCDM40 &= ~BIT2):(LCDM40 |= BIT2) )
  #define DH_E(x)  ((x == 0) ? (LCDM40 &= ~BIT3):(LCDM40 |= BIT3) )
  #define DH_F(x)  ((x == 0) ? (LCDM40 &= ~BIT4):(LCDM40 |= BIT4) )
  #define DH_G(x)  ((x == 0) ? (LCDM39 &= ~BIT3):(LCDM39 |= BIT3) )

  #define DH(x) ( DH_A(((LCD_CHAR[x]>>6)&0x01)),\
                  DH_B(((LCD_CHAR[x]>>5)&0x01)),\
                  DH_C(((LCD_CHAR[x]>>4)&0x01)),\
                  DH_D(((LCD_CHAR[x]>>3)&0x01)),\
                  DH_E(((LCD_CHAR[x]>>2)&0x01)),\
                  DH_F(((LCD_CHAR[x]>>1)&0x01)),\
                  DH_G((LCD_CHAR[x]&0x01))    )

  #define DL_A(x)  ((x == 0) ? (LCDM38 &= ~BIT5):(LCDM38 |= BIT5) )
  #define DL_B(x)  ((x == 0) ? (LCDM37 &= ~BIT4):(LCDM37 |= BIT4) )
  #define DL_C(x)  ((x == 0) ? (LCDM37 &= ~BIT2):(LCDM37 |= BIT2) )
  #define DL_D(x)  ((x == 0) ? (LCDM38 &= ~BIT2):(LCDM38 |= BIT2) )
  #define DL_E(x)  ((x == 0) ? (LCDM38 &= ~BIT3):(LCDM38 |= BIT3) )
  #define DL_F(x)  ((x == 0) ? (LCDM38 &= ~BIT4):(LCDM38 |= BIT4) )
  #define DL_G(x)  ((x == 0) ? (LCDM37 &= ~BIT3):(LCDM37 |= BIT3) )

  #define DL(x) ( DL_A(((LCD_CHAR[x]>>6)&0x01)),\
                  DL_B(((LCD_CHAR[x]>>5)&0x01)),\
                  DL_C(((LCD_CHAR[x]>>4)&0x01)),\
                  DL_D(((LCD_CHAR[x]>>3)&0x01)),\
                  DL_E(((LCD_CHAR[x]>>2)&0x01)),\
                  DL_F(((LCD_CHAR[x]>>1)&0x01)),\
                  DL_G((LCD_CHAR[x]&0x01))    )


  #define T_Slash(x)  ((x == 1)?(LCDM28 |= BIT1):(LCDM28 &= ~BIT1))
  #define Tariff(x)   ((x == 1)?((LCDM29 |= BIT1),T_Slash(1)):((LCDM29 &= ~BIT1),T_Slash(0)))


  #define TH_A(x)  ((x == 0) ? (LCDM28 &= ~BIT5):(LCDM28 |= BIT5) )
  #define TH_B(x)  ((x == 0) ? (LCDM27 &= ~BIT4):(LCDM27 |= BIT4) )
  #define TH_C(x)  ((x == 0) ? (LCDM27 &= ~BIT2):(LCDM27 |= BIT2) )
  #define TH_D(x)  ((x == 0) ? (LCDM28 &= ~BIT2):(LCDM28 |= BIT2) )
  #define TH_E(x)  ((x == 0) ? (LCDM28 &= ~BIT3):(LCDM28 |= BIT3) )
  #define TH_F(x)  ((x == 0) ? (LCDM28 &= ~BIT4):(LCDM28 |= BIT4) )
  #define TH_G(x)  ((x == 0) ? (LCDM27 &= ~BIT3):(LCDM27 |= BIT3) )

  #define TH(x) ( TH_A(((LCD_CHAR[x]>>6)&0x01)),\
                  TH_B(((LCD_CHAR[x]>>5)&0x01)),\
                  TH_C(((LCD_CHAR[x]>>4)&0x01)),\
                  TH_D(((LCD_CHAR[x]>>3)&0x01)),\
                  TH_E(((LCD_CHAR[x]>>2)&0x01)),\
                  TH_F(((LCD_CHAR[x]>>1)&0x01)),\
                  TH_G((LCD_CHAR[x]&0x01))    )



  #define TL_A(x)  ((x == 0) ? (LCDM26 &= ~BIT5):(LCDM26 |= BIT5) )
  #define TL_B(x)  ((x == 0) ? (LCDM25 &= ~BIT4):(LCDM25 |= BIT4) )
  #define TL_C(x)  ((x == 0) ? (LCDM25 &= ~BIT2):(LCDM25 |= BIT2) )
  #define TL_D(x)  ((x == 0) ? (LCDM26 &= ~BIT2):(LCDM26 |= BIT2) )
  #define TL_E(x)  ((x == 0) ? (LCDM26 &= ~BIT3):(LCDM26 |= BIT3) )
  #define TL_F(x)  ((x == 0) ? (LCDM26 &= ~BIT4):(LCDM26 |= BIT4) )
  #define TL_G(x)  ((x == 0) ? (LCDM25 &= ~BIT3):(LCDM25 |= BIT3) )

  #define TL(x) ( TL_A(((LCD_CHAR[x]>>6)&0x01)),\
                  TL_B(((LCD_CHAR[x]>>5)&0x01)),\
                  TL_C(((LCD_CHAR[x]>>4)&0x01)),\
                  TL_D(((LCD_CHAR[x]>>3)&0x01)),\
                  TL_E(((LCD_CHAR[x]>>2)&0x01)),\
                  TL_F(((LCD_CHAR[x]>>1)&0x01)),\
                  TL_G((LCD_CHAR[x]&0x01))    )

  #define showImportIndicator (LCDM25 |= BIT1, LCDM26 |= BIT1, LCDM27 &=~ BIT1)
  #define showExportIndicator (LCDM27 |= BIT1, LCDM26 |= BIT1, LCDM25 &= ~BIT1)

  #define Batt_L7(x) ((x == 1)? (LCDM28 |= BIT0):(LCDM28 &= ~BIT0))
  #define Batt_L6(x) ((x == 1)? (LCDM27 |= BIT0):(LCDM27 &= ~BIT0))
  #define Batt_L5(x) ((x == 1)? (LCDM26 |= BIT0):(LCDM26 &= ~BIT0))
  #define Batt_L4(x) ((x == 1)? (LCDM25 |= BIT0):(LCDM25 &= ~BIT0))

  #define Battery(x) ( ((x >= 7)? (Batt_L7(1)):(Batt_L7(0))),\
                       ((x >= 6)? (Batt_L6(1)):(Batt_L6(0))),\
                       ((x >= 5)? (Batt_L5(1)):(Batt_L5(0))),\
                       ((x >= 4)? (Batt_L4(1)):(Batt_L4(0))),\
                        (LCDM29 |= BIT0)  )
                      
  #define PLC(x) ((x == 0) ? (LCDM30 &= ~BIT0):(LCDM30 |= BIT0) )

  #define Net_L7(x) ((x == 1)? (LCDM24 |= BIT1):(LCDM24 &= ~BIT1))
  #define Net_L6(x) ((x == 1)? (LCDM24 |= BIT0):(LCDM24 &= ~BIT0))
  #define Net_L5(x) ((x == 1)? (LCDM32 |= BIT0):(LCDM32 &= ~BIT0))
  #define Net_L4(x) ((x == 1)? (LCDM31 |= BIT0):(LCDM31 &= ~BIT0))

  #define Network(x) ( ((x >= 7)? (Net_L7(1)):(Net_L7(0))),\
                       ((x >= 6)? (Net_L6(1)):(Net_L6(0))),\
                       ((x >= 5)? (Net_L5(1)):(Net_L5(0))),\
                       ((x >= 4)? (Net_L4(1)):(Net_L4(0))))


  #define HH_A(x)  ((x == 0) ? (LCDM24 &= ~BIT5):(LCDM24 |= BIT5) )
  #define HH_B(x)  ((x == 0) ? (LCDM23 &= ~BIT4):(LCDM23 |= BIT4) )
  #define HH_C(x)  ((x == 0) ? (LCDM23 &= ~BIT2):(LCDM23 |= BIT2) )
  #define HH_D(x)  ((x == 0) ? (LCDM24 &= ~BIT2):(LCDM24 |= BIT2) )
  #define HH_E(x)  ((x == 0) ? (LCDM24 &= ~BIT3):(LCDM24 |= BIT3) )
  #define HH_F(x)  ((x == 0) ? (LCDM24 &= ~BIT4):(LCDM24 |= BIT4) )
  #define HH_G(x)  ((x == 0) ? (LCDM23 &= ~BIT3):(LCDM23 |= BIT3) )

  #define HH(x) ( HH_A(((LCD_CHAR[x]>>6)&0x01)),\
                  HH_B(((LCD_CHAR[x]>>5)&0x01)),\
                  HH_C(((LCD_CHAR[x]>>4)&0x01)),\
                  HH_D(((LCD_CHAR[x]>>3)&0x01)),\
                  HH_E(((LCD_CHAR[x]>>2)&0x01)),\
                  HH_F(((LCD_CHAR[x]>>1)&0x01)),\
                  HH_G((LCD_CHAR[x]&0x01))    )


  #define HL_A(x)  ((x == 0) ? (LCDM22 &= ~BIT5):(LCDM22 |= BIT5) )
  #define HL_B(x)  ((x == 0) ? (LCDM21 &= ~BIT4):(LCDM21 |= BIT4) )
  #define HL_C(x)  ((x == 0) ? (LCDM21 &= ~BIT2):(LCDM21 |= BIT2) )
  #define HL_D(x)  ((x == 0) ? (LCDM22 &= ~BIT2):(LCDM22 |= BIT2) )
  #define HL_E(x)  ((x == 0) ? (LCDM22 &= ~BIT3):(LCDM22 |= BIT3) )
  #define HL_F(x)  ((x == 0) ? (LCDM22 &= ~BIT4):(LCDM22 |= BIT4) )
  #define HL_G(x)  ((x == 0) ? (LCDM21 &= ~BIT3):(LCDM21 |= BIT3) )

  #define HL(x) ( HL_A(((LCD_CHAR[x]>>6)&0x01)),\
                  HL_B(((LCD_CHAR[x]>>5)&0x01)),\
                  HL_C(((LCD_CHAR[x]>>4)&0x01)),\
                  HL_D(((LCD_CHAR[x]>>3)&0x01)),\
                  HL_E(((LCD_CHAR[x]>>2)&0x01)),\
                  HL_F(((LCD_CHAR[x]>>1)&0x01)),\
                  HL_G((LCD_CHAR[x]&0x01))    )




  #define Sec(x) ( (x==1)?(LCDM21 |= BIT5):(LCDM21 &= ~BIT5))



  #define MinH_A(x)  ((x == 0) ? (LCDM20 &= ~BIT5):(LCDM20 |= BIT5) )
  #define MinH_B(x)  ((x == 0) ? (LCDM19 &= ~BIT4):(LCDM19 |= BIT4) )
  #define MinH_C(x)  ((x == 0) ? (LCDM19 &= ~BIT2):(LCDM19 |= BIT2) )
  #define MinH_D(x)  ((x == 0) ? (LCDM20 &= ~BIT2):(LCDM20 |= BIT2) )
  #define MinH_E(x)  ((x == 0) ? (LCDM20 &= ~BIT3):(LCDM20 |= BIT3) )
  #define MinH_F(x)  ((x == 0) ? (LCDM20 &= ~BIT4):(LCDM20 |= BIT4) )
  #define MinH_G(x)  ((x == 0) ? (LCDM19 &= ~BIT3):(LCDM19 |= BIT3) )

  #define MinH(x) ( MinH_A(((LCD_CHAR[x]>>6)&0x01)),\
                    MinH_B(((LCD_CHAR[x]>>5)&0x01)),\
                    MinH_C(((LCD_CHAR[x]>>4)&0x01)),\
                    MinH_D(((LCD_CHAR[x]>>3)&0x01)),\
                    MinH_E(((LCD_CHAR[x]>>2)&0x01)),\
                    MinH_F(((LCD_CHAR[x]>>1)&0x01)),\
                    MinH_G((LCD_CHAR[x]&0x01))    )


  #define MinL_A(x)  ((x == 0) ? (LCDM18 &= ~BIT5):(LCDM18 |= BIT5) )
  #define MinL_B(x)  ((x == 0) ? (LCDM17 &= ~BIT4):(LCDM17 |= BIT4) )
  #define MinL_C(x)  ((x == 0) ? (LCDM17 &= ~BIT2):(LCDM17 |= BIT2) )
  #define MinL_D(x)  ((x == 0) ? (LCDM18 &= ~BIT2):(LCDM18 |= BIT2) )
  #define MinL_E(x)  ((x == 0) ? (LCDM18 &= ~BIT3):(LCDM18 |= BIT3) )
  #define MinL_F(x)  ((x == 0) ? (LCDM18 &= ~BIT4):(LCDM18 |= BIT4) )
  #define MinL_G(x)  ((x == 0) ? (LCDM17 &= ~BIT3):(LCDM17 |= BIT3) )

  #define MinL(x) (MinL_A(((LCD_CHAR[x]>>6)&0x01)),\
                   MinL_B(((LCD_CHAR[x]>>5)&0x01)),\
                   MinL_C(((LCD_CHAR[x]>>4)&0x01)),\
                   MinL_D(((LCD_CHAR[x]>>3)&0x01)),\
                   MinL_E(((LCD_CHAR[x]>>2)&0x01)),\
                   MinL_F(((LCD_CHAR[x]>>1)&0x01)),\
                   MinL_G((LCD_CHAR[x]&0x01))    )


  #define Relay(x)  ( (x==1)? (LCDM1|=BIT5,LCDM2|=BIT5 ,\
                              LCDM3|=BIT5,LCDM1&=~BIT4):\
                              ((LCDM1|=(BIT5 | BIT4)) ,\
                              LCDM3|=BIT5,\
                              LCDM2&=~BIT5 ))
                                     
                                                           
                      


  #define USB(x)   (((x==1)? (LCDM4 |=BIT5):(LCDM4&=~BIT5)))


  #define RS232(x)   ((x==1)? (LCDM5  |=  BIT5):(LCDM5  &=  ~BIT5))


  #define PB_10(x)  ((x == 1)? (LCDM9  |= BIT5):(LCDM9  &= ~BIT5))
  #define PB_20(x)  ((x == 1)? (LCDM12 |= BIT5):(LCDM12 &= ~BIT5))
  #define PB_30(x)  ((x == 1)? (LCDM13 |= BIT5):(LCDM13 &= ~BIT5))
  #define PB_40(x)  ((x == 1)? (LCDM13 |= BIT4):(LCDM13 &= ~BIT4))
  #define PB_50(x)  ((x == 1)? (LCDM14 |= BIT5):(LCDM14 &= ~BIT5))
  #define PB_60(x)  ((x == 1)? (LCDM15 |= BIT5):(LCDM15 &= ~BIT5))
  #define PB_70(x)  ((x == 1)? (LCDM16 |= BIT5):(LCDM16 &= ~BIT5))
  #define PB_80(x)  ((x == 1)? (LCDM17 |= BIT5):(LCDM17 &= ~BIT5))
  #define PB_90(x)  ((x == 1)? (LCDM19 |= BIT5):(LCDM19 &= ~BIT5))
  #define PB_100(x) ((x == 1)? (LCDM23 |= BIT5):(LCDM23 &= ~BIT5))


    
  #define PB(x) ( ((x >= 0) ? PB_10(1):PB_10(0)),\
                  ((x >= 10) ? PB_20(1):PB_20(0)),\
                  ((x >= 20) ? PB_30(1):PB_30(0)),\
                  ((x >= 30) ? PB_40(1):PB_40(0)),\
                  ((x >= 40) ? PB_50(1):PB_50(0)),\
                  ((x >= 50) ? PB_60(1):PB_60(0)),\
                  ((x >= 60) ? PB_70(1):PB_70(0)),\
                  ((x >= 70) ? PB_80(1):PB_80(0)),\
                  ((x >= 80) ? PB_90(1):PB_90(0)),\
                  ((x >= 90) ? PB_100(1):PB_100(0))\
                )

  #define LTamper(x) ((x==1)?(LCDM6 |=BIT4):(LCDM6 &=~BIT4) )

  #define UTamper(x) ((x==1)?(LCDM6 |=BIT5):(LCDM6 &=~BIT5) )


  #define MTamper(x) ((x==1)?(LCDM8 |=BIT5):(LCDM8 &=~BIT5) )


  #define NTamper(x) ((x==1)?(LCDM9 |=BIT4):(LCDM9 &=~BIT4) )





  #define EC (LCDM30 |= BIT1, LCDM31 &=  ~BIT1)
  #define GC (LCDM31 |= BIT1, LCDM30 &= ~BIT1)





  #define EN_Avarage(x)  ((x==1)?(LCDM34 |= BIT1):(LCDM34 &= ~BIT1))

  #define AM_Avarage(x)  ((x==1)?(LCDM3 |= BIT4):(LCDM3 &= ~BIT4))

  #define EN_Active(x)  ((x==1)?(LCDM39 |= BIT1):(LCDM39 &= ~BIT1))

  #define AM_Active(x)  ()//((x==1)?(LCDM4 |= BIT4):(LCDM4 &= ~BIT4))

  #define EN_Low(x)  ((x==1)?(LCDM34 |= BIT0):(LCDM34 &= ~BIT0))

  #define AM_Low(x)  ((x==1)?(LCDM4 |= BIT4):(LCDM4 &= ~BIT4))




  #define EN_Under(x)  (EN_Low(x))

  #define AM_Under(x)  (AM_Low(x))



  #define EN_Reactive(x)  (((x==1)?(LCDM35 |= BIT1):(LCDM35 &= ~BIT1)),EN_Active(x))

  #define AM_Reactive(x)  ()//((x==1)?(LCDM5 |= BIT5):(LCDM5 &= ~BIT5))


  #define EN_Balance(x)  ((x==1)?(LCDM12 |= BIT0 ):(LCDM12 &= ~BIT0))

  #define AM_Balance(x)  ((x==1)?(LCDM7 |= BIT4 ):(LCDM7 &= ~BIT4))

  #define EN_Power(x)  ((x==1)?(LCDM40 |= BIT1  ):(LCDM40 &= ~BIT1))

  #define AM_Power(x)  ((x==1)?(LCDM5 |= BIT4  ):(LCDM5 &= ~BIT4))

  #define EN_Energy(x)  ((x==1)?(LCDM40 |= BIT0):(LCDM40 &= ~BIT0))

  #define AM_Energy(x)  ((x==1)?(LCDM7 |= BIT5):(LCDM7 &= ~BIT5))


  #define EN_Error(x)  ((x==1)?(LCDM34 |= BIT1):(LCDM34 &= ~BIT1))

  #define AM_Error(x)  ((x==1)?(LCDM11 |= BIT5):(LCDM11 &= ~BIT5))


  #define EN_Interruption(x)  ((x==1)?(LCDM32 |= BIT1):(LCDM32 &= ~BIT1))

  #define AM_Interruption(x)  ((x==1)?(LCDM2 |= BIT4):(LCDM2 &= ~BIT4))




  //#define AM_CUM(x)  ((x==1)?(LCDM10 |= BIT5):(LCDM10 &= ~BIT5))


  #define EN_DEM(x)  ((x==1)?(LCDM39 |= BIT0):(LCDM39 &= ~BIT0))

  #define AM_DEM(x)  ((x==1)?(LCDM25 |= BIT5):(LCDM25 &= ~BIT5))

  #define Voltage(x) ((x==1)?(LCDM4 |= BIT0):(LCDM4 &= ~BIT0))

  #define Current(x) ((x==1)?(LCDM6 |= BIT0):(LCDM6 &= ~BIT0))

  #define Frequency(x) ((x==1)?(LCDM38 |= BIT1):(LCDM38 &= ~BIT1))

  #define PF(x) ((x==1)?(LCDM2 |= BIT0):(LCDM2 &= ~BIT0))

  #define EN_MAX(x) ((x==1)?(LCDM35 |= BIT0):(LCDM35 &= ~BIT0))

  #define AM_MAX(x) ((x==1)?(LCDM11 |= BIT4):(LCDM11 &= ~BIT4))

  #define EN_Over(x)  (EN_MAX(x))

  #define AM_Over(x)  (AM_MAX(x))


  #define EN_DAY(x) ((x==1)?(LCDM18 |= BIT1):(LCDM18 &= ~BIT1))

  #define AM_DAY(x) ((x==1)?(LCDM17 |= BIT0):(LCDM17 &= ~BIT0))

  #define EN_Birr(x) ((x==1)?(LCDM22 |= BIT0):(LCDM22 &= ~BIT0))

  #define AM_Birr(x) ((x==1)?(LCDM16 |= BIT4):(LCDM16 &= ~BIT4))

  #define EN_MIN(x) ((x==1)?(LCDM15 |= BIT4):(LCDM15 &= ~BIT4))
  #define AM_MIN(x) ((x==1)?(LCDM17 |= BIT1):(LCDM17 &= ~BIT1))

  #define Point1(x) ((x==1)?(LCDM8 |= BIT4):(LCDM8 &= ~BIT4))
  #define Point2(x) ((x==1)?(LCDM10 |= BIT4):(LCDM10 &= ~BIT4))
  #define Point3(x) ((x==1)?(LCDM12 |= BIT4):(LCDM12 &= ~BIT4))
  #define Point4(x) ((x==1)?(LCDM14 |= BIT4):(LCDM14 &= ~BIT4))

  #define UNIT_K(x) ((x==1)?(LCDM14 |= BIT0):(LCDM14 &= ~BIT0))
  #define UINT_V(x) ((x==1)?(LCDM23 |= BIT1):(LCDM23 &= ~BIT1))
  #define UNIT_WForwardSlash(x) ((x==1)?(LCDM22 |= BIT1):(LCDM22 &= ~BIT1))
  #define UNIT_WBackSlash2(x) ((x==1)?(LCDM21 |= BIT1):(LCDM21 &= ~BIT1))
  #define UNIT_ABackSlash(x) ((x==1)?(LCDM20 |= BIT0):(LCDM20 &= ~BIT0))
  #define UNIT_R(x) ((x==1)?(LCDM20 |= BIT1):(LCDM20 &= ~BIT1))
  #define UNIT_H(x) ((x==1)?(LCDM19 |= BIT1):(LCDM19 &= ~BIT1))
  #define UNIT_Z(x) ((x==1)?(LCDM19 |= BIT0):(LCDM19 &= ~BIT0))

  #define UNIT_None   ( UNIT_K(0),UINT_V(0), UNIT_WForwardSlash(0),UNIT_WBackSlash2(0),UNIT_ABackSlash(0),\
                        UNIT_R(0),UNIT_H(0),UNIT_Z(0) )
  #define UNIT_All    ( UNIT_K(1),UINT_V(1), UNIT_WForwardSlash(1),UNIT_WBackSlash2(1),UNIT_ABackSlash(1),\
                        UNIT_R(1),UNIT_H(1),UNIT_Z(1) )

  #define UINT_Amp    ( UNIT_K(0),UINT_V(0), UNIT_WForwardSlash(0),UNIT_WBackSlash2(1),UNIT_ABackSlash(1),\
                      UNIT_R(0),UNIT_H(0),UNIT_Z(0) )
  #define UINT_Volt   ( UNIT_K(0),UINT_V(1), UNIT_WForwardSlash(0),UNIT_WBackSlash2(0),UNIT_ABackSlash(0),\
                      UNIT_R(0),UNIT_H(0),UNIT_Z(0) )
  #define UINT_KV     ( UNIT_K(1),UINT_V(1), UNIT_WForwardSlash(0),UNIT_WBackSlash2(0),UNIT_ABackSlash(0),\
                      UNIT_R(0),UNIT_H(0),UNIT_Z(0) )
  #define UINT_HZ     ( UNIT_K(0),UINT_V(0), UNIT_WForwardSlash(0),UNIT_WBackSlash2(0),UNIT_ABackSlash(0),\
                      UNIT_R(0),UNIT_H(1),UNIT_Z(1) )
  #define UINT_KHZ    ( UNIT_K(1),UINT_V(0), UNIT_WForwardSlash(0),UNIT_WBackSlash2(0),UNIT_ABackSlash(0),\
                      UNIT_R(0),UNIT_H(1),UNIT_Z(1) )
  #define UNIT_W      ( UNIT_K(0),UINT_V(1), UNIT_WForwardSlash(1),UNIT_WBackSlash2(1),UNIT_ABackSlash(0),\
                      UNIT_R(0),UNIT_H(0),UNIT_Z(0) )
  #define UNIT_KW     ( UNIT_K(1),UINT_V(1), UNIT_WForwardSlash(1),UNIT_WBackSlash2(1),UNIT_ABackSlash(0),\
                      UNIT_R(0),UNIT_H(0),UNIT_Z(0) )
  #define UINT_VAr     ( UNIT_K(0),UINT_V(1), UNIT_WForwardSlash(0),UNIT_WBackSlash2(1),UNIT_ABackSlash(1),\
                      UNIT_R(1),UNIT_H(0),UNIT_Z(0) )
  #define UINT_KVA    ( UNIT_K(1),UINT_V(1), UNIT_WForwardSlash(0),UNIT_WBackSlash2(1),UNIT_ABackSlash(1),\
                      UNIT_R(0),UNIT_H(0),UNIT_Z(0) )
  #define UINT_WH    ( UNIT_K(0),UINT_V(1), UNIT_WForwardSlash(1),UNIT_WBackSlash2(1),UNIT_ABackSlash(0),\
                      UNIT_R(0),UNIT_H(1),UNIT_Z(0) )
  #define UINT_KWH    ( UNIT_K(1),UINT_V(1), UNIT_WForwardSlash(1),UNIT_WBackSlash2(1),UNIT_ABackSlash(0),\
                      UNIT_R(0),UNIT_H(1),UNIT_Z(0) )
  #define UNIT_VARH  ( UNIT_K(0),UINT_V(1), UNIT_WForwardSlash(0),UNIT_WBackSlash2(1),UNIT_ABackSlash(1),\
                      UNIT_R(1),UNIT_H(1),UNIT_Z(0) )
  #define UNIT_KVARH  (  UNIT_K(1),UINT_V(1), UNIT_WForwardSlash(0),UNIT_WBackSlash2(1),UNIT_ABackSlash(1),\
                      UNIT_R(1),UNIT_H(1),UNIT_Z(0) )

#endif
/**
*  Enables the Hardware LCD Driver     
*/
void init_lcd_c();
/**
* turns off all the LCD segments
*/
void all_segments_off();
/**
* turns on all the LCD segments
*/
void all_segments_on();

/**
* Turns off the date time display segments 
*/
void clear_date();
/**
* Tuns off the houre and minute display segments
*/
void clear_hourminute();
/**
* Turns off the main number display segments
*/
void clear_numbers();
/**
* Turns off all the unit indication display segments
*/
void clear_all_units();
/**
* Turns off all the amharic uint indicators
*/
void clear_all_amharic_indicators();
/**
* turns off all the english unit indicator segments
*/
void clear_all_english_indicatros();

/**
* turns off all non unit indicators this includes tamper, usb, rs232, energy direction
* energy level,...etc
*/
void clear_all_non_uint_indicators();



/**
* set the nmain number segments to 8(show all segments)
*/
void show_numbers();
/**
* shows all the unit segments
*/
void show_all_units();
/**
* shows all the amharic indicator segments
*/
void show_all_amharic_indicators();
/**
* shows all the english description text segments
*/
void show_all_english_indicatros();
/**
*  shows all the remaining indicators in the lcd includeing, tamper, usb,rs232,relay
*  power bar,battery, energy direction
*/
void show_all_non_uint_indicators();
/**
*  shows the hour and minute segments
*/
void show_hourminute();
/**
* shows the year,month and day segments
*/
void show_date();

#endif 
