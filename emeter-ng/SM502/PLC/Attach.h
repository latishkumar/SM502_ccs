

#ifndef ATTACH_H
#define ATTACH_H
#include<stdint.h>


/**
* Read TI host message specification
*/

typedef struct{

  uint16_t device_Id_Length;
  uint8_t *device_identifier;
}Attach_req;


typedef struct{

  uint16_t status;
  uint16_t device_id_length;
  uint8_t *device_identifier;
  uint16_t source_address;
  uint16_t base_address;
  
}Attach_conf;


void Attach_Request(Attach_req *req);
void Attach_Confirm(Attach_conf *confirm);
void AttachTimedOut();

#endif 