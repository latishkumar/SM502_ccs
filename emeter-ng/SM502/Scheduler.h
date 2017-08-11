



#ifndef SCHADULER_H
#define SCHADULER_H

#include <stdint.h>

#define MaxScheduledTasks 16
#define __monitor

typedef struct 
{
  void (*fp)(void);
   uint32_t time_in_ms;
   uint32_t counter;
}funcPointers;

enum TASK_SLOTS 
{
  IEC_REST_TASK = 0,
  MAC_PIB_TIMEOUT_TASK,
  ATTACH_TIMEOUT_TASK,
  KEEP_ALIVE_TASK,
  IEC_SEND_START_BUFFER_TASK,
  PROCESS_TAMPER_LC_TASK,
  PROCESS_TAMPER_UC_TASK,
  PHY_PIB_TEST_TASK,
  AUTO_DISPLAY_TASK,
  AUTO_MANUAL_DISPLAY_END_TASK,
  PROCESS_KEY_TASK,
  KEEPAD_BEEP_TASK,
  TOOGLE_BUZZER_TASK,
  SECCOND_BEEP_TASK,
  BACKUP_TASK,//ADC10_SC_TASK
  GENERAL_TASK
};
/**
* initializes time base for the scheduler
*/
void init_scheduler();
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
__monitor int8_t schedule_task(void(*Task)(),uint32_t time_in_ms,uint8_t task_slot);

/**
*  cancels a shaduled task 
*  @param Task the task to cancel 
*  @return 1 if the task was canceled sucessfully
*          0 if the task was not scaduled before 
*/
__monitor uint8_t cancel_task( void(*Task)() );

/**
* starts the schaduler 
*/
__monitor void start_scheduler();
/**
* stops the schaduler 
*/
__monitor void stop_scheduler();

/**
*  should be called constantly in the main loop
*  this method is introduced to run schadueled tasks asyncroniously 
*  with out disturbing  our main task in the system( the ADC ISR)
*/
void execute_tasks();
#endif
