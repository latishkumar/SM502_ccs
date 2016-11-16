

#include "LCD_C.h"




const uint8_t LCD_CHAR[]={ 
      0xFE,  //0
      0x30,  //1
      0x6D,  //2
      0x79,  //3
      0x33,  //4
      0x5B,  //5
      0x5F,  //6
      0x70,  //7
      0xFF,  //8
      0x73,  //9
      0x00  //10
};

//#define D1(x) ( D1_A(((LCD_CHAR[x])>>6)&0x01),\
//                D1_B(((LCD_CHAR[x])>>5)&0x01),\
//                D1_C(((LCD_CHAR[x])>>4)&0x01),\
//                D1_D(((LCD_CHAR[x])>>3)&0x01),\
//                D1_E(((LCD_CHAR[x])>>2)&0x01),\
//                D1_F(((LCD_CHAR[x])>>1)&0x01),\
//                D1_G((LCD_CHAR[x])&0x01)    )

//void D1(uint8_t x){
//
////                uint8_t y=LCD_CHAR[x]>>6;
////                        
////                D1_A((y&0x01));
////                y=LCD_CHAR[x]>>5;
////                D1_B((y&0x01));
////                y=LCD_CHAR[x]>>4;
////                D1_C((y&0x01));
////                y=LCD_CHAR[x]>>3;
////                D1_D((y&0x01));
////                y=LCD_CHAR[x]>>2;
////                D1_E((y&0x01));
////                y=LCD_CHAR[x]>>1;
////                D1_F((y&0x01));
////                y=LCD_CHAR[x];
////                D1_G((y&0x01));
//  
//  
//                //uint8_t y=LCD_CHAR[x]>>6;
//                        
//                D1_A(((LCD_CHAR[x]>>6)&0x01));
//                //y=LCD_CHAR[x]>>5;
//                D1_B(((LCD_CHAR[x]>>5)&0x01));
//                //y=LCD_CHAR[x]>>4;
//                D1_C(((LCD_CHAR[x]>>4)&0x01));
//                //y=LCD_CHAR[x]>>3;
//                D1_D(((LCD_CHAR[x]>>3)&0x01));
//                //y=LCD_CHAR[x]>>2;
//                D1_E(((LCD_CHAR[x]>>2)&0x01));
//                //y=LCD_CHAR[x]>>1;
//                D1_F(((LCD_CHAR[x]>>1)&0x01));
//                //y=LCD_CHAR[x];
//                D1_G(((LCD_CHAR[x])&0x01));  
//  
//                
// 
//}



void init_lcd_c()
{


    /*
       Fframe = from 30HZ to 100HZ = 41.6666...HZ
      Flcd = 2 * mux * Fframe
           = 2 * 6 * 41.66666... = 500 HZ
  
      Flcd =     Faclk
            ------------------------
            (LCDDIVx + 1) * 2^LCDPRE
  
      500  =         32KHz 
            --------------------------
                  (31 +1) + 2 ^ 5
           ~ 500 
  
  
     Clock source ACLK
    
    */
    //divide by 32 
    LCDCCTL0 =  LCDDIV_1 | LCDPRE_5 | LCD6MUX & ~LCDSSEL;// | LCDON;//&~LCDSSEL |LCDSON

    //Internal reference for charge pump
    LCDCVCTL = LCDCPEN | VLCD_2_96;
    REFCTL0 &= ~REFMSTR;//?????????

    LCDCPCTL0 = 0xFFFF;                     
    LCDCPCTL1 = 0xFFFF;                     
    LCDCPCTL2 = 0x00FF;   
    
    LCDCCTL0 |=LCDON;
    
}

void all_segments_off()
{
   LCDCMEMCTL |= LCDCLRM; //clear all the lcd memory    
}
  
void all_segments_on()
{
    show_numbers();
    show_all_units();
    show_all_amharic_indicators();
    show_all_english_indicatros();
    show_all_non_uint_indicators();
    show_hourminute();
    show_date();
    PB(100);
    Battery(7);
    
    Tariff(1);
#ifdef LCD_REV_3    
    PLC(1);           /*[M.G] Moved this statement to this sub-block, was outside */
    Network(8);
#endif
    
}
void clear_others()
{
  RS232(0);
  USB(0);
  LTamper(0);
  UTamper(0);
  MTamper(0);
  NTamper(0);
}
void clear_date()
{
  YH(10);YL(10);
  MH(10);ML(10);
  DH(10);DL(10);
  
  MD_Bar(10);
  YM_Bar(10);
  
}
void clear_hourminute()
{
   HH(10);
   HL(10);
   Sec(10);
   MinH(10);
   MinL(10);
}

void clear_numbers()
{
  D8(10);
  D7(10);
  D6(10);
  D5(10);
  D4(10);
  D3(10);
  D2(10);
  D1(10);
  Point4(0);
  Point3(0);
  Point2(0);
  Point1(0);
}
void clear_all_units()
{
  UNIT_None;
}
void clear_all_amharic_indicators()
{
  
  AM_MAX(0);
  AM_DEM(0);
#ifndef LCD_REV_3  
  AM_CUM(0);
#endif  
  AM_Interruption(0);
  AM_Error(0);
  AM_Energy(0);
  AM_Power(0);
  AM_Balance(0);
  AM_Avarage(0);
  AM_Low(0);
 
#ifdef LCD_REV_1
  AM_Reactive(0);
  AM_Active(0); 
  AM_Under(0);
  AM_Over(0);
#endif 
  
  AM_Balance(0);
  
}
void clear_all_english_indicatros()
{
  EN_MAX(0);
  EN_DEM(0);
#ifndef LCD_REV_3  
  EN_CUM(0);
#endif  
  EN_Interruption(0);
  EN_Error(0);
  EN_Energy(0);
  EN_Power(0);
  EN_Balance(0);
  EN_Reactive(0);
  EN_Low(0);
  EN_Active(0);
  EN_Avarage(0);
  
#ifndef LCD_REV_2  
  EN_Under(0);
  EN_Over(0);
#endif   
  
  Voltage(0);
  Current(0);
  Frequency(0);
  EN_Balance(0);
  PF(0);
  
}
void clear_all_non_uint_indicators()
{
  AM_MIN(0);
  AM_Birr(0);
  AM_DAY(0);
  EN_MIN(0);
  EN_Birr(0);
  EN_DAY(0);
   
}

void show_numbers()
{
  D8(8);
  D7(8);
  D6(8);
  D5(8);
  D4(8);
  D3(8);
  D2(8);
  D1(8);
  Point4(1);
  Point3(1);
  Point2(1);
  Point1(1);
}
void show_all_units()
{
  UNIT_All;
}
void show_all_amharic_indicators()
{
  AM_MAX(1);
  AM_DEM(1);
#ifndef LCD_REV_3  
  AM_CUM(1);
#endif  
  AM_Interruption(1);
  AM_Error(1);
  AM_Energy(1);
  AM_Power(1);
  AM_Balance(1);
  AM_Low(1);
  AM_Avarage(1);  
  AM_Balance(1);  
#ifdef LCD_REV_1
  AM_Reactive(1);
  AM_Active(1);
  AM_Under(1);
  AM_Over(1);
#endif 
}
void show_all_english_indicatros()
{
  EN_MAX(1);
  EN_DEM(1);
#ifndef LCD_REV_3  
  EN_CUM(1);
#endif   
  EN_Interruption(1);
  EN_Error(1);
  EN_Energy(1);
  EN_Power(1);
  EN_Balance(1);
  EN_Reactive(1);
  EN_Low(1);
  EN_Active(1);
  EN_Avarage(1);
  
#ifndef LCD_REV_2
  EN_Under(1);
  EN_Over(1);
#endif   
  
  Voltage(1);
  Current(1);
  Frequency(1);
  EN_Balance(1);
  PF(1);
}
void show_all_non_uint_indicators()
{
  AM_MIN(1);
  AM_Birr(1);
  AM_DAY(1);
  EN_MIN(1);
  EN_Birr(1);
  EN_DAY(1);
  
  RS232(1);
  USB(1);
  LTamper(1);
  UTamper(1);
  MTamper(1);
  NTamper(1);
}

void show_date()
{
  YH(8);YL(8);
  MH(8);ML(8);
  DH(8);DL(8);
  
  MD_Bar(8);
  YM_Bar(8);
  
}
void show_hourminute()
{
   HH(8);
   HL(8);
   Sec(8);
   MinH(8);
   MinL(8);
}
