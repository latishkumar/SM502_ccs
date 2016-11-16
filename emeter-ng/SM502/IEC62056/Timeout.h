
#ifndef TIMEOUT_H
#define TIMEOUT_H
#include <stdint.h>

/*
  the maximum inter character timeout is 1500 msec. 
  timer 2 interrupts every 5msec. therefore we should wait for about 
  1500/5 = 300 interrupts to abort the communication 
*/
#define InterCharacterTimeOut 1500 //msec i.e 1500msec

/*
  200msec < Tr < 1500msec 
*/
#define InterFrameTimeOutMin  200 
#define InterFrameTimeOutAvarage 850
#define InterFrameTimeOutMax  1500//300



//void InitTimer2();
//void Timer2_ON();
//void Timer2_OFF();
//__monitor void Start_Reset_TimeOut(uint8_t type);
//__monitor void Stop_TimeOut(uint8_t type);

#endif
