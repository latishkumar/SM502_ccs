
#include "Timeout.h"
#include "IEC62056.h"
#include "IEC62056_UART.H"




volatile uint16_t InterFrameTimeOutCounter = 0;
volatile uint16_t  InterCharacterTimeOutCounter = 0;

volatile uint8_t StartInterFrameCounter = 0;
volatile uint8_t StartInterCharCounter = 0;

extern volatile uint8_t IEC_Ready_To_Tx;

uint8_t T2_isOn = 0 ;
extern  uint8_t IEC_Tx_Done; //delete this as we go on
extern volatile uint8_t IEC_TX;

void InitTimer2()
{
          /*
          TA2CLK =  16MHZ / 16 == 250000
          1count ----------> 1/250000 sec 
          xcount <---------  5msec 
        
        x = 250000*5m count 
        x = 1250 count 
        */
        TA2CTL = TASSEL_2 | ID_3 | MC_0 | TAIE; //clock source is SMLCK ,divide by 8 ,up mode, enable interrupt 
        TA2CCR0 = 1250; //for 5msec interupt period 
        TA2EX0 = 8;//TAIDEX_8
        
}

void Timer2_ON()
{ 
   //TA2CTL &= ~MC_0;
   TA2CTL |= MC_1;
   T2_isOn = 1;
   
}

void Timer2_OFF()
{
  
  TA2CTL &= (0xFFCF); //clear bit 4 and 5
  //TA2CTL |= MC_0;
  T2_isOn = 0;
  
  StartInterCharCounter = 0;
  StartInterFrameCounter = 0;
  
}

/*
   type is the Timeout time to start 
0: IC timeout 
1: IF time out 
*/
__monitor void Start_Reset_TimeOut(uint8_t type)
{         
   if(type ==0)
   {
     InterCharacterTimeOutCounter = 0;
     //StartInterFrameCounter = 0;
     StartInterCharCounter = 1;
   }
   else if(type == 1)
   {
     InterFrameTimeOutCounter = 0; 
     StartInterFrameCounter = 1;
     //StartInterCharCounter = 0;
   }

}
__monitor void Stop_TimeOut(uint8_t type)
{
   if(type ==0)
   {
     StartInterCharCounter = 0;
   }
   else if(type == 1)
   { 
     StartInterFrameCounter = 0;
   }
}

//#pragma vector=TIMER2_A1_VECTOR
//__interrupt void TIMER2_A1_ISR(void)
//{
//   switch (__even_in_range(TA2IV, TA2IV_TA2IFG))
//    {
//       case TA2IV_TA2IFG:
//   if(StartInterFrameCounter == 1)
//   {
//          InterFrameTimeOutCounter ++;         
//            if(InterFrameTimeOutCounter >= InterFrameTimeOutMin && InterFrameTimeOutCounter <= InterFrameTimeOutMax && IEC_Ready_To_Tx == 1 )
//              {   
//                 //if(IEC_TX == 1)
//                 //{
//                   IEC_Start_SendBuffer();
//                   
//                   Stop_TimeOut(1);
//                   Start_Reset_TimeOut(0);
//                   
//                   IEC_Ready_To_Tx = 0;   
//                 //}
//              }
//            else if(InterFrameTimeOutCounter > InterFrameTimeOutMax)
//              {
//                ResetCommunication();
//                InterCharacterTimeOutCounter=0;
//                InterFrameTimeOutCounter = 0;
//                //send error message to the 
//                //some kind of internal error has occured and need a reset 
//              }
//   }
//   else if(StartInterCharCounter == 1)
//   {
//      InterCharacterTimeOutCounter++;
//      if(InterCharacterTimeOutCounter >=InterCharacterTimeOut )
//        {
//            ResetCommunication();
//            InterCharacterTimeOutCounter=0;
//            InterFrameTimeOutCounter = 0;
//        }  
//   }
//   break;
//    default:
//      break;
//    }  
//}
