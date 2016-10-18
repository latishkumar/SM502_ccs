


#include "Schaduler.h"
#include <msp430.h>






volatile funcPointers tasks[MaxScheduledTasks];
volatile uint8_t executable_Task_Index[MaxScheduledTasks];
volatile uint8_t TotalTasks = 0;

volatile uint8_t SchadulerIsOn=0;
volatile uint8_t Schaduler_counter=0;
/*
*
* Initializes time base for the scaduler
* Initializes timer 2 to run in 5 ms interval
*/
void Init_Scheduler()
{
        /*
          TA2CLK =  16MHZ / 8 == 2000000
          1count ----------> 1/2000000 sec 
          xcount <---------  1msec 
        
          x = 2000000*1m count 
          x = 2000 count 
        */
        TA2CTL = TASSEL_2 | ID_3 | MC_0 | TAIE; //clock source is SMLCK ,divide by 8 ,up mode, enable interrupt 
        TA2CCR0 = 2000; //for 1msec interupt period 
        TA2EX0 = 8;//TAIDEX_8
        
        stopSchaduler();
}


/**
* adds a task to be schaduled for execution at some time. The scaduler does not garrent that 
* the task will be executed exactly at the specified time, but the task will not be executed before the specified time.
* the minimum scadule time is 5msec 
* @param Task: the callback to execute after the time has elapsed 
* @param time_in_ms: the amount of time in milisecond to wait before executing this task
* @param task_slot: where to put the task, use the TASKS_SLOTS defination heare
* @return -1 if the task is not scaduled 
*          xx: if the task is schaduled succfully, returns the task slot number 
*/
__monitor uint8_t ScaduleTask(void(*Task)(),uint16_t time_in_ms,uint8_t task_slot)
{
  
  if(task_slot < MaxScheduledTasks)
  {
    if(tasks[task_slot].time_in_ms != 0)
       CancelTask2(Task);
  
       tasks[task_slot].time_in_ms = time_in_ms;
       tasks[task_slot].fp= Task;
       tasks[task_slot].counter = 0;
//       executable_Task_Index[task_slot] = 0;
       startSchaduler();
    return task_slot;
  }
  else 
    return -1;  
}

/**
*  cancels a shaduled task 
*  @param Task the task to cancel 
*  @return 1 if the task was canceled sucessfully
*          0 if the task was not scaduled before 
*/
__monitor uint8_t CancelTask2( void(*Task)() )
{
  uint8_t i=0;
  for(;i<MaxScheduledTasks;i++)
  {
    if(Task == tasks[i].fp)
    {
      tasks[i].time_in_ms = 0; 
      executable_Task_Index[i]=0;
      return 1;
    }
  }    
  return 0;
}
/**
* starts the schaduler 
*/
__monitor void startSchaduler()
{
  if(SchadulerIsOn == 0)
  {
     TA2CTL |= MC_1;
     SchadulerIsOn = 1;
  }
}
/**
* stops the schaduler 
*/
__monitor void stopSchaduler()
{
//  if((SchadulerIsOn == 1))
  {
    TA2CTL &= (0xFFCF); //clear bit 4 and 5
    SchadulerIsOn = 0;
  }
}
/**
* timer 2 interrupt service routine 
* checks if time has passed for any of the scahduled events 
* and execute them
*/
uint8_t temp_SC_Counter=0;
#pragma vector=TIMER2_A1_VECTOR
__interrupt void TIMER2_A1_ISR(void)
{
  uint8_t temp=0;
  uint16_t temp2=0;
   switch (__even_in_range(TA2IV, TA2IV_TA2IFG))
    {
       case TA2IV_TA2IFG:
         temp_SC_Counter = 0;
           for(Schaduler_counter = 0;Schaduler_counter<MaxScheduledTasks;Schaduler_counter++)
           {
             if(tasks[Schaduler_counter].time_in_ms > 0)
             {
               temp_SC_Counter++;
               temp=Schaduler_counter;
               tasks[temp].counter++;
               temp2 = tasks[temp].counter;
               if( temp2 >= tasks[temp].time_in_ms)
               {
                 executable_Task_Index[temp] = 1;                 
                 tasks[temp].time_in_ms = 0; //clear the task
               }
             }
           }
           if(temp_SC_Counter == 0)
           {
             stopSchaduler();
           }
       break;
    }  
}
/**
*  should be called constantly in the main loop
*  this method is introduced to run schadueled tasks asyncroniously 
*  with out disturbing  our main task in the system( the ADC ISR)
*/
void executeTasks()
{
  
  uint8_t z =0;
  for(;z<MaxScheduledTasks;z++)
  {
    if(executable_Task_Index[z] == 1)
    {
       tasks[z].time_in_ms = 0;
       tasks[z].counter  =0;
       executable_Task_Index[z] = 0;  
       
       tasks[z].fp(); //execute the task       
       TotalTasks--;       
    }
  }
}



