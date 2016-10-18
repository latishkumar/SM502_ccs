
#include "Queue.h"
#include <stddef.h>
#include <stdlib.h>


/**
*  resets a queue. sets the head and tail of the queue to point to the start of the buffer.
*  @param queue:  the queue to reset 
*  @return 1 
*/
__monitor int queue_reinit(Queue *queue)
{
  queue->tail = 0;
  queue->head = 0;
  return 1;
}
/**
*  add a data to queue 
*  @param queue: the queue to add data to
*  @param data:  the data to add to the queue
*  @return 1 if the data is added sucessfully 
*         -1 if the buffer is full 
*/
__monitor int queue_enqueue(Queue *queue, const uint8_t *data)
{
  
  if(queue_isFull(queue) == 1)
  {
    return -1;
  }
  else
  {
    #pragma message("compiling 1")
      queue->tail += 1;
      if(queue->tail>=Max_Buff_Length)
         queue->tail = 0;
    
    queue->buffer[queue->tail]= *data;
    return 1;
  }
}

/**
* removes an item from queue 
*  @param queue the queue to remove item from
*  @param pointer to the data removed from the queue 
*  @return -1 if the queue is empty 
*           1 if data was sucessfully returned     
*/
__monitor int queue_dequeue(Queue *queue, uint8_t *data)
{
  if(queue_isEmpty(queue) == 1)
  {
    return -1;
  }
  else
  {
    queue->head += 1;
    if(queue->head >= Max_Buff_Length)
      queue->head = 0;
    
    *data = queue->buffer[queue->head];

    return 1;
  }
}

/**
* @param queue the queue to check the size for
* returns the size of the queue. don't use this method, I think i have no need for it at the moment and haven't implemented it correctly. there is another tested generic queue implmentaion. 
*/
__monitor int queue_size(const Queue *queue)
{
  int x = queue->head - queue->tail;
  return x>0?x:-x;
}

/**
* checks if queu is full 
* @param queue  the queue to check if it is full or not 
* @return 1 if the queue is full
*        -1 if the queue is not full 
*/
__monitor int queue_isFull(const Queue *queue)
{
   if(queue->head < queue->tail)
   {
     int x = queue->tail - queue->head;
     if(x == Max_Buff_Length-1)
       return 1;
     else 
       return -1;
   }
   else
   {
     if(queue->tail == (queue->head-1))
       return 1;
     else 
       return -1;       
   } 
}
/**
* checks if the queue is empty
* @return 1 if the queue is empty 
*         -1 if the queue is not empty 
*/
__monitor int queue_isEmpty(const Queue *queue)
{
   if(queue->head == queue->tail)
     return 1;
   return -1;
  
}
