


#ifndef _DLMS_H
#define _DLMS_H

#include <stdlib.h>
#include <stdint.h>

#define FALSE 0
#define TRUE (!FALSE)
//#define NULL ((void *) 0)
//#define PREFERRED_PORT 0
#define PLC_PORT 0
#define IEC_PORT 1 


void init_dlms();
uint8_t process_dlms_frame();
uint32_t current_time(void);
#endif 

