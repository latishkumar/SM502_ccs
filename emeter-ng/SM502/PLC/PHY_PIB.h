


#ifndef PHY_PIB_H
#define PHY_PIB_H

#include "PHY_PIB.h"
#include <stdint.h>
/**
* Read TI host message specification
*/

typedef struct{

  int status;
  int AttribuitID;
  int AttributLength;
  uint8_t *AttributeValue; // need the PRIMESpec for this values 
}GET_PHY_PID_Replay;

extern GET_PHY_PID_Replay PHY_PIB_replay;

void GET_PHY_PIB_Request(uint8_t att_id);
void GET_PHY_PIB_rep(GET_PHY_PID_Replay *Rreplay);

#endif