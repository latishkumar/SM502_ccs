///#define __MSP430F6736___
//
//#include <msp430.h>
//#include <stdint.h>
//#include "emeter-toolkit.h"
//#include "../emeter-structs.h"
//
////#include "UART.h"
//#include "graphics.h"
//#include "EventTypes.h"
//#include "Status.h"
//#include "Logger.h"
//#include "Tamper.h"
//#include "I2C2EEPROM.h"
//#include "RTCC.h"
//
////#include "Communication.h"
////#include "Configuration.h"
////#include "Tamper.h"
////#include "Tariff.h"
//
//#include "UI.h"
//#define RelayPort P6OUT
//#define RelayDir  P6DIR
//#define RelayBIT  BIT5
//
////#define __MAIN_PROGRAM__
//
//int i = 0;
//extern int vrms;
//
//int Disconnect = 0;
//
//MeterStatus status;
//
//void ConnectMeter(); //Connect the meter form the grid
//void IsDisconnectRequired();
//void DisconnectMeter(); //Disconnects the meter form the grid
//void IsDisconnectRequired(); //checks the different cases for disconnect and updates the MeterStatus
//void calculateReadings();                 // call this on the foreground process
//void performPowerQualityMessurement(); // some of the power quality management is performed by the
//// background process. perform the remaining process hear
//void ProcessPowerFailer();
//void TransmitMessage();

//
//
//void InitOscillator()
//{
//	//initialize System Clock to 16MHz
//	//OSC is external 32.768KHz
//	//N=255
//	//Fdco = (N+1) * Fosc * FLLDIV
//	//Fdco = 254 * 32.768KHz *2 =  16.777216MHz //this is source for
//	//Fdcodiv = 8.388608MHz
//
//
//	#if defined(__MSP430_HAS_UCS__)
//		__bis_SR_register(SCG0);
//		/* Disable the FLL control loop */
//		UCSCTL6 = (UCSCTL6 | XT1DRIVE_3); /* Highest drive setting for XT1 startup */
//		while (SFRIFG1 & OFIFG) {
//			/* Check OFIFG fault flag */
//			UCSCTL7 &= ~(DCOFFG | XT1LFOFFG | XT2OFFG); /* Clear OSC fault flags */
//			SFRIFG1 &= ~OFIFG; /* Clear OFIFG fault flag */
//		}
//		//UCSCTL6 = (UCSCTL6 & ~(XT1DRIVE_3)); /* Reduce drive to something weaker */
//
//		UCSCTL1 = DCORSEL_5; /* Set RSELx for DCO = 16 MHz */
//		UCSCTL2 = FLLD_1 | 255; /* Set DCO Multiplier for 8MHz */
//		/* (N + 1) * FLLRef = FDCOCLKDIV */
//		/* (254 + 1) * 32768 = 8355840Hz */ //for UART setting
//		/* Set FLL Div = fDCOCLK/2 */
//		__bic_SR_register(SCG0);
//		/* Enable the FLL control loop */
//
//		UCSCTL5 |= DIVS1;
//		UCSCTL4 = SELM__DCOCLK | SELS__DCOCLK | SELA__XT1CLK; //16MHz MCLK, 16MHz SMCLK, 32KHz ACLK
//
//		/* Suitable settings for a 6736 */
//		PJDIR = BIT0 | BIT1 | BIT3; /* SMCLK, MCLK, ACLK */
//		PJSEL = BIT0 | BIT1 | BIT3; /* SMCLK, MCLK, ACLK */
//	#endif
//
//}
//
//void InitSystem() {
//
//	WDTCTL = WDTPW | WDTHOLD;	// Stop Watch dog timer
//
//	//InitOscillator();
//
////end of Initialize System Clock
//	RTCCinit();  // this should be initialized first
//	//P1DIR = 1;
//
//	InitLogg();
//	InitUI();
//
////	P5DIR |= BIT1 | BIT2;
//	// P5OUT = 1;
//	initTampperSwitchs();
//
//	InitGLCD();
//
////	/*
////	 * Initialize Relay
////	 */
////	RelayDir |= RelayBIT; // set to output
////	RelayPort &= ~RelayBIT; //output low
//
//
//	// enable Global Interrupt
//	__bis_SR_register( GIE);
//}
//
//extern unsigned int LastEventLogAddress;
// 
//struct phase_parms_s chan1;
//
//int main()
//{

//        
//  while(1)
//  {
//    kick_watchdog();

//        ReadKey();
//	if (status.KeyReleased == 1) {
//
//		ProcessKeyPress();
////		RedLedOn();
////		GreenLedOn();
//
//		}
//          if ((phase->status & NEW_LOG))
//            {
//               phase->status &= ~NEW_LOG; 
//                x = active_power();
//                    phase->readings.I_rms = current();
//                    phase->readings.V_rms = voltage();
//                    
//                     total_active_power = x;
//                     phase->readings.active_power = x;
//
//                x = reactive_power();
//                total_reactive_power = x;
//                phase->readings.reactive_power = x;
//                  phase->readings.apparent_power = apparent_power();
//                phase->readings.power_factor = power_factor();
//                
//                
//
//                phase->active_energy_counter += x*phase->metrology.dot_prod_logged.sample_count;
//                while (phase->active_energy_counter > ENERGY_WATT_HOUR_THRESHOLD)
//                {
//                    phase->active_energy_counter -= ENERGY_WATT_HOUR_THRESHOLD;
//                    phase->consumed_active_energy++;
//                }
//                phase->readings.frequency = frequency();
//                status.DisplayUpdateRequired = 1;
//            }
//              UpdateDisplay();
//  }
//  //return 0;
//}
//
///*
// * main.ci
// */
////int main(void) {i 
////    static int32_t x;
////    int i;
////	InitSystem();
//// 
////
////			   status.UpdateDate = 1;
////			   status.UpdateTime = 1;
////			   status.DisplayState = 0;
////			   status.DisplayUpdateRequired = 1; 
////			   status.TamperDetectedStatus = 0;
////			UpdateDisplay();
////
////    //unsigned long data;i
////    //int x=0; 
////	while (1) {int
////    if ((phase->status & NEW_LOG))
////            {
////               phase->status &= ~NEW_LOG; 
////                x = active_power();
////                    phase->readings.I_rms = current();
////                    phase->readings.V_rms = voltage();
////                    
////                     total_active_power = x;
////                     phase->readings.active_power = x;
////
////                x = reactive_power();
////                total_reactive_power = x;
////                phase->readings.reactive_power = x;
////                  phase->readings.apparent_power = apparent_power();
////                phase->readings.power_factor = power_factor();
////                
////                
////
////                phase->active_energy_counter += x*phase->metrology.dot_prod_logged.sample_count;
////                while (phase->active_energy_counter > ENERGY_WATT_HOUR_THRESHOLD)
////                {
////                    phase->active_energy_counter -= ENERGY_WATT_HOUR_THRESHOLD;
////                    phase->consumed_active_energy++;
////                }
////                phase->readings.frequency = frequency();
////                
////            }
////
////				CheckTamperSwithces();// from the tamper module
////				if (status.TamperDetectedStatus == 1) {
////					ProcessTamper();
////				}
////
////				ReadKey();
////				if (status.KeyReleased == 1) {
////
////					  ProcessKeyPress();
////					  RedLedOn();
////					  GreenLedOn();
////
////				}
////				UpdateDisplay();
////	}
////
//////		//}
//////
//////		//This future is used by the meter to interrupt power in different cases
//////		if (status.AutoDisconnectEnabled == 1) {
//////			//this method checks if the meter should be disconnected
//////
//////			//it checks if the meter has run out of money
//////			//or if disconnect is ordered for the central control
//////			//is so saves the disconnect reason and disconnects the meter.
//////			//the method also sets the way how the meter can be connected back.
//////			//based on priority
//////			IsDisconnectRequired();
//////			if (Disconnect == 1) {
//////				DisconnectMeter();
//////			}
//////		} else if (status.MeterDisconnected == 1) {
//////			ConnectMeter();
//////		}
//////
//////		if (status.NewLog == 1) //1 SEC power accumulation is available for processing
//////				{
//////			calculateReadings(); //call this on the foreground process
//////			performPowerQualityMessurement(); // some of the power quality management is performed by the
//////											  //background process. perform the remaining process hear
//////
//////			//CalculateBilling();               // call this on the tariff module
//////		}
//////
//////		//perform other tasks hear
//////
//////		//check for events from each module
//////
//////		if (status.PowerFailDetected == 1)           //power failure is detected
//////				{
//////			ProcessPowerFailer();
//////		}
//////
//////		if (status.CommunicationDetected == 1) // from the communication module.
//////				{
//////			//ProcessRecivedMessage();
//////		}
//////
//////		//CheckTamperSwithces();// from the tamper module
//////		if (status.TamperDetectedStatus == 1) {
//////			//ProcessTamper();
//////		}
//////
//////		if (status.ConfigurationDetectedStatus == 1) {
//////			//EnterConfigurationMode();
//////		}
//////
//////		ReadKey();
//////		if (status.KeyReleased == 1) {
//////			ProcessKeyPress();
//////		}
//////
//////		//Timed Tasks
//////		//Interrupt From Timer
//////		if (status.LoggingTimeIsUp == 1) //Timer
//////				{
//////			//Log energy
//////			//LogeData(); // Data Logging Module
//////		}
//////
//////		//Auto Transmissions are required in log time so
//////		//Instead of using timer to trigger transmission it is better to
//////		//set fixed date and transmit on those days
//////
//////		//CheckForTransmissionDates();
//////		if (status.AutoTransmissionTimeIsUp == 1) {
//////			//TransmitMessage();
//////		}
//////
//////		//this routine should check for 1 time and update the time on the LCD
//////		//it should check for the current date change and update the LCD
//////		//check at witch state it is and update Type and value display areas
//////		//this could be based on the key pad or an error message
//////		//once the LCD changes this displayed values, it should start a timer
//////		//and change back to the Active energy display(default) in say 1 minute time.
//////		UpdateDisplay();    //update the LCD Based On the currentState
//////
//////	}
////	//return 0;
////}
//
////Connects the meter to the grid
//void ConnectMeter() {
//	RelayPort |= RelayBIT;
//}
//
////Disconnects the meter form the grid
//void DisconnectMeter() {
//	RelayPort &= ~RelayBIT;
//}
//
////checks the different cases for disconnect and updates the MeterStatus
//void IsDisconnectRequired() {
//	//if
//	//Disconnect Command Received
//	//Run Auto of bill
//	//Configuration Detected
//
//	Disconnect = 1;
//	//else
//}
//
////call this on the foreground process
//void calculateReadings() {
//
//}
//
////some of the power quality management is performed by the
//void performPowerQualityMessurement() {
//
//	status.DisplayUpdateRequired = 1;
//}
//
////background process. perform the remaining process hear
//void ProcessPowerFailer() {
//	//disable global interrupt
//	//disable ADC24
//	//log event power failure event
//	EventLog l;
//	l.EventCode = PowerOut;
//	l.timeStump = getTimeStamp(2013, 4, 6, 2, 3, 3);
//	l.Checksum = (int) (l.EventCode + l.timeStump.TimestampLow
//			+ l.timeStump.TimestampUp);
//	//l.StartAddress = 0;	//last event log address
//	logEvent(&l);
//	//log energy
//
//	EnergyLog e;
//	e.ActiveEnergy = 1258;
//	e.ReactiveEnergy = 1258;
//	e.timeStump = getTimeStamp(2013, 5, 6, 2, 3, 3);
//	e.CRC = e.ActiveEnergy + e.ReactiveEnergy;
//	//e.startAddress = 0; //last event log address
//	logEnergy(&e);
//
//
//    //log current balance
//	//log last address used variables
//
//	//stop power measurement
//
//	//enable interrupt
//
//	//enter low power mode
//}
//
//void ProcessPowerUp() {
//	//disable global interrupt
//	//log power up event
//	EventLog l;
//	l.EventCode = PowerUp;
//	l.timeStump = getTimeStamp(2013, 4, 6, 2, 3, 3);
//	l.Checksum = (int) (l.EventCode + l.timeStump.TimestampLow
//			+ l.timeStump.TimestampUp);
//	//l.StartAddress = 0;	//last event log address
//	logEvent(&l);
//	//restore data
//
//	//start power measurement
//	//Enable ADC24B
//	//enable global interrupt
//}
//
