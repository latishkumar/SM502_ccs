/*
  Created on 8/13/2013
  Author Donayam 
*/

#ifndef IEC62056_H
#define IEC62056_H

#include <stdint.h>
#include "../Logger.h"
#include "Timeout.h"


/* Group C Constant deginations */
#define GENERAL_PURPOSE_OBJECTS 0
#define TOTAL_ACTIVE_POWER_PLUS 1
#define TOTAL_ACTIVE_POWER_MINUS 2

#define TOTAL_REACTIVE_POWER_PLUS 3
#define TOTAL_REACTIVE_POWER_MINUS 4

#define TOTAL_REACTIVE_POWER_QI 5
#define TOTAL_REACTIVE_POWER_QII 6
#define TOTAL_REACTIVE_POWER_QIII 7
#define TOTAL_REACTIVE_POWER_QIV 8

#define TOTAL_APPARENT_POWER_PLUS 9
#define TOTAL_APPARENT_POWER_MINUS 10

#define CURRENT 11
#define VOLTAGE 12
#define AVARAGE_POWER_FACOR 13
#define SPLLAY_FREQUENCY 14

#define TOTAL_POSITIVE_ACTIVE_POWER 15  //SUM Active power QI+QIV+QII+QIII
#define NET_POSITIVE_ACTIVE_POWER 16    //    Active power QI+QIV-QII-QIII

#define TOTAL_ACTIVE_POWER_QI 17
#define TOTAL_ACTIVE_POWER_QII 18
#define TOTAL_ACTIVE_POWER_QIII 19
#define TOTAL_ACTIVE_POWER_QIV 20


#define L1_ACTIVE_POWER_PLUS 21
#define L1_ACTIVE_POWER_MINUS 22
#define L1_REACTIVE_POWER_PLUS 23
#define L1_REACTIVE_POWER_MINUS 24

#define L1_REACTIVE_POWER_QI 25
#define L1_REACTIVE_POWER_QII 26
#define L1_REACTIVE_POWER_QIII 27
#define L1_REACTIVE_POWER_QIV 28

#define L1_APPARENT_POWER_PLUS 29
#define L1_APPARENT_POWER_MINUS 30
#define L1_CURRENT 31
#define L1_VOLTAGE 32
#define L1_POWER_FACTOR 33
#define L1_FREQUENCY 34
#define L1_POSITIVE_ACTIVE_POWER 35  //SUM Active power QI+QIV+QII+QIII
#define L1_Net_POSITIVE_ACTIVE_POWER 66    //    Active power QI+QIV-QII-QIII

#define L1_ACTIVE_POWER_QI 37
#define L1_ACTIVE_POWER_QII 38
#define L1_ACTIVE_POWER_QIII 39
#define L1_ACTIVE_POWER_QIV 40


#define ANGLE 81
#define UNITLESS_QUANTITY 82


#define NEUTRAL_CURRENT 91
#define NEUTRAL_VOLTAGE 92


#define Electricity_related_service_entries 96
#define Electricity_related_error_messages 97
#define Electricity_list 98
#define Electricity_data_profile 99


/* end of Group c definations.  According to IEC62056_61 only numbers from 0 to 99 are supported */



/*
  Value Group D definations 
*/

#define BILLING_PERIOD_AVERAGE  0
#define CUMULATIVE_MINIMUM_1 1
#define CUMULATIVE_MAXIMUM_1 2
#define MINIMUM_1 3
#define CURRENT_AVARAGE_1 4
#define LAST_AVARAGE_1  5
#define MAXIMUM_1 6
#define INSTANTANEOUS_VALUE 7
#define TIME_INTEGRAL_1 8
#define TIME_INTEGRAL_2 9
#define TIME_INTEGRAL_3 10
#define CUMULATIVE_MINIMUM_2 11
#define CUMULATIVE_MAXIMUM_2 12
#define MINIMUM_2 13
#define CURRENT_AVERAGE_2 14
#define LAST_AVERAGE_2 15
#define MAXIMUM_2 16


#define CUMULATIVE_MINIMUM_3 21
#define CUMULATIVE_MAXIMUM_3 22

#define MINIMUM_3 23
#define CURRENT_AVARAGE_3 24
#define LAST_AVARAGE_3 25
#define MAXIMUM_3 26

#define CURRENT_AVARAGE_5 27
#define CURRENT_AVARAGE_6 28
#define TIME_INTEGRAL_5 29
#define TIME_INTEGRAL_6 30


#define UNDER_LIMIT_THRESHOLD 31
#define UNDER_LIMIT_OCCURRENCE_COUNTER 32
#define UNDER_LIMIT_DURATION 33
#define UNDER_LIMIT_MAGNITUDE 34
#define OVER_LIMIT_THRESHOLD 35
#define OVER_LIMIT_OCCURRENCE_COUNTER 36
#define OVER_LIMIT_DURATION 37
#define OVER_LIMIT_MAGNITUDE 38
#define MISSING_THRESHOLD 39
#define MISSING_OCCURRENCE_COUNTER 40
#define MISSING_DURATION 41
#define MISSING_MAGNITUDE 42


#define TEST_AVERAGE 55

#define TIME_INTEGRAL_4 58


/* END OF VALUE GROUP D*/



/*VALUE GROUP E FOR CURRENT/VOLTAGE MEASUREMNT */
#define TOTAL 0
#define FIRST_HARMONIC 1
/*VALUE GROUP E FOR CURRENT/VOLTAGE MEASUREMNT */


/*value group C for Abstract Object */

#define METER_SERIAL_NUMBER 96
/*end of value group C for Abstract Object */


typedef struct{
  
 uint8_t OBIS_CODE[6];
 uint8_t datatosend[10];
 uint8_t data_length;
 
 
}IEC_Frame;



/*
Manufacturer's identification comprising three uppercase letters except as noted below
If atariff device transmits the third letter in lowercase,the m nimum reaction time tr for the
deviceis 20m sinstead of 200ms Event hough a tariff device transmits an upper case
third letter,this does not preclude supporting a 20m sreaction time
These letters shall be registered with the administrator The FLAG Association(see the
foreword)
*/
extern uint8_t Identification[16];

#define Start_Char 0x02F   // '/'
#define End_Char 0x21     //'!'
#define Tx_Request 0x3F  //'?'
#define CR 0x0D
#define LF 0x0A
#define STX 0x02
#define ETX 0x03
#define E0T 0x04
#define ACK 0x06
#define NAK 0x15
#define SOH 0x01


#define PasswordCommand 'P'
#define WriteCommand 'W'
#define ReadCommand 'R'
#define ExecuteCommand 'E'
#define ExitCommand 'B'


typedef struct{
  uint8_t isDate;
  uint8_t length;
  uint8_t Id[20];
  uint8_t id_length;
  uint8_t unit[5];
  uint8_t unit_length;
  
  uint8_t onlytime;
  uint8_t onlydate;
  
  uint8_t valid_timestamp;
  rtc_t timeStump;
  
  union{
    uint32_t longtemp;
    rtc_t Date_Time;
  }data;
  
}DataToSend;


//typedef struct 
//{
//  uint8_t CommandMessgae_Identifier;
//  uint8_t CommandType_Identifier;
//  uint8_t *data;
//  uint8_t BCC;
//  uint8_t calc_BCC;
//  uint8_t length;
//}Programming_Command_Frame;

enum MessageTypeIdentifier{
  PasswordIdentifier = 'P',
  WriteIdentifier = 'W',
  ReadIdentifier = 'R',
  ExecuteIdentifier = 'E',
  ExitIdentifier = 'B'
};

enum IEC_communicationState
{
  WaittingForRequest = 0,
  Tx_Identification = 1,
  WaitingForMode =2,
  WaittingForManufa_Specific_Instraction =4,
  WaitingForPrograming_Command = 5,
  DateExchange = 6,
  HandShake = 7
};

enum HandShakeStates{
  
  WaitingForRequest = 1,
  WaitingForAck = 2
    
};

void initIEC62056();
void IEC_Byte_Recived(uint8_t b);

void ResetCommunication();

void getFrameID();
uint8_t getID(uint8_t *ID,const uint8_t*data,uint8_t datalength);
uint8_t PasswordRecived(const uint8_t * data,uint8_t length,uint8_t CommandType_Identifier);

void Read_FrameRecived(const uint8_t *data, uint8_t length);
uint8_t Write_FrameRecived(const uint8_t *data, uint8_t length);
/*void Write_Read_FrameRecived(uint8_t *data, uint8_t length, uint8_t frameType*/
void IEC_SendIdentificationMessage();
/*
sends data message 
data : the data to send 
length : lenght of the data to send 
is programming: 1 the data message in programming mode 
                0 the data message in normal read mode 
*/
void IEC_SendDataMessage(const uint8_t *data,uint8_t length,uint8_t isprogramming);
void IEC_SendErrorMessage(uint8_t *ErrorDate,uint8_t length);
void IEC_SendAcknowledgmentMessage();
void IEC_SendRepeatRequestMessage();
void IEC_send_perdefiendReport();
void IEC_SendMessage();

void SendReplay(DataToSend *dataToSend);

void ManufacturerSpecificFrameRecived(uint8_t fram,int length);
uint8_t ToInt(const uint8_t *data, uint8_t length, uint8_t LSB_Firist);
//void Timer2_ON();
//void Timer2_OFF();
//void ResetTimer2();

#endif 