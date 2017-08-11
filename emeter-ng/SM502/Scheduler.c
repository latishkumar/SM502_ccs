#include <msp430.h>
#include <Scheduler.h>

volatile funcPointers tasks[MaxScheduledTasks];
volatile uint8_t executable_Task_Index[MaxScheduledTasks];
volatile uint8_t TotalTasks = 0;

volatile uint8_t SchadulerIsOn=0;
volatile uint8_t Schaduler_counter=0;

/*
*
* Initializes time base for the scheduler
* Initializes timer 2 to run in 5 ms interval
*/
void init_scheduler()
{
        /*
          TA2CLK =  16MHZ / 8 == 2000000
          1count ----------> 1/2000000 sec 
          xcount <---------  1msec 
        
          x = 2000000*1m count 
          x = 2000 count 
        */
        TA2CTL = TASSEL_2 | ID_3 | MC_0 | TAIE; //clock source is SMLCK ,divide by 8 ,up mode, enable interrupt 
        TA2CCR0 = 2000; //for 1msec interrupt period
        TA2EX0 = 8;//TAIDEX_8
        stop_scheduler();
}


/**
* adds a task to be scheduled for execution at some time. The scheduler does not grant that
* the task will be executed exactly at the specified time, but the task will not be executed before the specified time.
* the minimum schedule time is 5msec
* @param Task: the callback to execute after the time has elapsed 
* @param time_in_ms: the amount of time in millisecond to wait before executing this task
* @param task_slot: where to put the task, use the TASKS_SLOTS definition here
* @return -1 if the task is not scheduled
*          xx: if the task is scheduled successfully, returns the task slot number
*/
__monitor int8_t schedule_task(void(*Task)(),uint32_t time_in_ms,uint8_t task_slot)
{
  
  if(task_slot < MaxScheduledTasks)
  {
    if(tasks[task_slot].time_in_ms != 0)
        cancel_task(Task);
  
       tasks[task_slot].time_in_ms = time_in_ms;
       tasks[task_slot].fp= Task;
       tasks[task_slot].counter = 0;
//       executable_Task_Index[task_slot] = 0;
       start_scheduler();
    return task_slot;
  }
  else 
    return -1;  
}

/**
*  cancels a scheduled task
*  @param Task the task to cancel 
*  @return 1 if the task was canceled successfully
*          0 if the task was not scheduled before
*/
__monitor uint8_t cancel_task( void(*Task)() )
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
* starts the scheduler
*/
__monitor void start_scheduler()
{
  if(SchadulerIsOn == 0)
  {
     TA2CTL |= MC_1;
     SchadulerIsOn = 1;
  }
}
/**
* stops the scheduler
*/
__monitor void stop_scheduler()
{
//  if((SchadulerIsOn == 1))
  {
    TA2CTL &= (0xFFCF); //clear bit 4 and 5
    SchadulerIsOn = 0;
  }
}
/**
* timer 2 interrupt service routine 
* checks if time has passed for any of the scheduled events
* and execute them
*/
uint8_t temp_SC_Counter=0;
#pragma vector=TIMER2_A1_VECTOR
__interrupt void TIMER2_A1_ISR(void)
{
  uint8_t temp=0;
  uint32_t temp2=0;
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
             stop_scheduler();
           }
       break;
    }  
}
/**
*  should be called constantly in the main loop
*  this method is introduced to run scheduled tasks asynchronously
*  with out disturbing  our main task in the system( the ADC ISR)
*/
void execute_tasks()
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



