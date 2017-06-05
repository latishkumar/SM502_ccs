/*
 * association_ln.c
 *
 *  Created on: May 12, 2017
 *      Author: abi
 */
#include "headers.h"
#include "association_ln.h"
/*
 * constant parameters for SAP assignment, Association LN, COSEM logical device name
 */

/* SAP_assignment_list
 * list of logical devices 1 logical device(management logical device)
 * with at least 4 associations:
 *  Public client (server address 16),
 *  management client (server address 1),
 *  Reading Client (server address 2) and
 *  update firmware client (server address 3)
 */
const uint8_t sap_list[] =
{
    20 + 19 + 23 + 22 + 1,
        4,
            TAG_STRUCTURE, 2,
                TAG_UINT16, INJECT16(16),                                                           /* SAP */
                TAG_OCTET_STRING, 13, 'P', 'U', 'B', 'L', 'I', 'C', ' ', 'C', 'L', 'I','E','N','T',                          /* Logical device name public client */
            TAG_STRUCTURE, 2,
                TAG_UINT16, INJECT16(2),                                                            /* SAP */
                TAG_OCTET_STRING, 12, 'M', 'E', 'T', 'E', 'R', ' ', 'R', 'E', 'A', 'D','E','R',                              /* Logical device name reading client */
            TAG_STRUCTURE, 2,
                TAG_UINT16, INJECT16(1),                                                            /* SAP */
                TAG_OCTET_STRING, 16, 'U', 'T', 'I', 'L', 'I', 'I', 'T', 'Y', ' ', 'S','E','T','T','I','N','G',              /* Logical device name management client*/
            TAG_STRUCTURE, 2,
                TAG_UINT16, INJECT16(3),                                                            /* SAP */
                TAG_OCTET_STRING, 15, 'F', 'I', 'R', 'M', 'W', 'A', 'R', 'E', ' ', 'U','P','D','A','T','E',                  /* Logical device name firmware client*/
};

/*associated_partners_ids
 * lowest security(NS)
 * Low security (LS)
 * High security (HS)
 */
const uint8_t partner_HS[] =
{
    6,
        2,
            TAG_INT8, 0x30,                         /* Client SAP */
            TAG_UINT16, INJECT16(1)                 /* Server SAP */
};

const uint8_t partner_LS[] =
{
    6,
        2,
            TAG_INT8, 0x20,                         /* Client SAP */
            TAG_UINT16, INJECT16(1)                 /* Server SAP */
};

const uint8_t partner_NS[] =
{
    6,
        2,
            TAG_INT8, 0x10,                         /* Client SAP */
            TAG_UINT16, INJECT16(1)                 /* Server SAP */
};


/*
 * application context name
 */
const uint8_t application_context_name_logical_names[] =
{
    7, 0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x01
};

/*
 * authentication mechanism name
 */
const uint8_t auth_mech_name_NS[]={0x07,0x60,0x85,0x74,0x05,0x08,0x02,0x00};
const uint8_t auth_mech_name_LS[]={0x07,0x60,0x85,0x74,0x05,0x08,0x02,0x01};
const uint8_t auth_mech_name_HS[]={0x07,0x60,0x85,0x74,0x05,0x08,0x02,0x07};

/*
 * Variable parameters for SAP assignment,
 * Association LN, COSEM logical device name
 */

/*association_status
 * type-enum
 */
uint16_t uint8val = 0x00; //TODO

/*unique identification of thelogical device
 * type-octet-string[16]
 */
uint8_t DeviceIdentifier[]={13,'D','V','E','0','1','1','0','0','0','0','0','3','4'};

/*
 * Callback functions used for SAP assignment, Association LN, COSEM logical device name
 */

//TODO. Test this
void authentication_secret_LS(void *data, int direction)
{
   uint8_t *datap = data;
   if(direction == ATTR_READ)
   {
       //according to the current grid's specification this is
       //an eight bit lenght string
       memcpy(datap,(Passwords[1].password+1),Passwords[1].password[1]);
   }
   else // write
   {
     //assume that the format is according to the octat string format i.e. len, data
     uint8_t len = (*datap++);

     if(len > 16) //error
     {
       //
     }
     else
     {
       uint8_t i =1;
       Passwords[i].password[i] = len;
       memcpy((Passwords[i].password+2),datap,len);

       //update the EEPROM
       //TODO. ENCRIPT before writting. There is already a method that encripts and writes to EEPROM in logger module
       //refactor and use it.
       write_to_eeprom(&Passwords[i],&i,setPassword);
//       setPassword(&Passwords[1],1);//TODO encript before setting the password
     }
   }
}

void get_set_device_identifier(void *data,int data_direction)
{
     uint8_t *datap =  data;
    if(data_direction == ATTR_WRITE)
    {
      datap++;
      setLogicDeviceName(datap,(uint8_t * )0);
      //datap++;
      memcpy(device_identifier,datap,13);
    }
    else if(data_direction == ATTR_READ)
    {
       uint8_t  *datap2= (DeviceIdentifier+1);
       memcpy(datap2,device_identifier,13);
       memcpy(datap,DeviceIdentifier,14);
    }
}

