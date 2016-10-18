

#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H
/**
* Read TI host message specification
*/
//#include <stdint.h>
/***************************************/
/*  Methods for GET_SYSTEM_INFO       */
/***************************************/

/*
 *               structure for get_System_INFO
 *             Device Type             Description
		1 		    PRIME IEC-432-LLC convergence
		2 			PRIME IP convergence
		3			G3
		4			FLEX
		5			FLEX Lite
		6			FLEX OFDM
		7			Reserved
		8			P1901.2


		Device Mode       Value    Description
		Normal Mode			0		This is device normal operation mode for device type of:

										1.     PRIME IEC4-32 LLC
										2.     PRIME IPv4/IPv6
										3.     G3
		MAC Mode			1		Device is operating in MAC mode with Embedded AppEMU running in the device.

		LLC Mode 			2		Device is operating in LLC mode with Embedded AppEMU on top of IEC432-LLC

		P2P Mode 			3		Device is operating in Point-to-Point test mode for all device types.

		MAC Mode (no eAppEmu)4		Device is operating in MAC Mode without
									Embedded AppEMU running in the device.
									Applicable  only to PRIME device types.
		PHY Mode 			5		Device is operating in PHY Mode for all device types.

		G3 Embedded App Emu 7		G3 Device running the Embedded AppEmu.Applicable only to G3 devices
 */
typedef struct
{
  uint8_t Firmware_Version[4];  		    //from byte 0 to 3 MAJOR.MINOR.REVISION.BUILD.
  int  Device_Serial_Number_Length;
  int Device_Serial_Number[16];

  
  uint8_t Device_Type;	  			//from byte 22 to 22
  uint8_t Device_Mode;     			//from byte 23 to 23
  int HardwareRevision;    			//from byte 24 to 25  (0 pre D revision)  (0x44,D has EEPROM)

  uint8_t MAC_Address[6];  
  uint8_t PortAssignments; 			//from byte 32 to 32
    
  uint8_t PHY_Settings;
  //int reserved[3]
  uint8_t ARQ_PAC_Flags;
  uint8_t Security_Profile;
  
  
//#ifdef _PRIME_IPV4_
  long Source_IPv4_Add;
  long Net_Mask;
  long Gateway;
//#else
//  int Address_Prefix [4];
//  int Source_Address[4];
//#endif   
  int UPD_Port;  
}System_Info_t;

extern System_Info_t system_info;


enum DeviceTypes{
  PRIME_IEC_432_LLC_convergenc = 1,
  PRIME_IP_convergence = 2,
  G3 = 3,
  FLEX = 4,
  FLEX_Lite = 5,
  FLEX_OFDM = 6,
  //Reserved = 7,
  P1901_2 = 8,
};

enum DeviceModes{
  Normal_Mode = 0,
  MAC_Mode = 1,
  LLC_Mode = 2,
  P2P_Mode  =3,
  MAC_ModeEAppEMU = 4,
  PHY_Mode = 5 ,
  G3_EAppEMU = 7
};
void GetSystemInfoRequest();
void GetSystemInfoResponse();



/***************************************/
/*  Methods for SET_INFO      */
/***************************************/

typedef struct
{
	int INFO_Type;           //Type of information being set.
	int INFO_LENGHT;         //Length of INFO_Value, in bytes.
	uint8_t *INFO_Value; // change the size to something else,Value depending on the INFO_Type.

}SysInfoReq_type;



/*
 * Tone masks
 * Set spectral mask (static) where tones start, stop and notch.
	UINT8 toneMask[0]: tone number for the first tone in the band.
	UINT8 toneMask[1] bit6-bit0: number of tones in the band.
	UINT8 toneMask[2] – [11]: tone mask, 1 indicates the tone is on, 0
	indicates the tone is off.
	Firmware versions 1.3.1 and earlier use 10 bytes.
	Firmware versions later than 1.3.1  and less than 5.0.0.0 use 12
	bytes.
	Firmware versions later and equal to 5.0.0.0 user 14 bytes
 */
//const uint8_t Cenelec_A_36 [] = {0x17,0x24,0xff,0xff,0xff,0xff,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
//const uint8_t Cenelec_A_25 [] = {0x17,0x24,0xff,0xff,0x00,0xf8,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
//const uint8_t Cenelec_B    [] = {0x3f,0x10,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
//const uint8_t Cenelec_BC   [] = {0x3f,0x1a,0xff,0xff,0xff,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
//const uint8_t Cenelec_BCD  [] = {0x3f,0x20,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
//const uint8_t FCC_low_band [] = {0x21,0x24,0xff,0xff,0xff,0xff,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
//const uint8_t FCC_high_band[] = {0x45,0x24,0xff,0xff,0xff,0xff,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
//const uint8_t FCC_full_band[] = {0x21,0x48,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00 };



enum INFO_Types
{
	PRIME_PHY_TX_Info_Type =0x0,
	PRIME_PHY_RX_Info_Type =0x01,
	G3_PHY_TX_Info_Type = 0x02,
	G3_PHY_RX_Info_Type = 0x03,
	Vendor_and_Product_Id_Info_Type = 0x04,
	TX_PHY_Attenuation_Info_Type = 0x09
};


#define FEC_flag 0x01	
#define ROBO_flag 0x02   //0 PRIME mode 1 ROBO mode 
#define PRM_flag  0x04   //

enum ModulationTypePRIME
{
   DBPSK = 0,
   DQPSK = 1,
   D8PSK = 2,
   DBPSK_1_4_Recep = 3,
   DBPSK_1_8_Recep = 4
};

typedef struct
{
   //uint8_t reserved;
   uint8_t flags;
   int Modulation;
   int TX_Level;
   
}PRIME_PHY_TX_Parameters;




#ifdef _USE_G3_
//1 = Cenelec/FCC
#define flag_TMR  0x01  //Tone Map Request Mode
			//0 = disabled – non designated mode
			//1 = Enabled – PHY set to Tone Mask Req Mode.

#define flag_COH  0x08  //Coherent Modulation. A value of 1 for COH on and 0 for COH
						//off

#define flag_Band_Sel 0x40  //0 = Cenelec
enum ModulationType
{
	ROBO = 0x0,
	BPSK = 0x01,
	QPSK = 0x02,
	Eight_PSK = 0x03
};


typedef struct
{
	uint8_t flags;    //use the above flag definitions
	uint8_t modulation;
	int tx_level;   //0 - 32  According to the G3 specification
	uint8_t tone_maks[14];
}G3_PHY_TX_Parameters;



#define BLI 0x4 //Block Level Interleaver
#define AGC  0x01  //Automatic gain control

typedef struct
{
  uint8_t reserved;
  uint8_t flags;
  int Gain_value;
  uint8_t tone_mask[14];
}G3_PHY_RX_Parameters;

typedef struct
{
	int VendorID;		//Vendor ID (assigned by Prime Alliance for PRIME)
	uint8_t ProductID[16];//Vendor Specifics Identifier in ASCII, terminating with a NULL
						//character
}Vendor_ID;

typedef struct
{
	uint8_t TX_Gain;// The range is from -6 dB (-60 or 0xC4 ) to 3dB (30 or 0x1E ) in
					//  0.1 dB steps

	uint8_t TX_PGA;  //0 =  0 dB
					 //1 = -3 dB
					 //2 = -6 dB
}TX_PHY_Attenuation;

#endif


void SET_INFO_Request(unsigned int info_type);
void SET_INFO_Replay();

#endif 
