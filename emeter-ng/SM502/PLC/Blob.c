

#include "Blob.h"
#include "PLC_UART.h"
#include "PLC.h"
#include "../Logger.h"

Blob_Message_t readBlobMessage;
Blob_Message_t writeBlobMessage;

void ProcessReadBlob(Blob_Message_t *message)
{
//  uint8_t data[] ={ 0x96,0xC0, 0x04, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00};

    uint32_t blob;
    getBlob(&blob,message->blob_id); 
    blob=0;
    uint8_t data[] ={ 0x96,0xC0, 0x08, 0x00, message->blob_id, (message->blob_id>>8), 0x04, 0x00, (blob), (blob>>8), (blob>>16), (blob>>24)};
    
    int i=0;
     for(;i<12;i++)
     {
        AddtoTXBuffer(data[i]);
     }
     PLC_Start_SendBuffer();
    
}

void ProcessWriteBlob(Blob_Message_t *message)
{
     uint8_t data[] ={ 0x97,0xC0, 0x02, 0x00,00,00};
     int i=0;
     uint32_t blob=0;
     uint32_t temp=0;
            blob = *message->blobdata++;
            temp = ((uint16_t)(*message->blobdata++)<<8);
            blob |=temp;
            temp = ((uint32_t)(*message->blobdata++)<<16);
            blob|=temp;
            temp = ((uint32_t)(*message->blobdata++)<<24);
            blob|=temp;
          
     setBlob(&blob,message->blob_id);
     for(;i<6;i++)
     {
        AddtoTXBuffer(data[i]);
     }
     PLC_Start_SendBuffer();
}