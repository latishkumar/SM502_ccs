/*
 * Test.c
 *
 *  Created on: May 31, 2013
 *      Author: Elec_Lab
 */



//	        EnergyLog e,e2;
//			e.ActiveEnergy = 45678;
//			e.ReactiveEnergy = 1258;
//			e.timeStump = getTimeStamp(13, 5, 6, 2, 3, 3);
//			e.CRC = (e.ActiveEnergy + e.ReactiveEnergy) & 0xff;
//			e.startAddress = 0; //last event log address
//			//logEnergy(&e);
//
//			//EEPROM2_WriteLong(1234345,0,1);


//	EventLog evl,evl2;
//	evl2.StartAddress = 10;
//	evl.EventCode = 200;
//	evl.StartAddress = 10;
//	evl.timeStump = getTimeStamp(13,5,28,11,49,30);
//	evl.Checksum = (getCheckSum(evl.timeStump.TimestampLow) + evl.timeStump.TimestampUp + evl.EventCode)&0xff;
//
//	 int b = logEvent(&evl);

//	MeterAddress add,add2;
//	add.Address = 3456789;
//	add.checksum = getCheckSum(add.Address);
//
//	setMeterAddress(&add);

//			   EnergyLedOn();
//			   RedLedOn();
//			   GreenLedOn();

			   //unsigned long var,var1,var2;
//			   	   CurrentBalance b,b2;
//			   	   b.balance = 3789067;
//			   	   b.checksum = getCheckSum(b.balance);
//			   	   setCurrentBalance(&b);
			   //setFirmwareVersion(1);

			   //setTariffRate(0,365657);
			   //EEPROM2_waitFroWrite(80);
			   //setTariffRate(1,2345);
			   //EEPROM2_waitFroWrite(80);
			   //setTariffRate(2,567567);
			   //EEPROM2_waitFroWrite(80);

//			   Schedule_timeslot_t timeslot,t2,t3;
//			   timeslot.start_Hour = 12;
//			   timeslot.start_minute = 30;
//			   timeslot.tariff = 1;
//			   timeslot.checksum =(0xff)&(timeslot.start_Hour + timeslot.start_minute + timeslot.tariff);
//			   setTariffSchaduleTimeslot(1,&timeslot);

//			   EventLog evl,evl2,evl3;
//			   evl.StartAddress = LastEventLogAddress+1;
//			   getEvent(&evl);
//			   updateNextLogAddress(1);
//			   evl2.StartAddress = LastEventLogAddress +1;
//			   updateNextLogAddress(1);
//			   getEvent(&evl2);
//			   evl3.StartAddress = LastEventLogAddress+1;
//			   updateNextLogAddress(1);
//			   getEvent(&evl3);

//			   Public_Holiday_t t,t1,t2;
//			   t.day = 25;
//			   t.month = 10;
//			   t.year = 3;
//			   t.Checksum = (t.day + t.month + t.year) & 0xff;
//
//			   setPublicHoliday(23,&t);
			//BussIdeal2();

//			   int x1,x2,x3;
//			   x1= 150;
//			   //setTamperCount(MagneticTamperType,x1);
//			   setTamperCount(NeutralTamperType,170);

//			   Password_128 p,p2;
//
//			   p.long1 = 23452;
//			   p.long2 = 3456;
//			   p.long3 = 456;
//			   p.long4 =234;
			   //setPassword(&p,Tariff_Configration_Password);

//					  EnergyLedOff();
//					  GreenLedOff();
//					  RedLedOff();

					  //getTariffRate(0,&var);
					  //getTariffRate(1,&var1);
					 // getTariffRate(2,&var2);

//					  getTariffSchaduleTimeslot(0,&t2);
//					  getTariffSchaduleTimeslot(1,&t3);
					  //getFirmwareVersion(&ver);
					  //getCurrentBalance(&b2);
					  //getMeterAddress(&add2);

					    //getEvent(&evl2);
						//getEnergy(&e2,0);
					//EEPROM2_WriteLong(1234,0,1);
					 //EEPROM2_ReadInt8(2,1,&x);
					//EEPROM2_ReadLong(0,1,&data);
					//status.KeyReleased = 0;
//
//					  getPublicHoliday(0,&t1);
//					  getPublicHoliday(23,&t2);

					  //getTamperCount(MagneticTamperType,&vrms);
//					  getTamperCount(NeutralTamperType,&x3);

					  //getPassword(Tariff_Configration_Password,&p2);
