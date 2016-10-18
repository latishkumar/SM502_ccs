
#include <stddef.h>
#include "IEC62056/IEC62056_UART.h"

    //_STD_BEGIN
   // __no_init volatile unsigned char lcdIO @ 0xD2;
    //int lcdIO=0;
    //#pragma module_name = "?__write"

size_t __write(int handle, const unsigned char* buf, size_t bufSize)
{
    size_t nChars = 0;
    /* Check for the command to flush all handles */
    if (handle == -1)
    {
      return 0;
    }
    /* Check for stdout and stderr
    (only necessary if FILE descriptors are enabled.) */
    if (handle != 1 && handle != 2)
    {
      return -1;
    }
    for (/* Empty */; bufSize > 0; --bufSize)
    {
      IEC_AddtoTXBuffer(*buf);
      //queue_enqueue(iec_tx_buff,buf);
      //lcdIO = *buf;
      ++buf;
      ++nChars;
    }
    IEC_Start_SendBuffer();
    return nChars;
}

    //_STD_END