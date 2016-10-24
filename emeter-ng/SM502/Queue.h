

#ifndef QUEUE_H
#define QUEUE_H
#include <stdint.h>

#define Max_Buff_Length 200
#define __monitor

typedef struct{
  
  uint8_t buffer[Max_Buff_Length];
  uint8_t head;
  uint8_t tail;
  
}Queue;


/**
*  resets a queue. sets the head and tail of the queue to point to the start of the buffer.
*  @param queue:  the queue to reset 
*  @return 1 
*/
__monitor int queue_reinit(Queue *queue);

/**
*  add a data to queue 
*  @param queue: the queue to add data to
*  @param data:  the data to add to the queue
*  @return 1 if the data is added sucessfully 
*         -1 if the buffer is full 
*/
__monitor int queue_enqueue(Queue *queue, const uint8_t *data);
/**
* removes an item from queue 
*  @param queue the queue to remove item from
*  @param pointer to the data removed from the queue 
*  @return -1 if the queue is empty 
*           1 if data was sucessfully returned     
*/
__monitor int queue_dequeue(Queue *queue, uint8_t *data);
/**
* @param queue the queue to check the size for
* returns the size of the queue. don't use this method, I think i have no need for it at the moment and haven't implemented it correctly. there is another tested generic queue implmentaion. 
*/
__monitor int queue_size(const Queue *queue);
/**
* checks if the queue is empty
* @return 1 if the queue is empty 
*         -1 if the queue is not empty 
*/
__monitor int queue_isEmpty(const Queue *queue);

/**
* checks if queu is full 
* @param queue  the queue to check if it is full or not 
* @return 1 if the queue is full
*        -1 if the queue is not full 
*/
__monitor int queue_isFull(const Queue *queue);
//__monitor void queue_destroy(Queue *queue);

#endif
