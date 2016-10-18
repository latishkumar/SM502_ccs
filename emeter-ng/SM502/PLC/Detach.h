

#ifndef DETACH_H
#define DETACH_H
#include<stdlib.h>
/**
* Read TI host message specification
*/

typedef struct{
  uint16_t status;
  uint16_t dest_address;
}Detach_conf;

void Detach_Request(uint16_t dest_address);
void Detach_Confirm(Detach_conf *confirm);

#endif
