


#ifndef Relay_h
#define Relay_h

#include <msp430.h>
#include <stdint.h>

#define RelayOnPort P9OUT
#define RelayOnDir  P9DIR
#define RelayOnBIT  BIT1

#define RelayOffPort P9OUT
#define RelayOffDir  P9DIR
#define RelayOffBIT  BIT2



/** Initaializes the realy module by configuring the relay ports to output and reading the last relay status from EEPROM and 
*         changing the current relay state to that state.  
*
*/
void InitRelay();

/** reads the current system status and open or close the relay.
*   The relay has theree main states.
*       1. connected : this is the state when the relay is closed            
*       2. disconnected : this is the state where the relay is open 
*       3. ready to auto connect : this is the state where the relay is open but will auto connect if the reason for disconnecting is removed. 
*                                  the relay is moves to this when transitioning from disconnected to connected state. At this state the system will 
*                                  check if other operating condigions in the meter are fine (@see CheckConnectionIsEnabled) before connecting the meter       
*/      
void UpdateRelayStatus();

/** check the system status to see if the relay can be connected safely. this event is called in when the relay is dissconnected
*       
*/
uint8_t CheckConnectionIsEnabled(uint8_t *EventCode);

/** Closes the remote switch and logs the connect event
*   @param EventCode: the reason why the meter is connected 
*/
void ConnectMeter(uint8_t EventCode); 

/** Opens the remote switch and logs disconnect event with the reason of disconnection
*   @param EventCode: the reason why the meter is disconnected 
*/
void DisconnectMeter(uint8_t EventCode); 

/** check wheter the sytem need to open the remote switch or not based on the sytem status. 
*   @return  1 if the system need to open the remote swith
*            0 otherwise      
*/      
uint8_t IsDisconnectRequired(uint8_t *EventCode);

/** sets the outputs on the relay ports to zero so that the relay winding is deenergized. 
*   This method is called every time after a connect or disconnect command is executed.    
*/
void ResetRelayPort();
#endif