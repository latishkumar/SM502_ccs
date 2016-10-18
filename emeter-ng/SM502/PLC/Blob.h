

#ifndef _BLOB_H_
#define _BLOB_H_
#include <stdint.h>

/**
* Read TI host message specification
*/

typedef struct {
  
  uint16_t blob_id;
  uint16_t blob_length;
  uint8_t *blobdata;
  
}Blob_Message_t;



extern Blob_Message_t readBlobMessage;
extern Blob_Message_t writeBlobMessage;

void ProcessReadBlob(Blob_Message_t *message);
void ProcessWriteBlob(Blob_Message_t *message);


#endif 