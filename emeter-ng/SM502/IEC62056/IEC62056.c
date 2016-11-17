///*
// IEC62056.c
// Author: Danayam
// */
//
#include "IEC62056.h"
#include "IEC62056_UART.H"
////#include "../../TI_aes_128.h"
//#include "TI_aes.h"
//#include <msp430.h>
//#include "../Errors.h"
//#include "../Schaduler.h"
//#include "Timeout.h"
////#include "../graphics.h"
//uint16_t pos = 0;
//
////#include "../HardwareLimits.c"
//   
////#include <emeter-toolkit.h>
////#include "../../emeter-structs.h"
//   
//uint8_t Identification[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D','E', 'F' };
//   /*
// variable indicates whether the standard is waiting to recive data or 
// the protocol is waiting for the timeout to transmit data 
//0: waiting to recive data
//1: waiting to transmmit data 
//*/
//volatile uint8_t IEC_TX = 0; 
//volatile uint8_t IEC_Ready_To_Tx = 0;
//
//uint8_t IEC_State;
//uint8_t IEC_RecivedByteCounter = 0;
//uint8_t Temp_device_Address[32]; //max address is 32 (0..9)(a..z)(A...Z) and (space)
//uint8_t deviceAddressLength = 0;
//uint8_t IECtemp = 0;
//uint8_t boude_rate = '6'; //use 19200 db boude rate 
//uint8_t ACK_Boude_Rate = 0;
//uint8_t RequestedMode = 0;
//
//extern MeterAddress Meter_address;
//extern MeterAddress Meter_serialNumber;
//extern MeterAddress Meter_ProductNumber;
//
//#define ErrorIdentifierLength  5
//        
//const uint8_t ErrorIdentifier[ErrorIdentifierLength]={'E','R','O','O','R'};
//        
//DataToSend datatosend;
//DataToSend datarecived;
//
//extern rtc_t rtcc; //emeter-toolkit.h
//
//
////over voltage and current ranges and Frequency
//extern uint8_t  MAX_IRMS;
//extern uint16_t  MAX_VRMS; 
//extern uint8_t  MAX_Frequency;
//extern uint32_t MAXActivePowerTripPoint; //in 10m watt steps 
//
//
////under voltage and current ranges 
//extern uint8_t MIN_IRMS;
//extern uint8_t MIN_VRMS;
//extern uint8_t MIN_Frequency;
//
//extern uint16_t Nominal_Voltage ;//V
//extern uint8_t Basic_Current ; //A
//extern uint8_t Nominal_Frequency ;//HZ
//
//
//extern uint16_t ActivePowerTripDuration    ; //in seconds 
//extern uint16_t OverCurrentTripDuration    ;
//extern uint16_t OverVoltageTripDuration    ;
//extern uint16_t UnderVoltageTripDuration   ;
//extern uint16_t UnderFrequencyTripDuration ;
//extern uint16_t OverFrequencyTripDuration  ;
//
//
//
//extern volatile unsigned int IEC_Rx_Buffer_Index;
//extern unsigned long LastEnergyBillingCuttOffLogAddress;
////extern  phase_parms_s phase;
//
//extern Password_128 Passwords[4];
//uint8_t password_status;
//uint8_t HandShakeState;
//uint8_t DataExchangeState;
//
//
//extern volatile unsigned int IEC_Rx_Buffer_Index;
///*
//
// */
//uint8_t password_permission_level = 0;
//
//
//extern volatile uint16_t InterFrameTimeOutCounter ;
//extern volatile uint16_t  InterCharacterTimeOutCounter ;
////extern volatile uint8_t StartInterFrameCounter;
////volatile uint8_t StartInterCharCounter ;
//
//extern volatile uint8_t Error_Char_Recived ;
//
//extern uint8_t T2_isOn ;
//extern volatile unsigned int IEC_Tx_Buffer_Index ;
//
//const uint32_t multiplier[] = { 1, 10, 100, 1000, 10000, 100000,1000000,10000000,100000000,1000000000 };
//
//volatile uint8_t scaduledResetTaskNumber;
//volatile uint8_t taskScaduled = 0;
//uint8_t wait_BCC = 0;
void initIEC62056() {
  
	InitIECUART();
//	IEC_RecivedByteCounter = 0;
//        //InitTimer2();
//        IEC_State = HandShake;
//        HandShakeState = WaitingForRequest;
}
//
////void reset_Variables() {
////  
////	IEC_RecivedByteCounter = 0;
////	IECtemp = 0;
////        CurrentProgrammingFrame_rvd.BCC = 0;
////        CurrentProgrammingFrame_rvd.calc_BCC = 0;
////        CurrentProgrammingFrame_rvd.CommandMessgae_Identifier = 0;
////        CurrentProgrammingFrame_rvd.length = 0;
////        
////}
//
//uint8_t checkRequestSyntax(const uint8_t *data , uint8_t length)
//{
//  const uint8_t *datap = data;
//  //const uint8_t *datap2 = data ;
//  uint8_t end_3 = (length - 3); //sizeof(datap) * three for the last three char 2 for the first tow chars 
//  
//  uint8_t one = *datap;
//  uint8_t two = * ++datap;
//  
//  datap = data;
//  datap +=end_3;
//  
//  uint8_t end_endchar = *datap;
//  uint8_t end_cr = *++datap;
//  uint8_t end_lf = *++datap;
//  
//   
//  if(one == Start_Char && two==Tx_Request) 
//  {
//      //if((*(datap2+end_3) ==End_Char) && *(datap2++) == CR && *(datap2++) == LF)
//    if((end_endchar ==End_Char) && end_cr == CR && end_lf == LF)
//      {
//        return 1;
//      }
//  }
//  return 0;
//}
//
//uint8_t checkAckSyntax(const uint8_t *data, uint8_t length)
//{
//  const uint8_t *datap  = data;
//  const uint8_t *datap2 = data;
//  uint8_t end_2  = (length - 2); 
//  uint8_t one = *datap;
//  datap2 += end_2; 
//  uint8_t end_CR = *datap2;
//  uint8_t end_LF = *++datap2;
//  
//  if(one == ACK)
//  {
//    if( end_CR == CR && end_LF == LF)
//    {
//      return 1;
//    }
//  }
//  
//  return 0; 
//}
//
//uint8_t processHandshakeACK(const uint8_t *data,uint8_t length)
//{ 
//        uint8_t y,z,v;
//        v =  *++data;
//        z =  *++data;
//        y =  *++data;
//  
//  	if (z == boude_rate && boude_rate != '0') {
//		change_boud_rate(z);
//	}			
//        //enter to the requested mode			
//	if (y == '1' && z != boude_rate) {
//		IEC_send_perdefiendReport(); //continue with data read out mode
//                return 0;
//	}
//	else if (y == '0') // data read out
//        {
//	        //IEC_TX = 1;		
//		IEC_send_perdefiendReport(); //send IEC predefined report, data may be empty if required
//                return 0;
//        
//	} else if (y == '1' && (z == boude_rate)) {
//                //IEC_TX = 0;
//		//IEC_State = WaitingForPrograming_Command;
//                IEC_State = DateExchange;
//                  return 1;
//		
//	} else if (y >= 6 && y <= 9) {
//				//Manufacturer Specific Mode
//		DataExchangeState = WaittingForManufa_Specific_Instraction;
//                //IEC_TX = 0;
//                IEC_State = DateExchange;
//                return 1;
//	}
//        else
//        {
//            //IEC_TX = 1;
//            IEC_send_perdefiendReport();
//            return 0;
//        }
//	
//    
//}
//uint8_t RequestFrameRecived(const uint8_t *data,uint8_t length)
//{
//    //check for the device id and do something 
//     const uint8_t *id = data + (sizeof(data) << 1); //advance data by 2 bytes
//     uint8_t id_length = length - 5;
//     
//     if(id_length > Max_DeviceAddress)
//     {
//        //proocol error 
//        //don't respond time out will reset the process 
//       return 0;
//     } 
//     else//if id ==0000000000000  or id == meter address or id_length == 0
//     {
//       //process the ID
//       //check if the id matches our ID
//       //if 
//       //IEC_TX = 1;
//       IEC_SendIdentificationMessage();
//     }   
//     //else if(id ! = meter address )
//     //don't respond 
//     return 1;
//}
//
//
//void HandShakeFrameRecived(const uint8_t *data,uint8_t length)
//{ 
//     //Start_Reset_TimeOut(1);  
//     
//     if(taskScaduled == 1)
//        CancelTask2(&ResetCommunication);// CancelTask(scaduledResetTaskNumber);
//     
//     scaduledResetTaskNumber = ScaduleTask(ResetCommunication,InterFrameTimeOutMax,IEC_REST_TASK);// SchaduleTask(,);
//     taskScaduled = 1;
//     
//    if(Error_Char_Recived == 1)
//    {
//       //send NAK, be quite time out will stop the communication 
//    }
//    else {
//        switch(HandShakeState)
//        {
//        case WaitingForRequest:
//          if(checkRequestSyntax(data,length) ==1)
//           { 
//              //IEC_TX = 1;  //we are going to transmit next 
//              if(RequestFrameRecived(data,length) == 1)
//              {
//                  HandShakeState = WaitingForAck;
//                  IEC_Rx_Buffer_Index=0;
//              }
//           } 
//          else 
//           {
//             //no response, time out will reset the process 
//           }
//          break;
//        case WaitingForAck:
//          if( checkAckSyntax(data,length) == 1)
//           {
//             if(processHandshakeACK(data,length) == 1) {}
//                      //Start_Reset_TimeOut(1);//restart inter frame timeout 
//                   
//                      IEC_Rx_Buffer_Index=0;                   
//           }
//           else
//           { 
//               //no response, time out will reset the process        
//           } 
//          break;
//        default:
//              //no response , time out 
//          break;
//        }
//    }
//}
//
//void dataExchangeFrameRecived(const uint8_t *data,uint8_t length)
//{
//  
////  Stop_TimeOut(0);
////  Start_Reset_TimeOut(1);
//  
//  
//     if(taskScaduled == 1)
//       CancelTask2(&ResetCommunication);//CancelTask(scaduledResetTaskNumber);
//     
//     scaduledResetTaskNumber = ScaduleTask(ResetCommunication,InterFrameTimeOutMax,IEC_REST_TASK);//SchaduleTask(&ResetCommunication,InterFrameTimeOutMax);
//     taskScaduled = 1;
//  
//  
//  const uint8_t *datap = data;
//  uint8_t CommandIdentifier = *++datap;
//  uint8_t CommandArg = *++datap;
//  
//  datap = data;
//  uint8_t i=0;
//  uint8_t BCC = 0;
//  datap++;
//  for(;i<length-2;i++)
//  {
//    BCC ^= *datap;
//    datap++;
//  }
//  datap =  data;
//  datap+=length-1;
//  uint8_t reBCC = *datap;
//			if (reBCC == BCC) //frame id ok 
//			{
//                               datap = data;
//                               datap += 4;
//			  if (CommandIdentifier== PasswordIdentifier)
//			     {
//                               
//			       if(PasswordRecived(datap,length-6,CommandArg))							   
//                                 {
//                                    IEC_SendAcknowledgmentMessage();
//                                           return;
//                                 }
//                                 else
//                                 {
//                                           IEC_SendRepeatRequestMessage();
//                                           return;
//                                 }
//			      } else if (CommandIdentifier== WriteIdentifier) {						
//                                      if(password_permission_level == 2)  
//                                      {
//                                        if(Write_FrameRecived(datap, length-6) == 1)
//                                        {
//                                          //send ACK
//                                        }
//                                        else
//                                        {
//                                          //send error 
//                                        }
//                                          
//                                      }
//                                      else{
//                                         //send an erro frame saying no permission to write
//                                      }
//				} else if (CommandIdentifier== ReadIdentifier) {					
//                                                  if(password_permission_level == 1 || password_permission_level==2)    
//                                                  {
//                                                          Read_FrameRecived(datap, length-6); 
//                                                          
//                                                  }
//                                                 else{
//                                         //send an error frame saying no permission to read
//                                                 }					
//				}else if(CommandIdentifier==  ExecuteIdentifier){
//						
//                                
//                                }else if(CommandIdentifier==  ExitIdentifier)						
//                                {
//                                  ResetCommunication(); //end of communication, either through timeout or through 
//                                  return;
//                                }                                
//                                else
//                                {
//                                   // error unknown identifier 
//                                   // Probably send an error message 
//                                   // ??????????????????
//                                   IEC_SendRepeatRequestMessage();
//                                   return;
//                                }
//                        }
//                        else
//                        {
//                        }
// 
//                                
//}
//
//uint8_t checkDataExFrame(const uint8_t *data,uint8_t length)
//{
//  const uint8_t *datap = data;
//  const uint8_t *datap2 = data;  
//  
//  uint8_t one = *data;
//  
//  if(one == SOH)
//  {
//    uint8_t three = * (data +3);
//     if(three == STX)
//     {
//       return 1;
//     }else{
//       //this is protocol error 
//       //no response, time out will restart the process again 
//       
//     }    
//  }
//  else if(one == STX)
//  {
//    return 1;
//  } 
//  return 0;
//}
//
//
//void IEC_Byte_Recived(uint8_t byte)
//{
//    
////    if(IEC_Rx_Buffer_Index == 0)
////      Stop_TimeOut(1);
////    Start_Reset_TimeOut(0);  
////     WriteChar(byte,pos,2,font5x8);
////     //UCA1TXBUF = byte;
////     pos+=6;
//     
//     if(taskScaduled == 1)
//      CancelTask2(&ResetCommunication);//CancelTask(scaduledResetTaskNumber);
//     
//      scaduledResetTaskNumber = ScaduleTask(ResetCommunication,InterCharacterTimeOut,IEC_REST_TASK);//SchaduleTask(&ResetCommunication,InterCharacterTimeOut);
//      taskScaduled = 1;
//      
//      
////    IEC_AddToRXBuffer(byte);
//    queue_enqueue(iec_tx_buff);
//    switch(IEC_State)
//    {
//    case HandShake:
//         
////          if(T2_isOn == 1)
////          {            
////          }
////          else
////          {
////            //Start_Reset_TimeOut(0);
////            Timer2_ON();           
////          } 
//          
//           if(byte == LF || byte == NAK) //NAK may be recived during hand shake stage 
//           {
//              HandShakeFrameRecived(IEC_RX_BUF,IEC_Rx_Buffer_Index);
//              IEC_Rx_Buffer_Index = 0;
//           }
//                   
//      break;
//    case DateExchange:
//            
//            //IEC_AddToRXBuffer(byte);
//            if(byte == ETX || byte == E0T && wait_BCC == 0) //NAK may be recived during hand shake stage 
//            {
//                  wait_BCC = 1;                
//            }
//            else if(wait_BCC == 1)
//            {
//               //Start_Reset_TimeOut(1);//starts IF timeout and stops IC timeout 
//               //frame recived indication  
//               //data frame recived 
//               //reset the communication 
//               wait_BCC = 0;
//              if(checkDataExFrame(IEC_RX_BUF,IEC_Rx_Buffer_Index)  == 1)
//              {
//                 dataExchangeFrameRecived(IEC_RX_BUF,IEC_Rx_Buffer_Index);                
//                 IEC_Rx_Buffer_Index = 0;
//              }              
//              else
//              {
//                //no responce time out will restart the process 
//              }
//            }           
//      break;
//    }
//}
//
//
///*
// an encripted version of the password is stored in the EEPROM
// so when a user sends a passwrod
// encript the password with the default key's
// and compare it with the stored password
// if the password match
// set the permission level
// else
// responed as public user
//
// */
//uint8_t PasswordRecived(const uint8_t * data, uint8_t length,
//		uint8_t CommandType_Identifier) {
//
//	Password_128 temp, temp2;
//	uint8_t i = 0, j, status = 0;
//	switch (CommandType_Identifier) {
//	case '0': //data is operand for secure algorithm
//
//		if (length <= 16) { //only accepts a password of length 16
//			for (i = 0; i < 16; i++) {
//				temp.password[i] = *(data);
//				temp2.password[i] = temp.password[i]; //sotres the origional copy, b/c the buffer might be modifed at the same time.
//                                data++;
//			}
//                         j=0;
//			for (; j < MaxPasswords; j++) {
//				status = 0;
//				aes_enc_dec((unsigned char *) (temp.password+2),
//						(unsigned char*) (default_keys[j].password+2), 0);
//				for (i = 0; i < Password_Length; i++) {
//					if (temp.password[i] != Passwords[j].password[i]) {
//						status = 1;
//						break;
//					}
//					//break;
//				}
//
//				if (status == 0) //password found succesfully
//						{
//					password_permission_level = j;
//					return 1;
//				}
//
//				for (i = 0; i < Password_Length; i++) {
//					temp.password[i] = temp2.password[i]; //the password has been modified by the pervious encription , so copy it again
//				}
//
//			}
//			password_permission_level = 0; //public password
//
//		} else {
//			//password should not be >16
//		}
//
//		break;
//	case '1': //data is operand for comparison with internally held password
//		break;
//	case '2': //data is result of secure algorithm
//		break;
//	}
//        return 0;
//}
//
///*
// called when a write or read frame is recived
// *data is the data available with the frame
// *lenght is the frame data lenght
// *frameType : 0 write frame
// 1 read frame
// */
//void Read_FrameRecived(const uint8_t *data, uint8_t length) {
//	//uint8_t datatosend[50];
//	//uint8_t ID_temp[18]; //max id is 18
//	//uint8_t counter = 0;
//	uint8_t TheID[6];
//	EnergyBillingCutoffDayLog l;
//	uint8_t temp;
//
//	//uint8_t bcd[5] = { 0, 0, 0, 0, 0 };
//
//	//uint8_t temp_ID[10];
//         datatosend.id_length = 0;
//         datatosend.onlydate = 0;
//         datatosend.onlytime = 0;
//	//uint8_t temp_Id_length = 0;
//
//	//unsigned long longtemp;
//
//	//uint8_t unit[5];
//	//uint8_t unit_length;
//
//	//rtc_t temp_time;
//	//uint8_t valid_time_stamp;
//        datatosend.valid_timestamp = 0;
//        const uint8_t *datap = data;
//	while (*datap != '(' && datatosend.id_length < length) // get the id
//	{
//          datatosend.Id[datatosend.id_length] = *datap;
//          datatosend.id_length++;
//		//temp_ID[temp_Id_length] = *datap;
//		//temp_Id_length++;
//                datap++;		
//	}
//        
////        temp_ID[temp_Id_length] = '(';
////        temp_Id_length++;
//           
//	if(getID(TheID, data, length)==1)//getID(TheID, ID, counter);
//        {
//          //do noting 
//        }
//        else
//        {
//           //frame error
//           return;
//        }
//
//	switch (TheID[0]) //the switch is A
//	{
//	case 0: //Abstract object 
//            switch(TheID[1])//the switch is B
//            {
//            case '#'://not specified 
//            case 0:  //according to the standard 
//            case 1:  //non chanle specific objects can be accessed either using chanel 0 or chanel 1
//              
//
//                  switch(TheID[2])//the switch is C
//                  {
//                  case METER_SERIAL_NUMBER:
//                        //unit_length = 0;
//                        datatosend.unit_length = 0;
//                        datatosend.isDate = 0;
//                        datatosend.valid_timestamp = 0;
//                        /* F can not be 255 for IEC62056-21 protocol since only values from 0 - 99(or 0 - 127 7 bit data) are allowed */
//                         if(TheID[3] == 1 ) //D is 1 and F is 255 or don't care????????????????????????? 
//                         {
//                           /* Current grid concentrator defines only 8 id's(0-7) while while the standard defines 10id's */
//                             if(TheID[4] == 0) //Serial Number
//                             {
//                               //longtemp = Meter_serialNumber.Address;
//                               datatosend.data.longtemp = Meter_serialNumber.Address;
//                             }
//                             else if(TheID[4] ==1)
//                             {                              
//                               datatosend.data.longtemp = Meter_address.Address;  
//
//                             }
//                             else if(TheID[4] ==2)
//                             {
//                               datatosend.data.longtemp = Meter_ProductNumber.Address;
//                             }
//                         }
//                         else if(TheID[3] ==2)//parameter,changes calibration and accesses
//                         {
//                             if(TheID[4] == 0) //Number Of Configration Program Changes 
//                             {
//                               
//                             }
//                             else if(TheID[4] == 1)//Date Of last Configration Program Change
//                             {
//                             }
//                             else if(TheID[4] == 5) //Date of last Calibration
//                             {
//                             }                             
//                         }
//                         else if(TheID[3] == 7) //Number of power failers 
//                         {
//                           if(TheID[4] == 0) //toatal power failer on all phases 
//                           {
//                           }
//                           else if(TheID[4] == 1) //total power failer on phase 1
//                           {
//                           }
//                         }
//                         else
//                         {
//                           return; //send not implemented error 
//                         }
//                         
//                         SendReplay(&datatosend);
//                    break;
//                    
//                  }              
//              break;
//            }
//		break;
//	case 1: //electrical object 
//
//		switch (TheID[1]) //b 
//                {
//                case '#':
//		case 0:
//		case 1://chanel 1 
//
//                       datatosend.isDate = 0;
//                       datatosend.unit[0] = 'W';
//		       datatosend.unit[1] = 'h';
//                       datatosend.unit_length = 2;
//		       //unit_length = 2;
//		       datatosend.valid_timestamp = 1;
//		       datatosend.timeStump = getTime(&l.timeStump); //the time stump is the last energy log date
//                       
//                          if(TheID[3] == '#' || TheID[3] == 0)//starting from last billing period
//                          {
//				temp = getBillingCutoffDateLog(&l,
//						LastEnergyBillingCuttOffLogAddress - 42);
//                                if(temp == 0)
//                                {
//                                  //error 
//                                }
//
//
//				switch (TheID[2]) //switch with field C
//				{
//				case TOTAL_ACTIVE_POWER_PLUS:                                                                           
//					 datatosend.data.longtemp = (phase->consumed_active_energy_QI
//							+ phase->consumed_active_energy_QIV)
//							- (l.ActiveEnergy_QI + l.ActiveEnergy_QIV);
//
//					break;
//				case TOTAL_ACTIVE_POWER_MINUS:
//					 datatosend.data.longtemp = (phase->consumed_active_energy_QII + phase->consumed_active_energy_QIII)							
//							- (l.ActiveEnergy_QII + l.ActiveEnergy_QIII);
//					break;
//				case TOTAL_REACTIVE_POWER_PLUS:
//					 datatosend.data.longtemp = (phase->consumed_reactive_energy_QI
//							+ phase->consumed_reactive_energy_QII)
//							- (l.ReActiveEnergy_QI + l.ReActiveEnergy_QII);
//
//					break;
//				case TOTAL_REACTIVE_POWER_MINUS:
//					 datatosend.data.longtemp = (phase->reactive_energy_counter_QIII
//							+ phase->consumed_reactive_energy_QIV)
//							- (l.ReActiveEnergy_QIII + l.ReActiveEnergy_QIV);
//					break;
//				case TOTAL_REACTIVE_POWER_QI:
//					 datatosend.data.longtemp = (phase->consumed_reactive_energy_QI)
//							- (l.ReActiveEnergy_QI);
//					break;
//				case TOTAL_REACTIVE_POWER_QII:
//					 datatosend.data.longtemp = (phase->consumed_reactive_energy_QII)
//							- (l.ReActiveEnergy_QII);
//					break;
//				case TOTAL_REACTIVE_POWER_QIII:
//					 datatosend.data.longtemp = (phase->consumed_reactive_energy_QIII)
//							- (l.ReActiveEnergy_QIII);
//					break;
//				case TOTAL_REACTIVE_POWER_QIV:
//					 datatosend.data.longtemp = (phase->consumed_reactive_energy_QIV)
//							- (l.ReActiveEnergy_QIV);
//					break;
//
//				case TOTAL_APPARENT_POWER_PLUS:
//					break;
//				case TOTAL_APPARENT_POWER_MINUS:
//					break;
//
//				case TOTAL_ACTIVE_POWER_QI:
//					 datatosend.data.longtemp = (phase->consumed_active_energy_QI)
//							- (l.ActiveEnergy_QI);
//					break;
//				case TOTAL_ACTIVE_POWER_QII:
//					 datatosend.data.longtemp = (phase->consumed_active_energy_QII)
//							- (l.ActiveEnergy_QII);
//					break;
//				case TOTAL_ACTIVE_POWER_QIII:
//					 datatosend.data.longtemp = (phase->consumed_active_energy_QIII)
//							- (l.ActiveEnergy_QIII);
//					break;
//				case TOTAL_ACTIVE_POWER_QIV:
//					 datatosend.data.longtemp = (phase->consumed_active_energy_QIV)
//							- (l.ActiveEnergy_QIV);
//					break;
//                                        				
//				case TOTAL_POSITIVE_ACTIVE_POWER: //SUM Active power QI+QIV+QII+QIII
//					break;
//				case NET_POSITIVE_ACTIVE_POWER: //    Active power QI+QIV-QII-QIII
//					break;
//                                        
//				case Electricity_related_service_entries:
//					break;
//				case Electricity_related_error_messages:
//					break;
//				case Electricity_list:
//					break;
//				case Electricity_data_profile:
//					break;
//
//				}
//				
//				
//                          }
//                          else if(TheID[3] == 1)
//                          {
//                            
//                             if(TheID[4] == 0 && TheID[2] == 99) //Load profile with recording period 1
//                             {
//                               
//                             }
//                             else 
//                             {
//                               //no date 
//                             }
//                          }
//                          else if(TheID[3] == 2) //load profile with recording period 2
//                          {
//                             if(TheID[4] == 0 && TheID[2] == 99) //Load profile with recording period 1
//                             {
//                               
//                             }
//                             else 
//                             {
//                               //no date 
//                             }
//                          }
//                          
//                          else if(TheID[3] == 6)
//                          {
//                             if(TheID[2] == 0)//c ==0 
//                             {
//                                switch(TheID[4])
//                                {
//                                case 0: //nominal voltage V
//                                case 4: //Referance voltage for power quality Measurement
//			           datatosend.data.longtemp = Nominal_Voltage;
//                                   datatosend.unit[0] = 'V';
//                                   datatosend.unit_length = 1;
//                                   datatosend.valid_timestamp = 0;
//                                  break;
//                                case 1: //nominal current A
//			           datatosend.data.longtemp = Basic_Current;//?????????????????
//                                   datatosend.unit[0] = 'A';
//                                   datatosend.unit_length = 1;
//                                   datatosend.valid_timestamp = 0;
//                                  break;
//                                case 2: //Frequency Hz
//			           datatosend.data.longtemp = Nominal_Frequency;
//                                   datatosend.unit[0] = 'H';
//                                   datatosend.unit[1] = 'z';
//                                   datatosend.unit_length = 2;
//                                   datatosend.valid_timestamp = 0;
//                                  break;
//                                case 3: //Maximum Current A
//			           datatosend.data.longtemp = MAX_IRMS;
//                                   datatosend.unit[0] = 'A';
//                                   datatosend.unit_length = 1;
//                                   datatosend.valid_timestamp = 0;
//                                  break;
//                                 
//                                 
//                                }
//                             }
//                             else
//                             {
//                               //not general pupose object 
//                             }
//                          }
//                          else if(TheID[3] == 7)//instantanious value
//                          {
//                              switch (TheID[2]) //switch with field C
//				{
//                                case GENERAL_PURPOSE_OBJECTS: //
//                                    if(TheID[4] == 0) //(impulse/kwh active energy)
//                                    {
//				      datatosend.data.longtemp = PHASE_ENERGY_PULSES_PER_KW_HOUR;
//                                      datatosend.unit_length = 0;
//                                      datatosend.valid_timestamp = 0;
//                                    }
//                                    else 
//                                    {
//                                      //not supported 
//                                    }
//                                  break;
//				case TOTAL_ACTIVE_POWER_PLUS:
//                                         datatosend.valid_timestamp = 0;
//					 datatosend.data.longtemp = (phase->consumed_active_energy_QI + phase->consumed_active_energy_QIV);
//					break;
//				case TOTAL_ACTIVE_POWER_MINUS:
//                                         datatosend.valid_timestamp = 0;
//					 datatosend.data.longtemp = (phase->consumed_active_energy_QII + phase->consumed_active_energy_QIII);							
//					break;
//				case TOTAL_REACTIVE_POWER_PLUS:
//					 datatosend.valid_timestamp = 0;
//					 datatosend.data.longtemp = (phase->consumed_reactive_energy_QI
//							+ phase->consumed_reactive_energy_QII);
//					break;
//				case TOTAL_REACTIVE_POWER_MINUS:
//					 datatosend.valid_timestamp = 0;
//					 datatosend.data.longtemp = (phase->consumed_reactive_energy_QIII
//							+ phase->consumed_reactive_energy_QIV);
//					break;
//				case TOTAL_REACTIVE_POWER_QI:
//					 datatosend.valid_timestamp = 0;;
//					 datatosend.data.longtemp = (phase->consumed_reactive_energy_QI);
//							
//					break;
//				case TOTAL_REACTIVE_POWER_QII:
//					 datatosend.valid_timestamp = 0;
//					 datatosend.data.longtemp = (phase->consumed_reactive_energy_QII);							
//					break;
//				case TOTAL_REACTIVE_POWER_QIII:
//					 datatosend.valid_timestamp = 0;
//					 datatosend.data.longtemp = (phase->consumed_reactive_energy_QIII);							
//					break;
//				case TOTAL_REACTIVE_POWER_QIV:
//					 datatosend.valid_timestamp = 0;
//					 datatosend.data.longtemp = (phase->consumed_reactive_energy_QIV);							
//					break;
//
//				case TOTAL_APPARENT_POWER_PLUS:
//					break;
//				case TOTAL_APPARENT_POWER_MINUS:
//					break;
//
//				case TOTAL_ACTIVE_POWER_QI:
//					 datatosend.valid_timestamp = 0;
//					 datatosend.data.longtemp = (phase->consumed_active_energy_QI);							
//					break;
//				case TOTAL_ACTIVE_POWER_QII:
//					 datatosend.valid_timestamp = 0;
//					 datatosend.data.longtemp = (phase->consumed_active_energy_QII);							
//					break;
//				case TOTAL_ACTIVE_POWER_QIII:
//					 datatosend.valid_timestamp = 0;
//					 datatosend.data.longtemp = (phase->consumed_active_energy_QIII);							
//					break;
//				case TOTAL_ACTIVE_POWER_QIV:
//					 datatosend.valid_timestamp = 0;
//					 datatosend.data.longtemp = (phase->consumed_active_energy_QIV);							
//					break;
//                                        
//                                        
//				case CURRENT:
//                                case L1_CURRENT:
//                                        datatosend.valid_timestamp = 0;
//					datatosend.data.longtemp = phase->readings.I_rms;
//                                        datatosend.unit[0] = 'm';
//                                        datatosend.unit[1] = 'A';
//                                        datatosend.unit_length = 2;
//					break;
//				case VOLTAGE:
//                                case L1_VOLTAGE:
//                                        datatosend.valid_timestamp = 0;
//					datatosend.data.longtemp = phase->readings.V_rms/100;
//                                        datatosend.unit[0] = 'V';
//                                        datatosend.unit_length = 1;
//					break;
//				case AVARAGE_POWER_FACOR:
//                                case L1_POWER_FACTOR:
//                                        datatosend.valid_timestamp = 0;
//					 datatosend.data.longtemp = phase->readings.power_factor/100;
//                                        datatosend.unit[0] ='%';
//                                        datatosend.unit_length = 1;
//					break;
//				case SPLLAY_FREQUENCY:
//                                case L1_FREQUENCY:
//                                       datatosend.valid_timestamp = 0;
//					 datatosend.data.longtemp = phase->readings.frequency/100;
//                                        datatosend.unit[0]= 'H';
//                                        datatosend.unit[1] = 'z';   
//                                        datatosend.unit_length = 2;
//					break;
//
//				case TOTAL_POSITIVE_ACTIVE_POWER: //SUM Active power QI+QIV+QII+QIII
//                                        datatosend.valid_timestamp = 0;
//					 datatosend.data.longtemp = phase->consumed_active_energy_QI + phase->consumed_active_energy_QII+
//                                                phase->consumed_active_energy_QIII + phase->consumed_active_energy_QIV;
//					break;
//				case NET_POSITIVE_ACTIVE_POWER: //    Active power QI+QIV-QII-QIII
//                                        datatosend.valid_timestamp = 0;
//					 datatosend.data.longtemp = phase->consumed_active_energy_QI + phase->consumed_active_energy_QII-
//                                                phase->consumed_active_energy_QIII - phase->consumed_active_energy_QIV;
//					break;
//
//				case L1_ACTIVE_POWER_PLUS:
//                                     datatosend.valid_timestamp = 0;
//					 datatosend.data.longtemp = phase->readings.active_power>0?phase->readings.active_power/10:0;
//                                              datatosend.unit[0] = 'm';
//                                              datatosend.unit[1] = 'W';                                              
//                                              datatosend.unit_length = 2;                                                                                            
//					break;
//				case L1_ACTIVE_POWER_MINUS:
//                                     datatosend.valid_timestamp = 0;
//					 datatosend.data.longtemp = phase->readings.active_power<0?phase->readings.active_power/10:0;
//                                              datatosend.unit[0] = 'm';
//                                              datatosend.unit[1] = 'W';
//                                              datatosend.unit_length = 2;                                            
//					break;
//				case L1_REACTIVE_POWER_PLUS:
//                                  datatosend.valid_timestamp = 0;
//					 datatosend.data.longtemp = phase->readings.reactive_power >0?phase->readings.reactive_power/10:0;
//                                              datatosend.unit[0] = 'm';
//                                              datatosend.unit[1] = 'W';
//                                              datatosend.unit_length = 2;
//					break;
//				case L1_REACTIVE_POWER_MINUS:
//                                  datatosend.valid_timestamp = 0;
//					 datatosend.data.longtemp = phase->readings.reactive_power <0?phase->readings.reactive_power/10:0;
//                                              datatosend.unit[0] = 'm';
//                                              datatosend.unit[1] = 'W';
//                                              datatosend.unit_length = 2;                                 
//					break;
//
//				case L1_REACTIVE_POWER_QI:
//                                  datatosend.valid_timestamp = 0;
//					 datatosend.data.longtemp = (phase->readings.active_power > 0 && phase->readings.reactive_power >0)?phase->readings.reactive_power:0;
//					break;
//				case L1_REACTIVE_POWER_QII:
//                                  datatosend.valid_timestamp = 0;
//					 datatosend.data.longtemp = (phase->readings.active_power < 0 && phase->readings.reactive_power >0)?phase->readings.reactive_power:0;
//					break;
//				case L1_REACTIVE_POWER_QIII:
//                                  datatosend.isDate = 0;
//					 datatosend.data.longtemp = (phase->readings.active_power < 0 && phase->readings.reactive_power <0)?phase->readings.reactive_power:0;
//					break;
//				case L1_REACTIVE_POWER_QIV:
//                                  datatosend.valid_timestamp = 0;
//					 datatosend.data.longtemp = (phase->readings.active_power > 0 && phase->readings.reactive_power < 0)?phase->readings.reactive_power:0;
//					break;
//
//				case L1_APPARENT_POWER_PLUS:
//					break;
//				case L1_APPARENT_POWER_MINUS:
//					break;
//				                                 
//				case L1_POSITIVE_ACTIVE_POWER: //the instantanious power is only one 
//                                case L1_Net_POSITIVE_ACTIVE_POWER:
//                                       datatosend.valid_timestamp = 0;
//					 datatosend.data.longtemp = phase->readings.active_power;
//					break;
//
//				case L1_ACTIVE_POWER_QI:
//                                         datatosend.valid_timestamp = 0;
//					 datatosend.data.longtemp = (phase->readings.active_power > 0 && phase->readings.reactive_power > 0)?phase->readings.active_power:0;
//					break;
//				case L1_ACTIVE_POWER_QII:
//                                         datatosend.valid_timestamp = 0;
//					 datatosend.data.longtemp = (phase->readings.active_power < 0 && phase->readings.reactive_power > 0)?phase->readings.active_power:0;
//					break;
//				case L1_ACTIVE_POWER_QIII:
//                                         datatosend.valid_timestamp = 0;
//					 datatosend.data.longtemp = (phase->readings.active_power < 0 && phase->readings.reactive_power < 0)?phase->readings.active_power:0;
//					break;
//				case L1_ACTIVE_POWER_QIV:
//                                         datatosend.valid_timestamp = 0;
//					 datatosend.data.longtemp = (phase->readings.active_power > 0 && phase->readings.reactive_power < 0)?phase->readings.active_power:0;
//					break;
//
//				case ANGLE:
//					break;
//				case UNITLESS_QUANTITY:
//					break;
//
//				case NEUTRAL_CURRENT:
//                                     
//					break;
//				case NEUTRAL_VOLTAGE:
//					break;
//
//				case Electricity_related_service_entries:
//					break;
//				case Electricity_related_error_messages:
//					break;
//				case Electricity_list:
//					break;
//				case Electricity_data_profile:
//					break;
//
//				}
//                          }
//                          else if(TheID[3] == 8)//time intergral I
//                          {
//                          }
//                          else if(TheID[3] == 9)
//                          {
//                            if(TheID[2] == 0) //general pupose objects 
//                            {
//                              if(TheID[4] == 0) //time expired since last end of billing period 
//                              {
//                              }
//                              else if(TheID[4] ==1) //local time 
//                              {
//                                datatosend.data.Date_Time = rtcc;
//                                datatosend.onlytime = 1;
//                                datatosend.isDate = 1;
//                                datatosend.unit_length = 0;
//                                datatosend.valid_timestamp = 0;
//                                
//                              }
//                              else if(TheID[4] == 2) //local date
//                              {
//                                datatosend.data.Date_Time = rtcc;
//                                datatosend.onlydate= 1;
//                                datatosend.isDate = 1;
//                                datatosend.unit_length = 0;
//                                datatosend.valid_timestamp = 0;
//                              }
//                              else if(TheID[4] == 5) //Week day
//                              {                                 
//                                 datatosend.data.longtemp = RTCDOW;
//                                 datatosend.isDate = 0;
//                                 datatosend.unit_length = 0;
//                                 datatosend.valid_timestamp = 0;
//                              }
//                              else if(TheID[4] == 6) //time of last reset
//                              {
//                              }
//                              else if(TheID[4] == 7) //date of last reset 
//                              {
//                              }
//                              else if(TheID[4] == 8) //output pulse duration
//                              {
//                                 datatosend.isDate = 0;
//			         datatosend.data.longtemp = (ENERGY_PULSE_DURATION * 125)/512;//interms of adc ISR
//                                 datatosend.unit[0] = 'm';
//                                 datatosend.unit[1]= 's';
//                                 datatosend.unit[2] = 'e';
//                                 datatosend.unit[3] = 'c';
//                                 datatosend.unit_length = 4;
//                                 datatosend.valid_timestamp = 0;
//                              }
//                            }
//                            else
//                            {
//                              //implement next
//                            }
//                          }
//                          else if(TheID[3] == 10 && TheID[2] == 99) //c =99
//                          {
//                            if(TheID[4] == 1 ) //Dips voltage profile
//                            {
//                            }
//                            else if(TheID[4] ==2) //swells voltage profile 
//                            {
//                            }
//                            else if(TheID[4] == 3) //cuts voltage profile 
//                            {
//                            }
//                          }
//                          else if(TheID[3] == 31)//under limit thrushold
//                          {
//                                switch (TheID[2]) //switch with field C
//				{ 
//				case VOLTAGE:
//                                case L1_VOLTAGE:
//					datatosend.data.longtemp = MIN_VRMS;
//                                        datatosend.unit[0] = 'V';
//                                        datatosend.unit_length = 1;
//                                        datatosend.valid_timestamp = 0;
//					break;
//				case AVARAGE_POWER_FACOR:
//                                case L1_POWER_FACTOR:
//					datatosend.data.longtemp = phase->readings.power_factor/100;
//                                        datatosend.unit[0] ='%';
//                                        datatosend.unit_length =1;
//                                        datatosend.valid_timestamp = 0;
//					break;
//				case SPLLAY_FREQUENCY:
//                                case L1_FREQUENCY:
//					datatosend.data.longtemp = MIN_Frequency;
//                                        datatosend.unit[0]= 'H';
//                                        datatosend.unit[1] = 'z';     
//                                        datatosend.unit_length = 2;
//                                        datatosend.valid_timestamp = 0;
//					break;
//				case NEUTRAL_VOLTAGE:
//					datatosend.data.longtemp = MIN_VRMS;
//                                        datatosend.unit[0] = 'V';
//                                        datatosend.unit_length =1;
//                                        datatosend.valid_timestamp = 0;
//					break;
//				}
//                          }
//                          else if(TheID[3] == 32)//under limit occurance counter
//                          {
//                          }
//			  else if(TheID[3] == 35)//over limit thrushold
//                          {
//                              switch(TheID[2])
//                              {
//				case TOTAL_ACTIVE_POWER_PLUS:
//				case TOTAL_ACTIVE_POWER_MINUS:
//				case TOTAL_REACTIVE_POWER_PLUS:
//				case TOTAL_REACTIVE_POWER_MINUS:
//				case TOTAL_REACTIVE_POWER_QI:
//				case TOTAL_REACTIVE_POWER_QII:
//				case TOTAL_REACTIVE_POWER_QIII:
//				case TOTAL_REACTIVE_POWER_QIV:
//				case TOTAL_APPARENT_POWER_PLUS:
//				case TOTAL_APPARENT_POWER_MINUS:
//				case TOTAL_ACTIVE_POWER_QI:
//				case TOTAL_ACTIVE_POWER_QII:
//				case TOTAL_ACTIVE_POWER_QIII:
//				case TOTAL_ACTIVE_POWER_QIV:
//                                case TOTAL_POSITIVE_ACTIVE_POWER: //SUM Active power QI+QIV+QII+QIII
//				case NET_POSITIVE_ACTIVE_POWER: //    Active power QI+QIV-QII-QIII
//				case L1_ACTIVE_POWER_PLUS:
//				case L1_ACTIVE_POWER_MINUS:
//				case L1_REACTIVE_POWER_PLUS:
//				case L1_REACTIVE_POWER_MINUS:
//				case L1_REACTIVE_POWER_QI:
//				case L1_REACTIVE_POWER_QII:
//				case L1_REACTIVE_POWER_QIII:
//				case L1_REACTIVE_POWER_QIV:
//				case L1_APPARENT_POWER_PLUS:
//				case L1_APPARENT_POWER_MINUS:				                                 
//				case L1_POSITIVE_ACTIVE_POWER: //the instantanious power is only one 
//                                case L1_Net_POSITIVE_ACTIVE_POWER:
//				case L1_ACTIVE_POWER_QI:
//				case L1_ACTIVE_POWER_QII:
//				case L1_ACTIVE_POWER_QIII:
//				case L1_ACTIVE_POWER_QIV:
//			          datatosend.data.longtemp =  MAXActivePowerTripPoint;
//                                  datatosend.unit[0] = 'W';
//                                  datatosend.unit_length = 1;
//                                  datatosend.valid_timestamp = 0;
//                                  break;
//				case CURRENT:
//                                case L1_CURRENT:
//                                case NEUTRAL_CURRENT:
//                                        datatosend.valid_timestamp = 0;
//					datatosend.data.longtemp = MAX_IRMS;
//                                        datatosend.unit[0] = 'A';
//                                        datatosend.unit_length = 1;                                        
//					break;
//				case VOLTAGE:
//                                case L1_VOLTAGE:
//                                case NEUTRAL_VOLTAGE:
//                                        datatosend.valid_timestamp = 0;
//					datatosend.data.longtemp = MAX_VRMS;
//                                        datatosend.unit[0] = 'V';
//                                        datatosend.unit_length = 1;
//					break;
//				case AVARAGE_POWER_FACOR:
//                                case L1_POWER_FACTOR:
//                                        datatosend.valid_timestamp = 0;
//					datatosend.data.longtemp = phase->readings.power_factor/100;
//                                        datatosend.unit[0] ='%';
//                                        datatosend.unit_length = 1;
//					break;
//				case SPLLAY_FREQUENCY:
//                                case L1_FREQUENCY:
//                                        datatosend.valid_timestamp = 0;
//					datatosend.data.longtemp = MAX_Frequency;
//                                        datatosend.unit[0]= 'H';
//                                        datatosend.unit[1] = 'z';       
//                                        datatosend.unit_length = 2;
//					break;
//				}                           
//                          }
//                          else if(TheID[3] == 36)//over limit occurance counter
//                          {
//                          }
//			  else if(TheID[3] == 39)//missing thrushold
//                          {
//                          }
//                          else if(TheID[3] == 40)//missing occurance counter
//                          {
//                          }			
//                          else if(TheID[3] == 98  && TheID[2] == 99)//Event log
//                          {
//                             
//                          }
//                     SendReplay(&datatosend);
//                    break;
//		}
//		break;
//	default:
//		//no other chanel is availabel in our meter
//		break;
//	}
//}
//
//unsigned long getLong(const uint8_t *data,uint8_t length,uint8_t LSB_Firist)
//{
//      
//  	uint32_t output = 0;
//	uint8_t t = 0, position = 0;
//	if (LSB_Firist == 1) {
//		position = 0;
//	} else {
//		position = length - 1;
//	}
//
//	for (; t < length; t++) {
//		output += (*data - 48) * (multiplier[position]);
//		if (LSB_Firist == 1) {
//			position += 1;
//		} else {
//			position -= 1;
//		}
//		data++;
//	}
//	return output;
//}
//
//rtc_t getDateTime(const uint8_t *data,uint8_t length,uint8_t getDate,uint8_t getTime)
//{
//  rtc_t temp;
//  const uint8_t *temp_ptr = data;
//  if(getDate == 1)
//  {
//    temp.year =  ToInt(data,2, 0);
//    data +=3;
//    temp.month = ToInt(data,2,0);
//    data +=3;
//    temp.day =  ToInt(data,2,0);
//  }
//  if(getTime == 1)
//  {
//     if(getDate == 1) // the data contains both is date time 
//     {
//       data+=3;
//
//     }
//     
//     temp.hour =  ToInt(data,2, 0);
//     data+=3;
//     temp.minute = ToInt(data,2,0);
//     temp.second = 0;
//  }
//  
//  return temp;
//  
//}
//
//
//int8_t getMultiplier(const uint8_t *unit,uint8_t length) //test this method 
//{
//  
//   if(length>0)
//   {
//     if(*unit =='k' || *unit == 'K')
//       return 3;
//     else if(*unit == 'm')
//       return -3;  
//     else if(*unit == 'M')
//       return 6;
//   }
//   
//   return 1;   
//}
//
//uint8_t Write_FrameRecived(const uint8_t *data, uint8_t length)
//{
//        uint8_t write_succesfull=0;
//        uint8_t eeprom_error;
//        uint8_t z;
//	uint8_t TheID[6];
//	//EnergyBillingCutoffDayLog l;
//	uint8_t temp=0;
//        datarecived.id_length = 0;
//        const uint8_t *datap = data;
//        
//	while (*datap != '(' && datarecived.id_length < length) // get the id
//	{
//          //datarecived.Id[datarecived.id_length] = *datap;
//          datarecived.id_length++;
//		//temp_ID[temp_Id_length] = *datap;
//		//temp_Id_length++;
//                datap++;	
//                temp++;
//	}
//        
//        if(datarecived.id_length == length)
//        {
//          //error, the data contains only the OBIS code and no data to write
//          //frame error 
//          return 0;
//        }
//        
////        uint8_t temp_value8;
//        uint16_t temp_value16;
////        uint32_t temp_value32;
//          
//        //uint8_t unit[5]; 
//        uint8_t unit_length =0;
//       
//       
//        
//        const uint8_t *val = (++datap);
//        uint8_t data_length_counter = datarecived.id_length+1;
//        
//        
//        while(*val != '*' && *val !='&' &&  *val !=')' && data_length_counter < length)
//        {
//          val++;
//          data_length_counter++;
//        }
//        
//        uint8_t data_length =  data_length_counter - datarecived.id_length-1;
//        const uint8_t *data_start =  datap;
//        
//        const uint8_t *unit_start = (++val);
//        if(data_length_counter+1 >= length)
//        {
//          //no uint
//          unit_length = 0;
//        }
//        else
//        {
//          
//          const uint8_t  *unit_start_temp = unit_start;
//           while( *unit_start_temp != ')' )
//           {
//             unit_length++;
//             unit_start_temp++;
//           }
//           
//        }
//        
//       
//                
//        uint32_t data_value = getLong(data_start,data_length,0);
//        
//        int8_t multiplier_index  =  1;       
//        multiplier_index = getMultiplier(unit_start,unit_length);
//        
//        if(multiplier_index != 1)
//        {
//         if(multiplier_index < 0)
//         {
//           data_value = data_value/multiplier[(-1*multiplier_index)];
//         }
//         else {
//           data_value = data_value*multiplier[multiplier_index];
//         }
//        }
//         
//        
////        temp_ID[temp_Id_length] = '(';
////        temp_Id_length++;
//           
//	if(getID(TheID, data, length)==1)//getID(TheID, ID, counter);
//        {
//          //do noting 
//        }
//        else
//        {
//           //frame error
//           return 0;
//        }
//
//	switch (TheID[0]) //the switch is A
//	{
//	case 0: //Abstract object 
//            switch(TheID[1])//the switch is B
//            {
//            case '#'://not specified 
//            case 0:  //according to the standard 
//            case 1:  //non chanle specific objects can be accessed either using chanel 0 or chanel 1
//              
//              //unit_length = 0;             
//              
//                  switch(TheID[2])//the switch is C
//                  {
//                  case METER_SERIAL_NUMBER:
//                        /* F can not be 255 for IEC62056-21 protocol since only values from 0 - 99(or 0 - 127 7 bit data) are allowed */
//                         if(TheID[3] == 1 ) //D is 1 and F is 255 or don't care????????????????????????? 
//                         {
//                           /* Current grid concentrator defines only 8 id's(0-7) while while the standard defines 10id's */
//                             if(TheID[4] == 0) //Serial Number
//                             {
//                               Meter_serialNumber.Address = data_value;
//                               //calculate CRC
//                               z = setMeterAddress(&Meter_serialNumber, 2);
//                               if(z==1)
//                                  write_succesfull = 1;
//                               else
//                               {
//                                 //eeprom error 
//                                 eeprom_error = 1;
//                               }
//                             }
//                             else if(TheID[4] ==1)
//                             {                              
//                               Meter_address.Address = data_value;
//                               //calculate CRC 
//                               z = setMeterAddress(&Meter_address, 0);
//                               if(z==1)
//                                  write_succesfull = 1;
//                               else
//                               {
//                                 //eeprom error 
//                                 eeprom_error = 1;
//                               }
//                               //datatosend.data.longtemp = Meter_address.Address;  
//                               //datatosend.isDate = 0;
//                             }
//                             else if(TheID[4] ==2)
//                             {
//                               //datatosend.data.longtemp = Meter_ProductNumber.Address;
//                               Meter_ProductNumber.Address = data_value;
//                               //calculate CRC 
//                               z = setMeterAddress(&Meter_ProductNumber, 1);
//                               if(z==1)
//                                  write_succesfull = 1;
//                               else
//                               {
//                                 //eeprom error 
//                                 eeprom_error = 1;
//                               }
//                               //datatosend.isDate = 0;
//                             }
//                         }
//                         else if(TheID[3] ==2)//parameter,changes calibration and accesses
//                         {
//                             if(TheID[4] == 0) //Number Of Configration Program Changes 
//                             {
//                               
//                             }
//                             else if(TheID[4] == 1)//Date Of last Configration Program Change
//                             {
//                             }
//                             else if(TheID[4] == 5) //Date of last Calibration
//                             {
//                             }
//                             
//                         }
//                         else if(TheID[3] == 7) //Number of power failers 
//                         {
//                           if(TheID[5] == 0) //toatal power failer on all phases 
//                           {
//                           }
//                           else if(TheID[5] == 1) //total power failer on phase 1
//                           {
//                           }
//                         }
//                    break;
//                    
//                  }              
//              break;
//            }
//	   break;
//	case 1: //electrical object 
//
//		switch (TheID[1]) //b 
//                {
//                case '#':
//		case 0:
//		case 1:              //chanel 1 
//                          if(TheID[3] == 1)
//                          {                            
//                             if(TheID[4] == 0 && TheID[2] == 99) //Load profile with recording period 1
//                             {
//                               
//                             }
//                             else 
//                             {
//                               //no date 
//                             }
//                          }
//                          else if(TheID[3] == 2) //load profile with recording period 2
//                          {
//                             if(TheID[4] == 0 && TheID[2] == 99) //Load profile with recording period 1
//                             {
//                               
//                             }
//                             else 
//                             {
//                               //no date 
//                             }
//                          }
//                          
//                          else if(TheID[3] == 6)
//                          {
//                             if(TheID[2] == 0)//c ==0 
//                             {
//                                switch(TheID[4])
//                                {
//                                case 0: //nominal voltage V
//                                case 4: //Referance voltage for power quality Measurement 
//                                  //datatosend.data.longtemp = Meter_ProductNumber.Address;
//                                  
//                                  Nominal_Voltage = (uint8_t)(data_value); 
//                                  temp_value16 =  (uint16_t)(data_value);
//                                  //calculate CRC 
//                                  z = setNominalValues(&temp_value16, 0);//nominal voltage
//                                  if(z==1)
//                                  write_succesfull = 1;
//                                  else
//                                  {
//                                     //eeprom error 
//                                      eeprom_error = 1; //IEC send NAK
//                                  }                                   
//                                  break;
//                                case 1: //basic/nominal current A    
//                                     Basic_Current = (uint8_t)(data_value); 
//                                     temp_value16 =  (uint16_t)(data_value);
//                                       //calculate CRC 
//                                     z = setNominalValues(&temp_value16, 1);//nominal current
//                                     if(z==1)
//                                       write_succesfull = 1;
//                                     else
//                                     {
//                                       //eeprom error 
//                                      eeprom_error = 1; //IEC send NAK
//                                     }     
//                                  break;
//                                case 2: //Frequency Hz
//                                     Nominal_Frequency = (uint8_t)(data_value); 
//                                     temp_value16 =  (uint16_t)(data_value);
//                                       //calculate CRC 
//                                     z = setNominalValues(&temp_value16, 2);//nominal voltage
//                                     if(z==1)
//                                       write_succesfull = 1;
//                                     else
//                                     {
//                                       //eeprom error 
//                                      eeprom_error = 1; //IEC send NAK
//                                     }     
//                                  break;
//                                case 3: //Maximum Current A
//                                  
//                                   MAX_IRMS = (uint8_t)(data_value);
//                                   uint16_t irms = (uint16_t)MAX_IRMS;
//                                   z= setMaxValues(&irms,1);//set current;
//                                   
//                                   if(z==1)
//                                      write_succesfull = 1;
//                                   else
//                                   {
//                                     //eeprom error 
//                                      eeprom_error = 1; //IEC send NAK
//                                   }                                      
//                                  break;
//                                }
//                             }
//                             else
//                             {
//                               //not general pupose object 
//                             }
//                          }
//                          else if(TheID[3] == 7)//instantanious value
//                          {
//                              switch (TheID[2]) //switch with field C
//			       {                               
//				case TOTAL_REACTIVE_POWER_QI:
//                                      phase->consumed_reactive_energy_QI = data_value;						
//                                      write_succesfull = 1;
//					break;
//				case TOTAL_REACTIVE_POWER_QII:					
//                                        phase->consumed_reactive_energy_QII = data_value;
//                                       write_succesfull = 1;             
//					break;
//				case TOTAL_REACTIVE_POWER_QIII:
//                                        phase->consumed_reactive_energy_QIII = data_value;
//                                       write_succesfull = 1;       
//					break;
//				case TOTAL_REACTIVE_POWER_QIV:
//					phase->consumed_reactive_energy_QIV = data_value;	
//                                       write_succesfull = 1;					
//					break;
//				case TOTAL_ACTIVE_POWER_QI:
//					 phase->consumed_active_energy_QI = data_value;
//                                       write_succesfull = 1;       
//					break;
//				case TOTAL_ACTIVE_POWER_QII:
//					 phase->consumed_active_energy_QII = data_value;
//                                       write_succesfull = 1;       
//					break;
//				case TOTAL_ACTIVE_POWER_QIII:
//					phase->consumed_active_energy_QIII=data_value;
//                                       write_succesfull = 1;       
//					break;
//				case TOTAL_ACTIVE_POWER_QIV:
//                                       write_succesfull = 1; 
//					phase->consumed_active_energy_QIV = data_value;							
//					break;
//                                        
//				case UNITLESS_QUANTITY:
//					break;
//
//				case Electricity_related_service_entries:
//					break;
//				case Electricity_related_error_messages:
//					break;
//				case Electricity_list:
//					break;
//				case Electricity_data_profile:
//					break;
//
//				}
//                          }
//                          else if(TheID[3] == 8)//time intergral I
//                          {
//                          }
//                          else if(TheID[3] == 9)
//                          {
//                            if(TheID[2] == 0) //general pupose objects 
//                            {
//                              if(TheID[4] == 0) //time expired since last end of billing period 
//                              {
//                              }
//                              else if(TheID[4] ==1) //local time 
//                              {
//                                
//                                rtc_t temp_time44 = getDateTime(data_start,data_length,0,1);
//                                                                                           
//                                rtcc.second = 0;
//                                rtcc.hour = temp_time44.hour;
//                                rtcc.minute = temp_time44.minute;
//                                set_rtc_sumcheck();
//                                
//                                RTCCTL0_H = RTCKEY_H;                  // Unlock RTC
//
//                                RTCCTL13 = RTCBCD+RTCHOLD+RTCMODE+RTCTEV_0; // Init RTC
//                                RTCCTL0_L &= ~RTCOFIFG_L;   // Clear Flag
//                                RTCSEC   = rtcc.second;
//                                RTCMIN   = rtcc.minute;
//                                RTCHOUR  = rtcc.hour;//0x5;
//                                //RTCDOW   = rtcc.;//0x3;
//                                RTCDAY   = rtcc.day;//0x8;
//                                RTCMON   = rtcc.month;//0x07;
//                                RTCYEAR  = rtcc.year;//0x2013;                    
//                                
//                                RTCCTL13 &= ~RTCHOLD;                   // Enable RTC
//                                RTCCTL0_H = 0;   // LOCK RTC
//                                
//                                 write_succesfull = 1;
//                                
//                              }
//                              else if(TheID[4] == 2) //local date
//                              {
//                                
//                                rtc_t temp_time44 = getDateTime(data_start,data_length,1,0);
//                                rtcc.year = temp_time44.year;
//                                rtcc.month= temp_time44.month;
//                                rtcc.day= temp_time44.day;
//                                
//                                set_rtc_sumcheck();
//                                
//                                RTCCTL0_H = RTCKEY_H;                  // Unlock RTC
//
//                                RTCCTL13 = RTCBCD+RTCHOLD+RTCMODE+RTCTEV_0; // Init RTC
//                                RTCCTL0_L &= ~RTCOFIFG_L;   // Clear Flag
//                                RTCSEC   = rtcc.second;
//                                RTCMIN   = rtcc.minute;
//                                RTCHOUR  = rtcc.hour;//0x5;
//                                //RTCDOW   = rtcc.;//0x3;
//                                RTCDAY   = rtcc.day;//0x8;
//                                RTCMON   = rtcc.month;//0x07;
//                                RTCYEAR  = rtcc.year;//0x2013;                    
//                                
//                                RTCCTL13 &= ~RTCHOLD;                   // Enable RTC
//                                RTCCTL0_H = 0;   // LOCK RTC                                
//                                write_succesfull = 1;
//                                
//                              }
//                              else if(TheID[4] == 5) //Week day
//                              {                                 
//
//                              }
//                              else if(TheID[4] == 6) //time of last reset
//                              {
//                              }
//                              else if(TheID[4] == 7) //date of last reset 
//                              {
//                              }
//                              else if(TheID[4] == 8) //output pulse duration
//                              {
//
//                              }
//                            }
//                            else
//                            {
//                              //implement next
//                            }
//                          }
//                          else if(TheID[3] == 10 && TheID[2] == 99) //c =99
//                          {
//                            if(TheID[4] == 1 ) //Dips voltage profile
//                            {
//                            }
//                            else if(TheID[4] ==2) //swells voltage profile 
//                            {
//                            }
//                            else if(TheID[4] == 3) //cuts voltage profile 
//                            {
//                            }
//                          }
//                          else if(TheID[3] == 31)//under limit thrushold
//                          {
//                                switch (TheID[2]) //switch with field C
//				{ 
//				case VOLTAGE:
//                                case L1_VOLTAGE:     
//                                case NEUTRAL_VOLTAGE:
//                                  
//					 MIN_VRMS= data_value;
//                                         write_succesfull = 1;
//					break;
//				case AVARAGE_POWER_FACOR:
//                                case L1_POWER_FACTOR:
//
//					break;
//				case SPLLAY_FREQUENCY:
//                                case L1_FREQUENCY:
//					 MIN_Frequency = data_value;
//                                         write_succesfull = 1;
//					break;
//				}
//                          }
//                          else if(TheID[3] == 32)//under limit occurance counter
//                          {
//                          }
//                          else if (TheID[3] == 33)//under limit duration 
//                          {                            
//                              switch(TheID[2])
//                              {
//				
//				case VOLTAGE:
//                                case L1_VOLTAGE:
//                                case NEUTRAL_VOLTAGE:                                        
//					  UnderVoltageTripDuration  = data_value;    
//                                          write_succesfull = 1;                                
//					break;
//				case AVARAGE_POWER_FACOR:
//                                case L1_POWER_FACTOR:
//					break;
//				case SPLLAY_FREQUENCY:
//                                case L1_FREQUENCY:
//					  UnderFrequencyTripDuration=data_value; 
//                                           write_succesfull = 1;                                     
//					break;
//				             
//                              
//                            }                            
//                          }
//			  else if(TheID[3] == 35)//over limit thrushold
//                          {
//                              switch(TheID[2])
//                              {
//				case TOTAL_ACTIVE_POWER_PLUS:
//				case TOTAL_ACTIVE_POWER_MINUS:
//				case TOTAL_REACTIVE_POWER_PLUS:
//				case TOTAL_REACTIVE_POWER_MINUS:
//				case TOTAL_REACTIVE_POWER_QI:
//				case TOTAL_REACTIVE_POWER_QII:
//				case TOTAL_REACTIVE_POWER_QIII:
//				case TOTAL_REACTIVE_POWER_QIV:
//				case TOTAL_APPARENT_POWER_PLUS:
//				case TOTAL_APPARENT_POWER_MINUS:
//				case TOTAL_ACTIVE_POWER_QI:
//				case TOTAL_ACTIVE_POWER_QII:
//				case TOTAL_ACTIVE_POWER_QIII:
//				case TOTAL_ACTIVE_POWER_QIV:
//                                case TOTAL_POSITIVE_ACTIVE_POWER: //SUM Active power QI+QIV+QII+QIII
//				case NET_POSITIVE_ACTIVE_POWER: //    Active power QI+QIV-QII-QIII
//				case L1_ACTIVE_POWER_PLUS:
//				case L1_ACTIVE_POWER_MINUS:
//				case L1_REACTIVE_POWER_PLUS:
//				case L1_REACTIVE_POWER_MINUS:
//				case L1_REACTIVE_POWER_QI:
//				case L1_REACTIVE_POWER_QII:
//				case L1_REACTIVE_POWER_QIII:
//				case L1_REACTIVE_POWER_QIV:
//				case L1_APPARENT_POWER_PLUS:
//				case L1_APPARENT_POWER_MINUS:				                                 
//				case L1_POSITIVE_ACTIVE_POWER: //the instantanious power is only one 
//                                case L1_Net_POSITIVE_ACTIVE_POWER:
//				case L1_ACTIVE_POWER_QI:
//				case L1_ACTIVE_POWER_QII:
//				case L1_ACTIVE_POWER_QIII:
//				case L1_ACTIVE_POWER_QIV:                                  
//			          MAXActivePowerTripPoint = data_value;
//                                  write_succesfull = 1;
//                                  break;
//				case CURRENT:
//                                case L1_CURRENT:
//                                case NEUTRAL_CURRENT:                   
//					 MAX_IRMS = data_value;
//                                         write_succesfull = 1;                                       
//					break;
//				case VOLTAGE:
//                                case L1_VOLTAGE:
//                                case NEUTRAL_VOLTAGE:                                        
//					  MAX_VRMS = data_value; 
//                                          write_succesfull = 1;
//					break;
//				case AVARAGE_POWER_FACOR:
//                                case L1_POWER_FACTOR:
//					break;
//				case SPLLAY_FREQUENCY:
//                                case L1_FREQUENCY:                                       
//					  MAX_Frequency=data_value;
//                                          write_succesfull = 1;
//					break;
//				}                           
//                          }
//                          else if(TheID[3] == 36)//over limit occurance counter
//                          {
//                          }
//                          else if (TheID[3] == 37)//over limit duration 
//                          {                            
//                              switch(TheID[2])
//                              {
//				case TOTAL_ACTIVE_POWER_PLUS:
//				case TOTAL_ACTIVE_POWER_MINUS:
//				case TOTAL_REACTIVE_POWER_PLUS:
//				case TOTAL_REACTIVE_POWER_MINUS:
//				case TOTAL_REACTIVE_POWER_QI:
//				case TOTAL_REACTIVE_POWER_QII:
//				case TOTAL_REACTIVE_POWER_QIII:
//				case TOTAL_REACTIVE_POWER_QIV:
//				case TOTAL_APPARENT_POWER_PLUS:
//				case TOTAL_APPARENT_POWER_MINUS:
//				case TOTAL_ACTIVE_POWER_QI:
//				case TOTAL_ACTIVE_POWER_QII:
//				case TOTAL_ACTIVE_POWER_QIII:
//				case TOTAL_ACTIVE_POWER_QIV:
//                                case TOTAL_POSITIVE_ACTIVE_POWER: //SUM Active power QI+QIV+QII+QIII
//				case NET_POSITIVE_ACTIVE_POWER: //    Active power QI+QIV-QII-QIII
//				case L1_ACTIVE_POWER_PLUS:
//				case L1_ACTIVE_POWER_MINUS:
//				case L1_REACTIVE_POWER_PLUS:
//				case L1_REACTIVE_POWER_MINUS:
//				case L1_REACTIVE_POWER_QI:
//				case L1_REACTIVE_POWER_QII:
//				case L1_REACTIVE_POWER_QIII:
//				case L1_REACTIVE_POWER_QIV:
//				case L1_APPARENT_POWER_PLUS:
//				case L1_APPARENT_POWER_MINUS:				                                 
//				case L1_POSITIVE_ACTIVE_POWER: //the instantanious power is only one 
//                                case L1_Net_POSITIVE_ACTIVE_POWER:
//				case L1_ACTIVE_POWER_QI:
//				case L1_ACTIVE_POWER_QII:
//				case L1_ACTIVE_POWER_QIII:
//				case L1_ACTIVE_POWER_QIV:                                  
//			             ActivePowerTripDuration = data_value;
//                                     write_succesfull = 1;
//                                  break;
//				case CURRENT:
//                                case L1_CURRENT:
//                                case NEUTRAL_CURRENT:                   
//			             OverCurrentTripDuration = data_value;
//                                     write_succesfull = 1;                                       
//					break;
//				case VOLTAGE:
//                                case L1_VOLTAGE:
//                                case NEUTRAL_VOLTAGE:                                        
//			             OverVoltageTripDuration  = data_value;  
//                                     write_succesfull = 1;                                  
//					break;
//				case AVARAGE_POWER_FACOR:
//                                case L1_POWER_FACTOR:
//					break;
//				case SPLLAY_FREQUENCY:
//                                case L1_FREQUENCY:
//				      OverFrequencyTripDuration=data_value; 
//                                      write_succesfull = 1;
//					break;
//				             
//                              
//                            }                            
//                          }
//			  else if(TheID[3] == 39)//missing thrushold
//                          {
//                          }
//                          else if(TheID[3] == 40)//missing occurance counter
//                          {
//                          }			
//                          else if(TheID[3] == 98  && TheID[2] == 99)//Event log
//                          {
//                             
//                          }
//		}
//		break;
//	default:
//		//no other chanel is availabel in our meter
//		break;
//	}
//        return write_succesfull;
//}
///*
// this procedure formats the data as per the standard and sends the dataset
// id:- the id from the recived message
// id_length:- length of the id field
// value:- the value
// unit:- character array holding the unit to send the data
// unit_length:- the length of the unit field
// timestump :- the timestump to send with the data
// valid_timestump:- indicates wheter the timestump filed is to be used or not
// 0: timestump is not used
// 1: timestump is used
// */
//
//void SendReplay(DataToSend *dataToSend) //ola almost done 
//{
//	uint8_t datatosend[50];
//	//uint8_t ID[17]; //max id is 17
//
//	//uint8_t TheID[6];
//	//EnergyBillingCutoffDayLog l;
//
//	//unsigned long longtemp = 0;
//	rtc_t temp_time;
//
//	uint8_t bcd[5] = { 0, 0, 0, 0, 0 };
//	uint8_t temp = 0;
//	uint8_t counter = 0;
//
//	for (counter = 0; counter < dataToSend->id_length; counter++) {
//		datatosend[counter] = dataToSend->Id[counter];
//		//(dataToSend->Id)++;
//	}
//
//	datatosend[counter] = '(';
//        counter++;
//        
//        if(dataToSend->isDate ==1)
//        {
//          
//		temp_time =dataToSend->data.Date_Time;
//                
//                if(dataToSend->onlytime !=1)
//                {
//                
//		  BIN2BCD = temp_time.year;
//		  datatosend[counter] = (BIN2BCD >> 4) + 48;
//		  counter++;
//		  datatosend[counter] = (BIN2BCD & 0x0f) + 48;
//		  counter++;
//
//		  datatosend[counter] = '-';
//		  counter++;
//
//		  BIN2BCD = temp_time.month;
//		  datatosend[counter] = (BIN2BCD >> 4) + 48;
//		  counter++;
//		  datatosend[counter] = (BIN2BCD & 0x0f) + 48;
//		  counter++;
//
//		  datatosend[counter] = '-';
//		  counter++;
//
//		  BIN2BCD = temp_time.day;
//		  datatosend[counter] = (BIN2BCD >> 4) + 48;
//		  counter++;
//		  datatosend[counter] = (BIN2BCD & 0x0f) + 48;
//		  counter++;
//
//		  datatosend[counter] = ' ';
//		  counter++;
//                }
//                
//                if(dataToSend->onlydate !=1)
//                {
//		   BIN2BCD = temp_time.hour;
//		   datatosend[counter] = (BIN2BCD >> 4) + 48;
//		   counter++;
//		   datatosend[counter] = (BIN2BCD & 0x0f) + 48;
//		   counter++;
//
//		   datatosend[counter] = ':';
//		   counter++;
//
//		   BIN2BCD = temp_time.minute;
//		   datatosend[counter] = (BIN2BCD >> 4) + 48;
//		   counter++;
//		   datatosend[counter] = (BIN2BCD & 0x0f) + 48;
//		   counter++;       
//                }
//                              
//        }
//        else{
//	     bin2bcd32(bcd, dataToSend->data.longtemp); //modify this to handle time data type 
//
//	     for (temp = 0; temp < 5; temp++) {
//		BIN2BCD = bcd[temp];
//		datatosend[counter] = (BIN2BCD >> 4) + 48; //to ASCI
//		counter++;
//		datatosend[counter] = (BIN2BCD & 0x0f) + 48; //to ASCI
//		counter++;
//	    }
//        }
//
//        
//        if(dataToSend->unit_length > 0) //the parameter have valid unit field 
//        {	  
//           datatosend[counter] = '*';
//	   counter++;
//          for(temp =0;temp<dataToSend->unit_length;temp++)
//          {
//
//	   datatosend[counter] = dataToSend->unit[temp];
//	   counter++;
//           //unit++;
//          }
//        }
//        
//	datatosend[counter] = ')';
//	counter++;
//
//	if (dataToSend-> valid_timestamp == 1) {
//
//		datatosend[counter] = '('; //add the time stamp from this point forwared
//		counter++;
//
//		temp_time = dataToSend->timeStump;
//
//		BIN2BCD = temp_time.year;
//		datatosend[counter] = (BIN2BCD >> 4) + 48;
//		counter++;
//		datatosend[counter] = (BIN2BCD & 0x0f) + 48;
//		counter++;
//
//		datatosend[counter] = '-';
//		counter++;
//
//		BIN2BCD = temp_time.month;
//		datatosend[counter] = (BIN2BCD >> 4) + 48;
//		counter++;
//		datatosend[counter] = (BIN2BCD & 0x0f) + 48;
//		counter++;
//
//		datatosend[counter] = '-';
//		counter++;
//
//		BIN2BCD = temp_time.day;
//		datatosend[counter] = (BIN2BCD >> 4) + 48;
//		counter++;
//		datatosend[counter] = (BIN2BCD & 0x0f) + 48;
//		counter++;
//
//		datatosend[counter] = ' ';
//		counter++;
//
//		BIN2BCD = temp_time.hour;
//		datatosend[counter] = (BIN2BCD >> 4) + 48;
//		counter++;
//		datatosend[counter] = (BIN2BCD & 0x0f) + 48;
//		counter++;
//
//		datatosend[counter] = ':';
//		counter++;
//
//		BIN2BCD = temp_time.minute;
//		datatosend[counter] = (BIN2BCD >> 4) + 48;
//		counter++;
//		datatosend[counter] = (BIN2BCD & 0x0f) + 48;
//		counter++;
//
//		datatosend[counter] = ')';
//		counter++;
//
//	}
//	IEC_SendDataMessage(datatosend, counter, 1); //send the message in programing mode
//}
//
//
//
///*
//
// converts a character array to an integer
// this is only for 8 bit number 
// */
//uint8_t ToInt(const uint8_t *data, uint8_t length, uint8_t LSB_Firist) {
//	uint8_t output = 0;
//	int8_t t = 0, position = 0;
//	if (LSB_Firist == 1) {
//		position = 0;
//	} else {
//		position = length - 1;
//	}
//
//	for (; t < length; t++) {
//		output += (*data - 48) * (multiplier[position]);
//		if (LSB_Firist == 1) {
//			position += 1;
//		} else {
//			position -= 1;
//		}
//		data++;
//	}
//
//	return output;
//
//}
//
///*
// ID = uint8_t [6] {A,B,C,D,E,F}
// data = the data containing the ID, i.e ID (   untill the open bracket of the data set
// */
//uint8_t getID(uint8_t *ID, const uint8_t *data, uint8_t datalength) {
//
//	//Assume LSB firist
//	uint8_t i = 0, j = 0, k = 0, tempcount = 0;
//	uint8_t tempdata[5] = { 0, 0, 0, 0, 0 }, ID_temp[6] = { '#', '#', '#', '#',
//			'#', '#' };
//	for (; i < datalength && *data!='('; i++) {
//		tempdata[tempcount] = *data;
//
//		if (*data == '-') {
//			ID_temp[0] = ToInt(tempdata, tempcount, 0);
//			tempcount = 0;
//			j = 1;
//		} else if (*data == ':') {
//			ID_temp[1] = ToInt(tempdata, tempcount, 0);
//			tempcount = 0;
//			j = 2;
//		} else if (*data == '.' && k == 0) {
//			ID_temp[2] = ToInt(tempdata, tempcount, 0);
//			tempcount = 0;
//			k = 1;
//			j = 3;
//		} else if (*data == '.' && k == 1) {
//			ID_temp[3] = ToInt(tempdata, tempcount, 0);
//			tempcount = 0;
//			j = 4;
//		}
//
//		else if (*data == '*') {
//			ID_temp[4] = ToInt(tempdata, tempcount, 0);
//			tempcount = 0;
//			j = 5;
//		} else {
//			tempcount++;
//		}
//		data++;
//	}
//        
//        if(*data != '(' && i >= datalength)
//        {
//          return 0;
//        }
//        else
//	  ID_temp[j] = ToInt(tempdata, tempcount, 1);
//
//	for (i = 0; i < 6; i++) {
//		*ID = ID_temp[i];
//		ID++;
//
//	}
//        return 1;
//}
//
///*
// call this method at the end of communication or after a time out
// which is a maximum of 200msec( inter character time out )
//
// */
//void ResetCommunication() {
//	//if boude raet is not 300db
//	//intialize to 300bd boude rate
//	change_boud_rate('0');
//        //IEC_State = WaittingForRequest;old
//        IEC_State = HandShake;
//        Error_Char_Recived = 0; 
//        IEC_Rx_Buffer_Index = 0;
//        
//        
//     if(taskScaduled == 1)
//        CancelTask2(&ResetCommunication);//CancelTask(scaduledResetTaskNumber);
//     
////     scaduledResetTaskNumber = SchaduleTask(&ResetCommunication,InterFrameTimeOutMax);
////     taskScaduled = 1;
//     
//	//reset variables
//	//reset_Variables();old
//	//cleare password flags
//	//       
//        HandShakeState = WaitingForRequest;           
//	password_permission_level = 0;
//        //Timer2_OFF();
//}
//void IEC_SendIdentificationMessage() {
//        IEC_Tx_Buffer_Index = 0;
//	IEC_AddtoTXBuffer(Start_Char);
//	IEC_AddtoTXBuffer('X');
//	IEC_AddtoTXBuffer('X');
//	IEC_AddtoTXBuffer('X');
//	IEC_AddtoTXBuffer(boude_rate);   //boude rate 300db
//
//	int i = 0;
//	for (; i < 16; i++)
//		IEC_AddtoTXBuffer(Identification[i]);
//
//	IEC_AddtoTXBuffer(CR);
//	IEC_AddtoTXBuffer(LF);
//
//	//IEC_Start_SendBuffer();   //start the TX
//        ScaduleTask(IEC_Start_SendBuffer,InterFrameTimeOutMin,IEC_SEND_START_BUFFER_TASK);//SchaduleTask(&IEC_Start_SendBuffer,InterFrameTimeOutMin);
//        //IEC_Ready_To_Tx =1;
//}
//
//void IEC_SendDataMessage(const uint8_t *data, uint8_t length, uint8_t isprogramming) {
//        IEC_Tx_Buffer_Index = 0;
//	uint8_t BCC = 0, i = 0;
//	
//	IEC_AddtoTXBuffer(STX);
//
//	for (; i < length; i++) {
//		IEC_AddtoTXBuffer(*data);
//		BCC ^= *data;
//		data++;
//	}
//
//	if (isprogramming == 1) {
//	} else {
//		IEC_AddtoTXBuffer(End_Char);
//		BCC ^= End_Char;
//		IEC_AddtoTXBuffer(CR);
//		BCC ^= CR;
//		IEC_AddtoTXBuffer(LF);
//		BCC ^= LF;
//	}
//
//	IEC_AddtoTXBuffer(ETX);
//	BCC ^= ETX;
//	IEC_AddtoTXBuffer(BCC);
//
//	//IEC_Start_SendBuffer();   //start the TX
//        ScaduleTask(IEC_Start_SendBuffer,InterFrameTimeOutMin,IEC_SEND_START_BUFFER_TASK);//SchaduleTask(&IEC_Start_SendBuffer,InterFrameTimeOutMin);
//        //IEC_Ready_To_Tx  =1;
//}
//
//void IEC_SendErrorMessage(uint8_t *ErrorDate,uint8_t length)
//{
//  	uint8_t BCC = 0, i = 0;
//	IEC_Tx_Buffer_Index = 0;
//	IEC_AddtoTXBuffer(STX);
//        
//        for(i=0;i<ErrorIdentifierLength;i++)
//        {
//           IEC_AddtoTXBuffer(ErrorIdentifier[i]);
//        }
//        
//	for (i=0; i < length; i++) {
//		IEC_AddtoTXBuffer(*ErrorDate);
//		BCC ^= *ErrorDate;
//		ErrorDate++;
//	}
//
//	IEC_AddtoTXBuffer(ETX);
//	BCC ^= ETX;
//	IEC_AddtoTXBuffer(BCC);
//
//	//IEC_Start_SendBuffer();   //start the TX
//        ScaduleTask(IEC_Start_SendBuffer,InterFrameTimeOutMin,IEC_SEND_START_BUFFER_TASK);//SchaduleTask(&IEC_Start_SendBuffer,InterFrameTimeOutMin);
//        //IEC_Ready_To_Tx =1;
//}
//
//void IEC_SendAcknowledgmentMessage() {
//        IEC_Tx_Buffer_Index = 0;
//	IEC_AddtoTXBuffer(ACK);
//	//IEC_Start_SendBuffer();   //start the TX
//        ScaduleTask(IEC_Start_SendBuffer,InterFrameTimeOutMin,IEC_SEND_START_BUFFER_TASK);//SchaduleTask(&IEC_Start_SendBuffer,InterFrameTimeOutMin);
//        //IEC_Ready_To_Tx = 1;
//}
//
//void IEC_SendRepeatRequestMessage() {
//  IEC_Tx_Buffer_Index = 0;
//	IEC_AddtoTXBuffer(NAK);
//	//IEC_Start_SendBuffer();   //start the TX
//        ScaduleTask(IEC_Start_SendBuffer,InterFrameTimeOutMin,IEC_SEND_START_BUFFER_TASK);//SchaduleTask(&IEC_Start_SendBuffer,InterFrameTimeOutMin);
//        //IEC_Ready_To_Tx = 1;
//}
//
//void IEC_send_perdefiendReport() {
//  IEC_Tx_Buffer_Index = 0;
//}
//
//
//
