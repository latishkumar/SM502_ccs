
#include "Logger.h"
#include "FIFO.h"

FIFO_Buffer_t EventLogFifo  = {EventLogAddress_Start,0,EventLogSize,0,0,0,0,-1,0,0,0,0};
FIFO_Buffer_t EnergyLogFifo = {EnergyLogAddress_Start,0,EnergyLogSize,0,0,0,0,-1,0,0,0,0};

FIFO_Buffer_t MonthlyBillingLogFifo;//TODO. finish this one 

//uint8_t logEvent2(EventLog *l,unsigned log StartAddress);
//uint8_t getEvent2(EventLog *l,unsigned long StartAddress);
//
//uint8_t logEnergy2(EnergyLog *l,unsigned long StartAddress);
//uint8_t getEnergy2(EnergyLog *l,unsigned long StartAddress);


void init_eeprom_data_logs()
{
//    init_fifo(&EventLogFifo,EventLogSize,EventLogAddress_Start,EventLog_SIZE);
//    	EventLogFifo.InsertItem = logEvent2;
//	EventLogFifo.RemoveItem = getEvent2;
//	EventLogFifo.PeakItem   = getEvent2;
//
//    init_fifo(&EnergyLogFifo,EnergyLogSize,EnergyLogAddress_Start,EnergyLog_SIZE);
//    	EventLogFifo.InsertItem = logEnergy2;
//	EventLogFifo.RemoveItem = getEnergy2;
//	EventLogFifo.PeakItem   = getEnergy2;    
  
}