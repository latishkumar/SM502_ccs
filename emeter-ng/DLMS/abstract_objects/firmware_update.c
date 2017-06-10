/*
 * firmware_update.c
 *
 *  Created on: May 15, 2017
 *      Author: abi
 */
#include "headers.h"
#include "firmware_update.h"

/*
 * Image transfer
 * image block size
 */
uint32_t image_block_size = 64;//let's assume a single image block size is 64 bytes.

/*
 * Image transfer
 * Image_transferred_blocks_status
 *    NOT-TRANSFERED = 0;
 *    TRANSFERED = 1;
 */
uint8_t image_transferred_blocks_status;

/*
 * Image transfer
 * mage_first_not_transferred_block_number
 */
uint32_t image_first_not_transferred_block_number = 124;

/*
 * Image transfer
 * mage_transfer_enabled
 */
uint8_t image_transfer_enabled = 1;

/*
 * Image transfer
 * image_transfer_status
 */

uint8_t image_transfer_status;

uint8_t image_identification[]={6,1,2,3,4,5,6};

uint8_t image_signature []={6,1,2,3,4,5,6};
/*
 * Image transfer
 * image_to_activate_info
 */
const uint8_t image_to_activate_info[] =
{
    INJECT16(0x8000 | (1*21 + 1)),
         1,
            TAG_STRUCTURE, 3,
                TAG_UINT32, INJECT32(ITEM_TAG_IMAGE_SIZE),
                STUFF_DATA|TAG_OCTET_STRING, 6, ITEM_TAG_IMAGE_IDENTIFICATION,2,3,4,5,6,
                STUFF_DATA|TAG_OCTET_STRING, 6, ITEM_TAG_IMAGE_SIGNATURE,2,3,4,5,6
};

/*
 * Image transfer
 * get_image_transferred_blocks_status
 */
void get_image_transferred_blocks_status(void *data,int data_direction) // read only method
{
    //uint8_t *datap =  data;
    if(data_direction == ATTR_WRITE)
    {


    }
    else if(data_direction == ATTR_READ)
    {
      //TODO. read the transfered images bit string from EEPROM and send it
      readAllFirwmareBlockStatus(data,0);
    }
}

/*
 *Image transfer
 *get_image_first_not_transferred_block_number
 */
void get_image_first_not_transferred_block_number(void *data,int data_direction)
{
    //uint8_t *datap =  data;
    if(data_direction == ATTR_WRITE)
    {


    }
    else if(data_direction == ATTR_READ)
    {

      //TODO. read the first block  not transfered from EEPROM and send it
      //write some algorithm to find the first block that is not transfered
      //read a block and mask it with 0xff
      uint8_t data=0,i=0,j=0,z=0;
      uint32_t blocks_count =256;//TODO. get the blocks count
      for(;i<blocks_count;i++)
      {
        readFirwmareBlockStatus(&data,i);

        if(data & 0xff)
        {

        }
        else{
          uint8_t x=1;
          for(;j<8;j++)
          {
            if( data&(x<<j) != 1 )
            {
              data = (i*8) + j;
              z=1;
              break;
            }
          }
          if(z == 1)
          {
            break;
          }
        }
      }

      if(z==1)
      {
        //return > size
      }

    }
}

/*
 * Image transfer
 * image_transfer_initiate method
 */
/**
* the format of the image identifier is
* dv_M_mm where M=>Major release number
*              mm=>Minor release number
* this should be copied to the active firmware version after the new firmware is activated
* and the active firmware version has a lenght of 5
* so when coping ignore the '_' characters i.e
*                                          the active firmware will be just dvMmm
*
* according to the current DCU the image identifier will be transmitted as hex value
* ascii which has 2x the size of the firmware version + null byte
* so the size of the firmware version for our case will be 2*7 + 2 = 16
* Note: the file name of the image withou extension will be used as image identifier so the image file names should be
* named as dv_1_00.hex   for firmware major version 1 and release 0
*/
uint8_t image_ident[16];
uint8_t image_ident_size=16;//let the max be 10
uint32_t image_size=0;
#define max_image_size 122880ul
void image_transfer_initiate(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
    uint8_t type = *data++;//type is 2 struct
    uint8_t size = *data++;//size 2
    image_ident_size = *data++; // octat string
    int i;
    for(i=0;i<image_ident_size;i++) //read image identifier
    {
      image_ident[i] = *data++;
    }
    writeImageIdentifier(image_ident,type);
    uint8_t size_type = *data++;//uint32_t
    //read image size
    uint32_t temp=0;
    temp = *data++;
    image_size  = temp << 24;
    temp = (*data++);
    image_size |= temp<<16;
    temp = (*data++);
    image_size |= temp<<8;
    image_size |= *data++;

    //TODO. do something with the image size and the image identifier
    //write the image identifier to the EEPROM and prepare the EEPROM for image transfer
    //set image transfer started indicator flag
    //if the image size is above our max limit cancel the transfer
    //finally update the "image_transferred_status" variable with "image_transferred_status_t" initiated status
    //if everything is fine so far
    if(image_size>max_image_size)
    {
       *response_len = 0;
       return;
    }
    writeImageSize(image_size,temp);
    EEPROM_reformatImageTransferStatusBytes();
    image_transfer_status = TRANSFER_INITIATED;
    *response_len = 0;
}

/*
 * Image transfer
 * image_block_transfer
 */
uint32_t previous_block_number;
uint32_t current_block_number=0;
uint8_t block_data[64];
/**
* checks for missed block numbers and updates the status to EEPROM
*/
void updateBlocksStatus()
{
  if(current_block_number > previous_block_number+1)
  {
    while(previous_block_number == current_block_number-1)
    {
       ++previous_block_number;
       updatateFirwmareBlockStatus(previous_block_number,0);//set this block as not recived
    }
  }
}


void image_block_transfer(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
    //TODO.if image_transferred_blocks_status is initiated, accept blocks
    //otherwise ignore them
    if(image_transfer_status == TRANSFER_INITIATED)
    {
      uint8_t type = *data++;//type is 2 struct
      uint8_t size = *data++;//size 2

      uint8_t blockNumberType = *data++;//uint32_t

      //read image size
      uint32_t temp=0;
      temp = *data++;
      current_block_number  = temp << 24;
      temp = (*data++);
      current_block_number |= temp<<16;
      temp = (*data++);
      current_block_number |= temp<<8;
      current_block_number |= *data++;

      //the data
      uint8_t block_data_type = *data++;//octate string
      uint8_t _block_size = *data++; // octate string should be 64, image_block_size

      //TODO write them directly to eeprom
      int i;
      for( i=0;i<_block_size;i++) //read block
      {
        block_data[i] = *data++;
      }
      /*
      * Write the data to EEPROM on the appropriate block
      * use the "current_block_number" var as the block number to write
      * figure out a mechanisim to collect failed block numbers for the final blocks retransmision
      */

      writeFirmwareBlock(block_data,current_block_number);
      updateBlocksStatus();
      previous_block_number=current_block_number;
      writePreviousImageNumber(previous_block_number,temp);
      updatateFirwmareBlockStatus(previous_block_number,1);//set this block as recived
    }
    *response_len = 0;
}

/*
 * Image tranfer
 * image_tranfer method
 */
void image_verify(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{
    //verify the iamge and update, may be calculate for image checksum
    //image_transfer_status


    *response_len = 0;

}

/*
 *Image transfer
 *Image image_activate method
 */
void image_activate(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{

    //if image is not verified
      //verify the image and update
    //activate the image.
    //restart and start boot firmware upgrade proccess

    *response_len = 0;
}
