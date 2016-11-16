


#ifndef MAC_PIB_H
#define MAC_PIB_H

#include "MAC_PIB.h"
#include <stdint.h>

/**
* Read TI host message specification
*/

enum pib_status_enum{
  PIB_NOK = 0,
  PIB_OK = 1 ,
  PIB_DISCONNECTED = 2,
  PIB_CL_CLOSE = 3

};


typedef struct{

  int status;
  int AttribuitID;
  int AttributLength;
  uint8_t *AttributeValue; // need the PRIMESpec for this values 
}GET_MAC_PID_Replay;

extern GET_MAC_PID_Replay MAC_PIB_replay;

void GET_MAC_PIB_Request(uint8_t att_id);
void GET_MAC_PIB_rep(GET_MAC_PID_Replay *Rreplay);
void KeepAlive();
void Get_Mac_PIB_TimedOut();

#endif
