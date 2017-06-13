//--------------------------------------------------------------------------
//
//  Software for MSP430 based e-meters.
//
//  THIS PROGRAM IS PROVIDED "AS IS". TI MAKES NO WARRANTIES OR
//  REPRESENTATIONS, EITHER EXPRESS, IMPLIED OR STATUTORY, 
//  INCLUDING ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS 
//  FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR 
//  COMPLETENESS OF RESPONSES, RESULTS AND LACK OF NEGLIGENCE. 
//  TI DISCLAIMS ANY WARRANTY OF TITLE, QUIET ENJOYMENT, QUIET 
//  POSSESSION, AND NON-INFRINGEMENT OF ANY THIRD PARTY 
//  INTELLECTUAL PROPERTY RIGHTS WITH REGARD TO THE PROGRAM OR 
//  YOUR USE OF THE PROGRAM.
//
//  IN NO EVENT SHALL TI BE LIABLE FOR ANY SPECIAL, INCIDENTAL, 
//  CONSEQUENTIAL OR INDIRECT DAMAGES, HOWEVER CAUSED, ON ANY 
//  THEORY OF LIABILITY AND WHETHER OR NOT TI HAS BEEN ADVISED 
//  OF THE POSSIBILITY OF SUCH DAMAGES, ARISING IN ANY WAY OUT 
//  OF THIS AGREEMENT, THE PROGRAM, OR YOUR USE OF THE PROGRAM. 
//  EXCLUDED DAMAGES INCLUDE, BUT ARE NOT LIMITED TO, COST OF 
//  REMOVAL OR REINSTALLATION, COMPUTER TIME, LABOR COSTS, LOSS 
//  OF GOODWILL, LOSS OF PROFITS, LOSS OF SAVINGS, OR LOSS OF 
//  USE OR INTERRUPTION OF BUSINESS. IN NO EVENT WILL TI'S 
//  AGGREGATE LIABILITY UNDER THIS AGREEMENT OR ARISING OUT OF 
//  YOUR USE OF THE PROGRAM EXCEED FIVE HUNDRED DOLLARS 
//  (U.S.$500).
//
//  Unless otherwise stated, the Program written and copyrighted 
//  by Texas Instruments is distributed as "freeware".  You may, 
//  only under TI's copyright in the Program, use and modify the 
//  Program without any charge or restriction.  You may 
//  distribute to third parties, provided that you transfer a 
//  copy of this license to the third party and the third party 
//  agrees to these terms by its first use of the Program. You 
//  must reproduce the copyright notice and any other legend of 
//  ownership on each copy or partial copy, of the Program.
//
//  You acknowledge and agree that the Program contains 
//  copyrighted material, trade secrets and other TI proprietary 
//  information and is protected by copyright laws, 
//  international copyright treaties, and trade secret laws, as 
//  well as other intellectual property laws.  To protect TI's 
//  rights in the Program, you agree not to decompile, reverse 
//  engineer, disassemble or otherwise translate any object code 
//  versions of the Program to a human-readable form.  You agree 
//  that in no event will you alter, remove or destroy any 
//  copyright notice included in the Program.  TI reserves all 
//  rights not specifically granted under this license. Except 
//  as specifically provided herein, nothing in this agreement 
//  shall be construed as conferring by implication, estoppel, 
//  or otherwise, upon you, any license or other right under any 
//  TI patents, copyrights or trade secrets.
//
//  You may not use the Program in non-TI devices.
//
//  File: server_msgs.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: server_msgs.c,v 1.22 2013/01/30 05:36:32 a0393626 Exp $
//
//
//--------------------------------------------------------------------------
//
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "cosem.h"
#include "obis.h"
#include "iec62056_46_link.h"
#include "msgs.h"
#include "packing.h"
#include "config.h"
#include "server_msgs.h"
#include "EventTypes.h"
#include "DLMS.h"
#include "Logger.h"
#define FALSE 0
#define TRUE (!FALSE)

int8_t access_selector;
uint32_t SA_From_Entry=0, SA_To_Entry=0;
sSA_Range SA_Range[2];
uint16_t SA_From_Column=0, SA_To_Column=0;
uint16_t SA_Range_Num_Columns=0;
//const uint8_t common_expanded_aes_gcm_key[176];
extern async_hdlc_tx_t tx[];
extern uint16_t Chunk_Size;


async_hdlc_tx_t queued_tx =
{
    {
        0
    },
    0,
    0,
    0,
};

static iec62056_46_link_t *link_xxx = NULL;

#if defined(IEC62056_SUPPORT_AES_GCM_128)
static uint8_t local_iv[12];
static uint8_t far_iv[12];
#endif

extern const uint8_t acceptable_conformance[4][3];

#include <time.h>

void exchange_authentication_mechanism(void *data, int direction)
{
    uint8_t *s;
    
    s = (uint8_t *) data;
    
    s[0] = 7;
    s[1] = 0x60;
    s[2] = 0x85;
    s[3] = 0x74;
    s[4] = 0x05;
    s[5] = 0x08;
    s[6] = 0x02;
    switch (link_xxx->association)
    {
    case 0:
        /* Use lowest level */
        s[7] = 0x00;
        break;
    case 1:
        /* Use low level */
        s[7] = 0x01;
        break;
    case 2:
        /* Use AES-GCM-128 */
        s[7] = 0x07;
        break;
    case 3:
        /* Use AES-GCM-128 */
        s[7] = 0x07;
        break;
        
    }
}
/*- End of function --------------------------------------------------------*/

void exchange_partner(void *data, int direction)
{
    uint8_t *x;
    
    x = (uint8_t *) data;
    
    x[3] = link_xxx->far_msap >> 14;    /* Client SAP */
    x[5] = 0;                           /* Server SAP */
    x[6] = 1;                           /* Server SAP */
}
/*- End of function --------------------------------------------------------*/

void exchange_date_time(void *data, int direction)
{
#if 0
    time_t now;
#endif
    struct tm *tm;
    struct tm tmx;
    uint8_t *x;
    int y;

    if (direction == ATTR_READ)
    {
#if 0
        now = time(NULL);
        tm = localtime(&now);
        x = (uint8_t *) data;
        y = tm->tm_year + 1900;
        x[0] = 12;
        unpack16(&x[1], y);
        x[3] = tm->tm_mon + 1;
        x[4] = tm->tm_mday;
        x[5] = tm->tm_wday + 1;
        x[6] = tm->tm_hour;
        x[7] = tm->tm_min;
        x[8] = tm->tm_sec;
        x[9] = 0xFF;
        y = 120;
        unpack16(&x[10], y);
        x[12] = 0x00;
        printf("Reading clock as %d/%02d/%02d %02d:%02d:%02d\n",
               tm->tm_year + 1900,
               tm->tm_mon + 1,
               tm->tm_mday,
               tm->tm_hour,
               tm->tm_min,
               tm->tm_sec);
#endif
    }
    else
    {
        x = (uint8_t *) data;
        tm = &tmx;
        y = pack16(&x[1]);
        tm->tm_year = y - 1900;
        tm->tm_mon = x[3] - 1;
        tm->tm_mday = x[4];
        tm->tm_wday = x[5] - 1;
        tm->tm_hour = x[6];
        tm->tm_min = x[7];
        tm->tm_sec = x[8];
        y = pack16(&x[10]);
/*        printf("Setting clock to %d/%02d/%02d %02d:%02d:%02d\n",
               tm->tm_year + 1900,
               tm->tm_mon + 1,
               tm->tm_mday,
               tm->tm_hour,
               tm->tm_min,
               tm->tm_sec);*/
    }
    
}
/*- End of function --------------------------------------------------------*/

static uint8_t negotiated_conformance[3];

static int last_start = 0;

int interpret_and_fill(uint8_t *to, const uint8_t *from, int len)
{
    uint8_t *tox;
    const uint8_t *fromx;
    int64_t val; //maynot be supported in our architecture
    int type;
    int length;
    
    tox = (uint8_t *) to;
    fromx = (const uint8_t *) from;

    while (tox < to + len)
    {
        type = *fromx;
        //printf("Type %d (%p %p %p %p)\n", type, tox, to + len, fromx, from + len);
        /* Find how long this item will be, and then check if the whole item will
           fit in the current chunk */
        length = 1;
        switch (type & 0x7f)
        {
        case TAG_NULLDATA:
            break;
        case TAG_BITSTRING:
            break;
        case TAG_OCTET_STRING:
        case TAG_VISIBLE_STRING:
            length = 2 + fromx[1];
            break;
        case TAG_GENERALIZED_TIME:
            break;
        case TAG_BCD:
            break;
        case TAG_ARRAY:
        case TAG_STRUCTURE:
        case TAG_BOOLEAN:
        case TAG_INT8:
        case TAG_UINT8:
        case TAG_ENUM:
            length = 2;
            break;
        case TAG_INT16:
        case TAG_UINT16:
            length = 3;
            break;
        case TAG_FLOATING_POINT:
        case TAG_INT32:
        case TAG_UINT32:
        case TAG_FLOAT32:
            length = 5;
            break;
        case TAG_INT64:
        case TAG_UINT64:
        case TAG_FLOAT64:
            length = 9;
            break;
        case TAG_DATETIME:
        case TAG_DATE:
        case TAG_TIME:
        case TAG_DONTCARE:
        default:
            break;
        }
        if (tox + length > to + len)
        {
            /* This won't fit */
            break;
        }
        *tox++ = *fromx++ & 0x7F;
        if ((type & 0x80))
        {
            /* This value needs to be inserted */
            switch (type & 0x7F)
            {
            case TAG_NULLDATA:
                break;
             case TAG_STRUCTURE:
                if(access_selector==2 && SA_From_Column !=0 && SA_To_Column !=0 && tox==(to+1))
                {
                  *tox++ = (SA_To_Column-SA_From_Column+1);
                  fromx++;
                }  
                else if(access_selector==1 && SA_Range_Num_Columns > 0)
                {
                  *tox++ = SA_Range_Num_Columns;
                  fromx++;
                }  
                else
                {
                *tox++ = *fromx++;
                }
                break;
            case TAG_ARRAY:   
              *tox++ = *fromx++;
              break;
            case TAG_BITSTRING:
                /* TODO */
                break;
            case TAG_FLOATING_POINT:
                break;
            case TAG_OCTET_STRING:
            case TAG_VISIBLE_STRING:
                length = fromx[0];
                *tox++ = *fromx++;
                if(tox==(uint8_t *)0x17b9)
                {
                  __asm__ __volatile__ (" NOP");
                }  
                get_string_item(tox, length, *fromx);
                tox += length;
                fromx += length;
                break;
            case TAG_GENERALIZED_TIME:
                break;
            case TAG_BCD:
                break;
            case TAG_BOOLEAN:
            case TAG_INT8:
            case TAG_UINT8:
            case TAG_ENUM:
                *tox++ = get_numeric_item(*fromx++);
                break;
            case TAG_INT16:
            case TAG_UINT16:
                fromx++;
                val = get_numeric_item(*fromx++);
                *tox++ = (val >> 8) & 0xFF;
                *tox++ = val & 0xFF;
                break;
            case TAG_INT32:
            case TAG_UINT32:
            case TAG_FLOAT32:
                fromx += 3;
                val = get_numeric_item(*fromx++);
                *tox++ = (val >> 24) & 0xFF;
                *tox++ = (val >> 16) & 0xFF;
                *tox++ = (val >> 8) & 0xFF;
                *tox++ = val & 0xFF;
                break;
            case TAG_INT64:
            case TAG_UINT64:
            case TAG_FLOAT64:
                /* TODO: This isn't really dealing with a 64 bit value */
                fromx += 7;
                val = get_numeric_item(*fromx++);
                *tox++ = (val >> 56) & 0xFF;;
                *tox++ = (val >> 48) & 0xFF;;
                *tox++ = (val >> 40) & 0xFF;;
                *tox++ = (val >> 32) & 0xFF;;
                *tox++ = (val >> 24) & 0xFF;
                *tox++ = (val >> 16) & 0xFF;
                *tox++ = (val >> 8) & 0xFF;
                *tox++ = val & 0xFF;
                break;
            case TAG_DATETIME:
            case TAG_DATE:
            case TAG_TIME:
            case TAG_DONTCARE:
            default:
                break;
            }
        }
        else
        {
            /* This value just needs to be copied over */
            switch (type & 0x7F)
            {
            case TAG_NULLDATA:
                break;
            case TAG_BITSTRING:
                /* TODO */
                break;
            case TAG_INT64:
            case TAG_UINT64:
            case TAG_FLOAT64:
                *tox++ = *fromx++;
                *tox++ = *fromx++;
                *tox++ = *fromx++;
                *tox++ = *fromx++;
                /* Fall through */
            case TAG_INT32:
            case TAG_UINT32:
            case TAG_FLOAT32:
                *tox++ = *fromx++;
                *tox++ = *fromx++;
                /* Fall through */
            case TAG_INT16:
            case TAG_UINT16:
                *tox++ = *fromx++;
                /* Fall through */
            case TAG_ARRAY:
            case TAG_STRUCTURE:
            case TAG_BOOLEAN:
            case TAG_INT8:
            case TAG_UINT8:
            case TAG_ENUM:
                *tox++ = *fromx++;
                break;
            case TAG_FLOATING_POINT:
                break;
            case TAG_OCTET_STRING:
            case TAG_VISIBLE_STRING:
                length = fromx[0];
                memcpy(tox, fromx, length + 1);
                tox += (length + 1);
                fromx += (length + 1);
                break;
            case TAG_GENERALIZED_TIME:
                break;
            case TAG_BCD:
                break;
            case TAG_DATETIME:
            case TAG_DATE:
            case TAG_TIME:
            case TAG_DONTCARE:
            default:
                break;
            }
        }
    }
#if defined(LOG_PACKETS)    
printf("XXT fromx len %d\n", fromx - from);
fflush(stdout);
#endif
    //printf("Done - %d\n", fromx - from);
    return fromx - from;    
}    

uint8_t setblock_residue_len=0;
uint8_t *template_pointer;

void compare_and_retrieve(uint8_t *write_data, int write_len)
{
    uint8_t *data;
    int type;
    int length, len;
 // Fromx is the data passed by Client, Tox is the data from the template   
      data = write_data;  
      len = write_len;
      int i;
    while (len)
    {
        type = *template_pointer; // It is assumed that the data and template data matches

        /* Find how long this item will be, and then check if the whole item will
           fit in the current chunk */
        length = 1;
        switch (type & 0x7f)
        {
        case TAG_NULLDATA:
            break;
        case TAG_BITSTRING:
            break;
        case TAG_OCTET_STRING:
        case TAG_VISIBLE_STRING:
            length = 2 + data[1];
            break;
        case TAG_GENERALIZED_TIME:
            break;
        case TAG_BCD:
            break;
        case TAG_ARRAY:
        case TAG_STRUCTURE:
        case TAG_BOOLEAN:
        case TAG_INT8:
        case TAG_UINT8:
        case TAG_ENUM:
            length = 2;
            break;
        case TAG_INT16:
        case TAG_UINT16:
            length = 3;
            break;
        case TAG_FLOATING_POINT:
        case TAG_INT32:
        case TAG_UINT32:
        case TAG_FLOAT32:
            length = 5;
            break;
        case TAG_INT64:
        case TAG_UINT64:
        case TAG_FLOAT64:
            length = 9;
            break;
        case TAG_DATETIME:
        case TAG_DATE:
        case TAG_TIME:
        case TAG_DONTCARE:
        default:
            break;
        }
        if (length > len)
        {
            /* This won't fit */
            setblock_residue_len=len;
            break;
        }
        else
        {
           setblock_residue_len=0;
        }   
        data++;
        template_pointer++; 
        len-=length;
            /* This value needs to be inserted */
            switch (type & 0x7F)
            {
            case TAG_NULLDATA:
                break;
             case TAG_STRUCTURE:
             case TAG_ARRAY:   
                    data++;
                    template_pointer++; 
                break;
            case TAG_BITSTRING:
                /* TODO */
                break;
            case TAG_FLOATING_POINT:
                break;
            case TAG_OCTET_STRING:
            case TAG_VISIBLE_STRING:
                length = data[0];
                data++;
                template_pointer++; 
                if (type & 0x80) set_string_item(*template_pointer, data, length);
                template_pointer += length;
                data += length;
                break;
            case TAG_GENERALIZED_TIME:
                break;
            case TAG_BCD:
                break;
            case TAG_BOOLEAN:
            case TAG_INT8:
            case TAG_UINT8:
            case TAG_ENUM:
                if (type & 0x80) 
                  set_numeric_item(*template_pointer,data,1);
                template_pointer+=1;
                data+=1;
                break;
            case TAG_INT16:
            case TAG_UINT16:
                template_pointer++;
                if (type & 0x80) 
                  set_numeric_item(*template_pointer,data,2);
                template_pointer+=1;
                data+=2;
                break;
            case TAG_INT32:
            case TAG_UINT32:
            case TAG_FLOAT32:
                template_pointer+=3;
                if (type & 0x80) set_numeric_item(*template_pointer,data,4);
                template_pointer+=1;
                data+=4;
                break;
            case TAG_INT64:
            case TAG_UINT64:
            case TAG_FLOAT64:
                /* TODO: This isn't really dealing with a 64 bit value */
                template_pointer+=7;
                if (type & 0x80) set_numeric_item(*template_pointer,data,8);
                template_pointer+=1;
                data+=8;
                break;
            case TAG_DATETIME:
            case TAG_DATE:
            case TAG_TIME:
            case TAG_DONTCARE:
            default:
                break;
            }
       if(template_pointer >= msg_info.template + msg_info.sz_template)
        {
          template_pointer = (uint8_t *)msg_info.template;
          msg_info.entries_remaining--;
        }

     }
     for(i=0;i<setblock_residue_len;i++)
     {
       *(write_data+i) = *(data+i);
     }  
}
    

    

/*- End of function --------------------------------------------------------*/



static int type_copy(void *to, const void *from, uint8_t type)
{
    uint8_t *tox;
    const uint8_t *fromx;
    int length;
    
    tox = (uint8_t *) to;
    fromx = (const uint8_t *) from;

    switch (type)
    {
    case TAG_NULLDATA:
        return 0;
    case TAG_ARRAY:
      if ((fromx[0] & 0x80))
        {
            length = ((fromx[0] & 0x7F) << 8) | fromx[1];
            //memcpy(to, ((uint8_t *) from) + 2, length);
        }
        else
        {
            length = fromx[0];
            //memcpy(to, ((uint8_t *) from) + 1, length);
        }
       return length;
    case TAG_STRUCTURE:
            length = fromx[0];
            memcpy(to, ((uint8_t *) from) + 1, length);
        return length;
    case TAG_BOOLEAN:
    case TAG_INT8:
    case TAG_UINT8:
    case TAG_ENUM:
        tox[0] = fromx[0];
        return 1;
    case TAG_BITSTRING:
        return 0;
    case TAG_INT32:
    case TAG_UINT32:
    case TAG_FLOAT32:
        tox[0] = fromx[3];
        tox[1] = fromx[2];
        tox[2] = fromx[1];
        tox[3] = fromx[0];
        return 4;
    case TAG_FLOATING_POINT:
        return 0;
    case TAG_OCTET_STRING:
    case TAG_VISIBLE_STRING:
        length = fromx[0];
        memcpy(to, from, length + 1);
        return length + 1;
    case TAG_GENERALIZED_TIME:
        return 0;
    case TAG_BCD:
        return 0;
    case TAG_INT16:
    case TAG_UINT16:
        tox[0] = fromx[1];
        tox[1] = fromx[0];
        return 2;
    case TAG_INT64:
    case TAG_UINT64:
    case TAG_FLOAT64:
        tox[0] = fromx[7];
        tox[1] = fromx[6];
        tox[2] = fromx[5];
        tox[3] = fromx[4];
        tox[4] = fromx[3];
        tox[5] = fromx[2];
        tox[6] = fromx[1];
        tox[7] = fromx[0];
        return 8;
    case TAG_DATETIME:
    case TAG_DATE:
    case TAG_TIME:
    case TAG_DONTCARE:
    default:
        return 0;
    }
}
/*- End of function --------------------------------------------------------*/

static int locate_object_attr(iec62056_46_link_t *link,
                              uint16_t class_id,
                              const uint8_t instance_id[6],
                              uint8_t attribute_id,
                              uint8_t access_selector,
                              int *element)
{
    int i;
    int j;

    for (i = 0;  object_list[i].attr_elements;  i++)
    { 
      if(object_list[i].assoc & (0x01<<(link->association)))
      { if (object_list[i].class_id == class_id)
         {
           if (((instance_id[0] == object_list[i].instance_id[0] )  && (instance_id[1] == object_list[i].instance_id[1] )
                && (instance_id[2] == object_list[i].instance_id[2] ) && (instance_id[3] == object_list[i].instance_id[3] )
                && (instance_id[4] == object_list[i].instance_id[4] ) && (instance_id[5] == object_list[i].instance_id[5] )))                                         
                        
                  break;
        }
      }  
    }
    if (object_list[i].attr_elements)
    {
        /* We found a match for the object */
        if (object_list[i].attrs != NULL)
        {
            if (class_id == CLASS_ID_ASSOCIATION_LN  &&  attribute_id == 2)
            {
                /* This is a special case, to be handled by a block by block response */
                *element = -1;
                return i;
            }
            for (j = 0;  j < object_list[i].attr_elements;  j++)
            {
                if ((object_list[i].attrs[j].id & 0x3F) == attribute_id)
                    break;
            }
            if (j < object_list[i].attr_elements  &&  object_list[i].attrs[j].addr)
            {
                *element = j;
                return i;
            }
        }
    }
    return -1;
}
/*- End of function --------------------------------------------------------*/
    
static int locate_object_method(iec62056_46_link_t *link,
                                uint16_t class_id,
                                const uint8_t instance_id[6],
                                uint8_t method_id,
                                int *element)
{
    int i;
    int j;

    for (i = 0;  object_list[i].attr_elements;  i++)
    {
      if(object_list[i].assoc & (0x01<<(link->association)))
      { if (object_list[i].class_id == class_id)
        {
           if (((instance_id[0] == object_list[i].instance_id[0] )  && (instance_id[1] == object_list[i].instance_id[1] )
                && (instance_id[2] == object_list[i].instance_id[2] ) && (instance_id[3] == object_list[i].instance_id[3] )
                && (instance_id[4] == object_list[i].instance_id[4] ) && (instance_id[5] == object_list[i].instance_id[5] )))
                                                                

                  break;
               
        }
      }
    }
    if (object_list[i].attr_elements)
    {
        /* We found a match for the object */
        if (object_list[i].methods != NULL)
        {
            for (j = 0;  j < object_list[i].method_elements;  j++)
            {
                if (object_list[i].methods[j].id == method_id)
                    break;
            }
            if (j < object_list[i].method_elements  &&  object_list[i].methods[j].callback)
            {
                *element = j;
                return i;
            }
        }
    }
    return -1;
}
/*- End of function --------------------------------------------------------*/

static int extract_cosem_field(const uint8_t msg[], int len, int *depth, int qty[])
{
    int length;

    len = 0;
    switch (msg[0])
    {
    case TAG_NULLDATA:
        len = 1;
        break;
    case TAG_ARRAY:
        (*depth)++;
        qty[*depth] = 0x8000 | (msg[1] + 1);
        len = 1 + 1;
        break;
    case TAG_STRUCTURE:
        (*depth)++;
        qty[*depth] = msg[1] + 1;
        len = 1 + 1;
        break;
    case TAG_BOOLEAN:
        len = 1 + 1;
        break;
    case TAG_BITSTRING:
        length = msg[1];
        len = 1 + 1 + ((length + 7) >> 3);
        break;
    case TAG_INT32:
        len = 1 + 4;
        break;
    case TAG_UINT32:
        len = 1 + 4;
        break;
    case TAG_FLOATING_POINT:
        len = 1 + 4;
        break;
    case TAG_OCTET_STRING:
        length = msg[1];
        len = 1 + 1 + length;
        break;
    case TAG_VISIBLE_STRING:
        length = msg[1];
        len = 1 + 1 + length;
        break;
    case TAG_GENERALIZED_TIME:
        length = msg[1];
        len = 1 + 1 + length;
        break;
    case TAG_BCD:
        len = 1 + 1;
        break;
    case TAG_INT8:
        len = 1 + 1;
        break;
    case TAG_INT16:
        len = 1 + 2;
        break;
    case TAG_UINT8:
        len = 1 + 1;
        break;
    case TAG_UINT16:
        len = 1 + 2;
        break;
    case TAG_COMPACT_ARRAY:
        (*depth)++;
        qty[*depth] = 0x8000 | (msg[1] + 1);
        len = 1 + 1;
        break;
    case TAG_INT64:
        len = 1 + 8;
        break;
    case TAG_UINT64:
        len = 1 + 8;
        break;
    case TAG_ENUM:
        len = 1 + 1;
        break;
    case TAG_FLOAT32:
        len = 1 + 4;
        break;
    case TAG_FLOAT64:
        len = 1 + 8;
        break;
    case TAG_DATETIME:
        len = 1 + 12;
        break;
    case TAG_DATE:
        len = 1 + 5;
        break;
    case TAG_TIME:
        len = 1 + 4;
        break;
    case TAG_DONTCARE:
        len = 1;
        break;
    default:
        len = 1;
        break;
    }
    while (qty[*depth])
    {
        qty[*depth] = (qty[*depth] & 0x8000) | ((qty[*depth] & 0xFF) - 1);
        if ((qty[*depth] & 0xFF) != 0)
            break;
        --(*depth);
    }
    return len;
}
/*- End of function --------------------------------------------------------*/

static int extract_ber_field(const uint8_t msg[], int len)
{
#if defined(LOG_PACKETS)
    int i;
#endif
    int length;
//    int length2;
#if defined(LOG_PACKETS)
    int value;
#endif

    switch (msg[0])
    {
    case TAG_UNIVERSAL_EOC:
        break;
    case TAG_UNIVERSAL_BOOLEAN:
        break;
    case TAG_UNIVERSAL_INTEGER:
        length = msg[1];
#if defined(LOG_PACKETS)
        printf("Integer %d - ", msg[1]);
#endif
//        length2 = length + 2;
#if defined(LOG_PACKETS)
        for (i = 2, value = 0;  i < length2;  i++)
            value = (value << 8) | msg[i];
        printf("%d\n", value);
#endif
        return length + 1;
    case TAG_UNIVERSAL_BIT_STRING:
        break;
    case TAG_UNIVERSAL_OCTET_STRING:
        length = msg[1];
#if defined(LOG_PACKETS)
        printf("Octet string %d - ", msg[1]);
#endif
//        length2 = length + 2;
#if defined(LOG_PACKETS)
        for (i = 2;  i < length2;  i++)
            printf("%02X ", msg[i]);
        printf("\n");
#endif
        return length + 1;
    case TAG_UNIVERSAL_NULL:
        break;
    case TAG_UNIVERSAL_OBJECT_IDENTIFIER:
        length = msg[1];
#if defined(LOG_PACKETS)
        printf("Object identifier %d - ", msg[1]);
#endif
//        length2 = length + 2;
#if defined(LOG_PACKETS)
        for (i = 2;  i < length2;  i++)
            printf("%02X ", msg[i]);
        printf("\n");
#endif
        return length + 1;
    case TAG_UNIVERSAL_OBJECT_DESCRIPTOR:
        length = msg[1];
#if defined(LOG_PACKETS)
        printf("Object descriptor %d - ", msg[1]);
#endif
//        length2 = length + 2;
#if defined(LOG_PACKETS)
        for (i = 2;  i < length2;  i++)
            printf("%02X ", msg[i]);
        printf("\n");
#endif
        return length + 1;
    case TAG_UNIVERSAL_EXTERNAL:
    case TAG_UNIVERSAL_REAL:
    case TAG_UNIVERSAL_ENUMERATED:
    case TAG_UNIVERSAL_EMBEDDED_PDV:
    case TAG_UNIVERSAL_UTF8STRING:
    case TAG_UNIVERSAL_RELATIVE_OID:
    case TAG_UNIVERSAL_SEQUENCE:
    case TAG_UNIVERSAL_SET:
    case TAG_UNIVERSAL_NUMERIC_STRING:
    case TAG_UNIVERSAL_PRINTABLE_STRING:
    case TAG_UNIVERSAL_T61_STRING:
    case TAG_UNIVERSAL_VIDEOTEX_STRING:
    case TAG_UNIVERSAL_IA5_STRING:
    case TAG_UNIVERSAL_UTC_TIME:
    case TAG_UNIVERSAL_GENERALIZED_TIME:
    case TAG_UNIVERSAL_GRAPHIC_STRING:
    case TAG_UNIVERSAL_VISIBLE_STRING:
    case TAG_UNIVERSAL_GENERAL_STRING:
    case TAG_UNIVERSAL_UNIVERSAL_STRING:
    case TAG_UNIVERSAL_CHARACTER_STRING:
    case TAG_UNIVERSAL_BMPSTRING:
    default:
#if defined(LOG_PACKETS)
        printf("Unknown tag %d\n", msg[0]);
#endif
        break;
    }
    return 0;
}
/*- End of function --------------------------------------------------------*/

int extract_cosem_asn1(const uint8_t msg[], int len)
{
    int i;
    int depth;
    int qty[100];

    depth = 0;
    qty[0] = 0;
    i = 0;
    do
    {
        i += extract_cosem_field(&msg[i], len - i, &depth, qty);
    }
    while (i < len  &&  depth != 0);
    return i;
}
/*- End of function --------------------------------------------------------*/

int xxxx(int type)
{
    switch (type)
    {
    case TAG_AUTHENTICATION_INFORMATION_COMPONENT:
#if defined(LOG_PACKETS)
        printf("Authentication information component\n");
#endif
        break;
    case TAG_RESPONDER_ACSE_REQUIREMENTS_FIELD_COMPONENT:
#if defined(LOG_PACKETS)
        printf("Responder ACSE requirements field component\n");
#endif
        break;
    case TAG_RESPONDER_MECHANISM_NAME_COMPONENT:
#if defined(LOG_PACKETS)
        printf("Responder mechanism name component\n");
#endif
        break;
    case TAG_ACSE_REQUIREMENTS_FIELD_COMPONENT:
#if defined(LOG_PACKETS)
        printf("ACSE requirements field component\n");
#endif
        break;
    case TAG_MECHANISM_NAME_COMPONENT:
#if defined(LOG_PACKETS)
        printf("Mechanism name component\n");
#endif
        break;
    default:
        return -1;
    }
    return 0;
}
/*- End of function --------------------------------------------------------*/

static int check_context_name(const uint8_t msg[], int length, int *context_id)
{
    if (length != 7)
        return -1;
    /* Check the country code and name */
    if (msg[0] != 0x60  ||  msg[1] != 0x85  ||  msg[2] != 0x74)
        return -1;
    /* Check the organisation code is the DLMS UA */
    if (msg[3] != 0x05  ||  msg[4] != 0x08)
        return -1;
    /* So, we seem to have something for us. Return the application context and the context ID */
    *context_id = msg[6];
    return msg[5];
}
/*- End of function --------------------------------------------------------*/
int security_level;
#define YES 0xad
#define NO 0xbc
uint16_t challenge_c2s_len,challenge_s2c_len;
uint8_t challenge_c2s[16],reply_c2s[16];
uint8_t challenge_s2c[16]={'T','e','x','a','s','I','n','s','t','r','u','m','e','n','t','s'};
uint8_t Authorised = NO;

//uint8_t AARE_HLS_Challenge[]={'T','e','x','a','s','I','n','s','t','r','u','m','e','n','t','s'};

void send_aare(iec62056_46_link_t *link, const uint8_t context[], const uint8_t conformance[], int status)
{
    //async_hdlc_tx_t tx;

  static uint8_t xxxx_HLS[76] =
    {
        PDU_AARE, 74,
            AARE_APDU_TAG_APPLICATION_CONTEXT_NAME, 9,
                TAG_UNIVERSAL_OBJECT_IDENTIFIER, 7,
                    0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x01,
            AARE_APDU_TAG_RESULT,
               0x03,0x02,0x01,0x00,
            AARE_APDU_TAG_RESULT_SOURCE_DIAGNOSTIC, 5,
                AARE_APDU_TAG_APPLICATION_CONTEXT_NAME, 3,
                    TAG_UNIVERSAL_INTEGER, 1,
                        0x0e,
            AARE_APDU_TAG_RESPONDER_ACSE_REQUIREMENTS,
                        0x02,0x07,0x80,   
            AARE_APDU_TAG_MECHANISM_NAME,
              0x07,0x60,0x85,0x74,0x05,0x08,0x02,0x02,            
            AARE_APDU_TAG_RESPONDING_AUTHENTICATION_VALUE,
            18, 0x80, 16, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            AARE_APDU_TAG_USER_INFORMATION, 16,
                TAG_UNIVERSAL_OCTET_STRING, 14,
                    PDU_INITIATE_RESPONSE, 0x00, 0x06,
                        0x5F, 0x1F,
                        0x04, 0x00, 0x00, 0x30, 0x1D,
                        0x00, 0x64,
                        0x00, 0x07
   };
  
  static uint8_t xxxx_LLS_NS[43] =
    {
        PDU_AARE, 41,
            AARE_APDU_TAG_APPLICATION_CONTEXT_NAME, 9,
                TAG_UNIVERSAL_OBJECT_IDENTIFIER, 7,
                    0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x01,
            AARE_APDU_TAG_RESULT,
               0x03,0x02,0x01,0x00,
            AARE_APDU_TAG_RESULT_SOURCE_DIAGNOSTIC, 5,
                AARE_APDU_TAG_APPLICATION_CONTEXT_NAME, 3,
                    TAG_UNIVERSAL_INTEGER, 1,
                        0x00,
            AARE_APDU_TAG_USER_INFORMATION, 16,
                TAG_UNIVERSAL_OCTET_STRING, 14,
                    PDU_INITIATE_RESPONSE, 0x00, 0x06,
                        0x5F, 0x1F,
                        0x04, 0x00, 0x00, 0x30, 0x1D,
                        0x00, 0x64,
                        0x00, 0x07
   };
  
    static uint8_t xxxx1[] =
    {
        PDU_AARE, 31,
            AARE_APDU_TAG_APPLICATION_CONTEXT_NAME, 9,
                TAG_UNIVERSAL_OBJECT_IDENTIFIER, 7,
                    0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x01,
            AARE_APDU_TAG_RESULT, 3,
                TAG_UNIVERSAL_INTEGER, 1,
                    0,       // Result - accepted
            AARE_APDU_TAG_RESULT_SOURCE_DIAGNOSTIC, 5,
                0xA1, 3,
                    TAG_UNIVERSAL_INTEGER, 1,
                        0,
            AARE_APDU_TAG_USER_INFORMATION, 6,
                TAG_UNIVERSAL_OCTET_STRING, 4,
                    PDU_CONFIRMED_SERVICE_ERROR, CONFIRMED_SERVICE_ERROR_INITIATEERROR, SERVICE_ERROR_INITIATE, 1
    };

#if defined(LOG_PACKETS)
    printf("send AARE\n");
#endif
    if(security_level==0 || security_level==1) {
    negotiated_conformance[0] = 
    xxxx_LLS_NS[36] = conformance[0] & acceptable_conformance[link->association][0];
    negotiated_conformance[1] = 
    xxxx_LLS_NS[37] = conformance[1] & acceptable_conformance[link->association][1];
    negotiated_conformance[2] = 
    xxxx_LLS_NS[38] = conformance[2] & acceptable_conformance[link->association][2];
    if(link->association==0)
    {
      xxxx_LLS_NS[38]&=~0x04;
    }  
    if (xxxx_LLS_NS[36] == 0  &&  xxxx_LLS_NS[37] == 0  &&  xxxx_LLS_NS[38] == 0)
    {
        status = 0x11;
    }
    
    }
    else{
        negotiated_conformance[0] = 
    xxxx_HLS[69] = conformance[0] & acceptable_conformance[link->association][0];
    negotiated_conformance[1] = 
    xxxx_HLS[70] = conformance[1] & acceptable_conformance[link->association][1];
    negotiated_conformance[2] = 
    xxxx_HLS[71] = conformance[2] & acceptable_conformance[link->association][2];
    if(link->association==0)
    {
      xxxx_HLS[71]&=~0x04;
    }  
    if (xxxx_HLS[69] == 0  &&  xxxx_HLS[70] == 0  &&  xxxx_HLS[71] == 0)
    {
        status = 0x11;
    }
    
    memcpy(xxxx_HLS+42,challenge_s2c,16);
     }
    

    if (status)
    {
        memcpy(xxxx1 + 4, context, 9);
        xxxx1[17] = 1;                              // 1 = rejected permanent, 2 = rejected transient
        xxxx1[20] = 0xA0 + ((status >> 4) & 0x0F);  // ACSE service provider
        xxxx1[24] = status & 0x0F;                  // no common ACSE version
        hdlc_async_tx_build_iframe(&tx[link->port], link, TRUE, FALSE, xxxx1, sizeof(xxxx1));
    }
    else if (security_level==0 || security_level==1)
    {
        memcpy(xxxx_LLS_NS + 4, context, 9);
        hdlc_async_tx_build_iframe(&tx[link->port], link, TRUE, FALSE, xxxx_LLS_NS, sizeof(xxxx_LLS_NS));
    }
    else 
    {
        memcpy(xxxx_HLS + 4, context, 9);
        hdlc_async_tx_build_iframe(&tx[link->port], link, TRUE, FALSE, xxxx_HLS, sizeof(xxxx_HLS));
    }
    finalise_and_send_iframe(link, &tx[link->port]);
}
/*- End of function --------------------------------------------------------*/

TimeStump plc_start_time;
void process_aarq(iec62056_46_link_t *link, const uint8_t msg[], int len)
{
    int type;
    int length;
    int i,k;
#if defined(LOG_PACKETS)
    int j;
#endif
    int status;
    int present;
    int context_id;
    const uint8_t *context;
    int security_context_id;
//    const uint8_t *security_context;
    
    const uint8_t *conformance;

    security_level = 0;
    status = 0;
    present = 0;
    context = NULL;
//    security_context = NULL;
    conformance = NULL;
    for (i = 0;  i < len;  )
    {
        type = msg[i++];
        length = msg[i++];
#if defined(LOG_PACKETS)
        printf("Type 0x%X, length %d - ", type, length);
#endif
        switch (type)
        {
        case AARQ_APDU_TAG_PROTOCOL_VERSION:
#if defined(LOG_PACKETS)
            printf("Protocol version\n");
#endif
            if (length < 2  ||  (msg[i + 1] & 0x80) == 0)
                status = 0x22;
            break;
        case AARQ_APDU_TAG_APPLICATION_CONTEXT_NAME:
#if defined(LOG_PACKETS)
            printf("Application context name\n");
#endif
            context = &msg[i];
            if (check_context_name(&msg[i + 2], length - 2, &context_id) == 0x01)
            {
                switch (context_id)
                {
                case 1:
                    if (reject_ln_context)
                        status = 0x12;
                    break;
                case 2:
                    if (reject_sn_context)
                        status = 0x12;
                    break;
                default:
                    status = 0x12;
                    break;
                }
            }
            else
            {
                context_id = -1;
                status = 0x12;
            }
#if defined(LOG_PACKETS)
            printf("Application context name - %d\n", context_id);
#endif
            break;
        case AARQ_APDU_TAG_CALLED_AP_TITLE:
#if defined(LOG_PACKETS)
            printf("Called AP title\n");
#endif
            break;
        case AARQ_APDU_TAG_CALLED_AE_QUALIFIER:
#if defined(LOG_PACKETS)
            printf("Called AE qualifier\n");
#endif
            break;
        case AARQ_APDU_TAG_CALLED_AP_INVOCATION_ID:
#if defined(LOG_PACKETS)
            printf("Called AP invocation ID\n");
#endif
            break;
        case AARQ_APDU_TAG_CALLED_AE_INVOCATION_ID:
#if defined(LOG_PACKETS)
            printf("Called AE invocation ID\n");
#endif
            break;
        case AARQ_APDU_TAG_CALLING_AP_TITLE:
#if defined(LOG_PACKETS)
            printf("Calling AP title\n");
#endif
            break;
        case AARQ_APDU_TAG_CALLING_AE_QUALIFIER:
#if defined(LOG_PACKETS)
            printf("Calling AE qualifier\n");
#endif
            break;
        case AARQ_APDU_TAG_CALLING_AP_INVOCATION_ID:
#if defined(LOG_PACKETS)
            printf("Calling AP invocation ID");
#endif
            break;
        case AARQ_APDU_TAG_CALLING_AE_INVOCATION_ID:
#if defined(LOG_PACKETS)
            printf("Calling AE invocation ID\n");
#endif
            break;
        case AARQ_APDU_TAG_SENDER_ACSE_REQUIREMENTS:
            if (length < 2  ||  (msg[i + 1] & 0x80) == 0)
                status = 0x12;
            else
                present |= 0x04;
#if defined(LOG_PACKETS)
            printf("Sender ACSE requirements\n");
#endif
            break;
        case AARQ_APDU_TAG_MECHANISM_NAME:
#if defined(LOG_PACKETS)
            printf("Mechanism name\n");
#endif
//            security_context = &msg[i];
            if (check_context_name(&msg[i], length, &security_context_id) == 0x02)
            {
                /* Check the context ID */
                switch (security_context_id)
                {
                case 0x00:
                    security_level = 0;//NS
                    break;
                case 0x01:
                    security_level = 1;//LS
                    break;
                case 0x02:
                    security_level = 2;//HS
                    break;
                }
            }
            present |= 0x01;
            break;
        case AARQ_APDU_TAG_CALLING_AUTHENTICATION_VALUE:
#if defined(LOG_PACKETS)
            printf("Authentication value - ");
            for (j = 0;  j < length;  j++)
                printf("%02X ", msg[i + j]);
            printf("\n");
            printf("Expected authentication value - ");
            for (j = 0;  j < authentications[link->association][1] + 1;  j++)
                printf("%02X ", authentications[link->association][1 + j]);
            printf("\n");
#endif
            if(security_level==1){
               if (length != (authentications[link->association][1] + 2))
                status=0x1D;
              if(msg[i] != 0x80)
                status=0x1d;
                     
              for(k=0;k<(length-1);k++)
              { 
                if(msg[i+1+k]!=authentications[link->association][1+k])
                { 
                  status=0x1d; 
                }
              }
            }
   
            else if(security_level==2)
            {
              challenge_c2s_len=msg[i+1]/*length-1*/;
              memcpy(challenge_c2s,msg+i+2,challenge_c2s_len/*length-1*/);
            }
       
            else
            {
             __asm__ __volatile__ (" NOP");
            }  
/*            if(memcmp(&msg[i + 1], &authentications[link->association][1], length - 1))
            {
                status = 0x1D;
            } */
            present |= 0x02;
            break;
        case AARQ_APDU_TAG_IMPLEMENTATION_INFORMATION:
#if defined(LOG_PACKETS)
            printf("Implementation information\n");
#endif
            present |= 0x08;
            break;
        case AARQ_APDU_TAG_USER_INFORMATION:
#if defined(LOG_PACKETS)
            printf("User information\n");
#endif
            /* Check the protocol version byte */
#if defined(LOG_PACKETS)
            printf("Proposed DLMS protocol version %d\n", msg[i + 6]);
#endif
            if (msg[i + 3])
            {
                status = 0x11;
            }
            else if (msg[i + 4])
            {
                status = 0x11;
            }
            else if (msg[i + 5]  &&  msg[i + 7] < DLMS_PROTOCOL_VERSION)
            {
                status = 0x11;
            }
            else if (msg[i + 5] == 0  &&  msg[i + 6] < DLMS_PROTOCOL_VERSION)
            {
                status = 0x11;
            }
            conformance = msg + i + 11;
            /* Check the proposed MAX PDU is acceptable */
            if (msg[i + length - 2] == 0  &&  msg[i + length - 1] < 128)
            {
                status = 0x11;
            }
            break;
        default:
            if (xxxx(type) == 0)
                break;
#if defined(LOG_PACKETS)
            printf("Unknown\n");
#endif
            break;
        }
        if (type & 0x20)
            extract_ber_field(&msg[i], length);
        i += length;
    }
    if (authentications[link->association][0] != security_level)
    {
        status = 0x1D;
    }
    else
    {
        switch (security_level)
        {
        case 0:
            if (present)
                status = 0x1D;
            break;
        case 1:
            if (present != 0x07)
                status = 0x1D;
            break;
        case 2:
            if (present != 0x07)
                status = 0x1D;
            break;
        }
    }
    if (present & 0x08)
        status = 0x11;
#if defined(LOG_PACKETS)
    printf("AARQ status is 0x%X\n", status);
#endif
    send_aare(link, context, conformance, status);
    link->configured = (status == 0);
    if(link->configured) //connection established
    {
    	uint8_t tmp;
        //log communication event
        event_log l;
        if(link->port == PLC_PORT)
        {
        	plc_start_time = getTimeStamp(rtcc.year,rtcc.month,rtcc.day,rtcc.hour,rtcc.minute,rtcc.second);
        }
        else if(link->port == IEC_PORT )
        {
        	l.event_code = BEGIN_COMMUNICATION_OPTICAL_PORT;
        	l.time_stamp = getTimeStamp(rtcc.year,rtcc.month,rtcc.day,rtcc.hour,rtcc.minute,rtcc.second);
        	l.checksum  =(getCheckSum(&(l.time_stamp.TimestampLow),4) + l.time_stamp.TimestampUp + l.event_code)&0xff;
        	tmp = 6;
        	write_to_eeprom(&l,&tmp,log_events);
        }
    }
}

/*- End of function --------------------------------------------------------*/

void send_confirmed_service_error(iec62056_46_link_t *link, int a, int b, int c)
{
    //async_hdlc_tx_t tx;
    uint8_t response[4];

#if defined(LOG_PACKETS)
    printf("send Confirmed-Service-Error\n");
#endif
    response[0] = PDU_CONFIRMED_SERVICE_ERROR;
    response[1] = a;
    response[2] = b;
    response[3] = c;
    hdlc_async_tx_build_iframe(&tx[link->port], link, TRUE, FALSE, response, 4);
    finalise_and_send_iframe(link, &tx[link->port]);
}
/*- End of function --------------------------------------------------------*/

void send_read_response(iec62056_46_link_t *link)
{
    //async_hdlc_tx_t tx;
    uint8_t response[8];

#if defined(LOG_PACKETS)
    printf("send Read-Response\n");
#endif
    response[0] = PDU_READ_RESPONSE;
    response[1] = 1;    /* Items */
    response[2] = 0;    /* Data */
    response[3] = TAG_INT32;
    response[4] = 1;    /* The integer */
    response[5] = 2;
    response[6] = 3;
    response[7] = 4;
    hdlc_async_tx_build_iframe(&tx[link->port], link, TRUE, FALSE, response, 8);
    finalise_and_send_iframe(link, &tx[link->port]);
}
/*- End of function --------------------------------------------------------*/

//#if defined(IEC62056_SUPPORT_READ)
static void process_read_request(iec62056_46_link_t *link, const uint8_t msg[], int len)
{
 // Not Supported
}
/*- End of function --------------------------------------------------------*/
//#endif

//#if defined(IEC62056_SUPPORT_WRITE)
static void process_write_request(iec62056_46_link_t *link, const uint8_t msg[], int len)
{
//Not Supported
}
/*- End of function --------------------------------------------------------*/
//#endif


int current_get_block_type = 0;
int current_get_block_len = 0;
int current_get_block_no = -1;
unsigned char current_get_block_datatype = 0;
uint8_t *current_get_block_addr = NULL;
exchange_data_t current_get_block_callback = NULL;
int current_get_block_chunk_len = -1,current_get_block_chunk_len_temp;
int current_get_block_chunk_last = 0;

/* This supports block mode for getting the object list to the client */
void send_get_response_block_a(iec62056_46_link_t *link, uint8_t invoke_id, int32_t block_no)
{
    //async_hdlc_tx_t tx;
    uint8_t hdr[10];
//    uint8_t xxxx[Chunk_Size];
    uint8_t *xxxx;
    int16_t len;
    int i;
    int n;
    int attr_no;
    int method_no;
    int max;
    int selectors;

    xxxx=current_get_block_addr;
    hdr[0] = PDU_GET_RESPONSE;
    hdr[1] = 2;     /* With block */
    hdr[2] = invoke_id;
    hdr[3] = 0;     /* Not the last item */
    unpack32(&hdr[4], ++block_no);
    hdr[8] = 0;     /* Data block follows */
    len = 0;

    switch (block_no)
    {
    case 1:
        /* Count the elements in the object array */
        n = 0;
        for (i = 0;  object_list[i].attr_elements;  i++)
        {
          if(object_list[i].assoc & (0x01<<(link->association)))
          { if (object_list[i].attrs)
                n++;
          }  
        }
        xxxx[0] = TAG_ARRAY;
        xxxx[1] = n;
        len = 2;
        break;
    default:
        n = 2;
        for (i = 0;  object_list[i].attr_elements;  i++)
        {
          if(object_list[i].assoc & (0x01<<(link->association)))
           {
            if (object_list[i].attrs)
            {
                if (block_no == n)
                {
                    /* object_list_element */
                    xxxx[0] = TAG_STRUCTURE;
                    xxxx[1] = 4;
                    /* class_id */
                    xxxx[2] = TAG_UINT16;
                    unpack16(&xxxx[3], object_list[i].class_id);
                    /* version */
                    xxxx[5] = TAG_UINT8;
                    xxxx[6] = object_list[i].version;
                    /* logical_name */
                    xxxx[7] = TAG_OCTET_STRING;
                    xxxx[8] = 6;
                    memcpy(&xxxx[9], object_list[i].instance_id, 6);
                    /* access_rights */
                    xxxx[15] = TAG_STRUCTURE;
                    xxxx[16] = 2;
                    /* attribute_access */
                    xxxx[17] = TAG_ARRAY;
                    xxxx[18] = object_list[i].attr_elements;
                    len = 19;
                    break;
                }
                n++;
                if (block_no < n + ((object_list[i].attr_elements + 7) >> 3))
                {
                    len = 0;
                    attr_no = (block_no - n) << 3;
                    max = attr_no + 8;
                    if (max > object_list[i].attr_elements)
                        max = object_list[i].attr_elements;
                    for (  ;  attr_no < max;  attr_no++)
                    {
                        /* attribute_access_item */
                        xxxx[len + 0] = TAG_STRUCTURE;
                        xxxx[len + 1] = 3;
                        /* attribute_id */
                        xxxx[len + 2] = TAG_INT8;
                        xxxx[len + 3] = object_list[i].attrs[attr_no].id & 0x3F;
                        /* access_mode */
                        xxxx[len + 4] = TAG_ENUM;
                        xxxx[len + 5] = (object_list[i].attrs[attr_no].access_mode >> 3*(3 - link->association)) & 0x07;
                        /* access_selectors */
                        selectors = object_list[i].attrs[attr_no].id & 0xC0;
                        if (selectors)
                        {
                            xxxx[len + 6] = TAG_ARRAY;
                            xxxx[len + 7] = (selectors == 0xC0)  ?  2  :  1;
                            len += 8;
                            /* Check for the selective access by range bit */
                            if ((selectors & 0x40))
                            {
                                xxxx[len + 0] = TAG_INT8;
                                xxxx[len + 1] = 1;
                                len += 2;
                            }
                            /* Check for the selective access by entry bit */
                            if ((selectors & 0x80))
                            {
                                xxxx[len + 0] = TAG_INT8;
                                xxxx[len + 1] = 2;
                                len += 2;
                            }
                        }
                        else
                        {
                            xxxx[len + 6] = TAG_NULLDATA;
                            len += 7;
                        }
                    }
                    break;
                }
                n += ((object_list[i].attr_elements + 7) >> 3);
                if (block_no == n)
                {
                    /* method_access_descriptor */
                    xxxx[0] = TAG_ARRAY;
                    xxxx[1] = object_list[i].method_elements;
                    len = 2;
                    for (method_no = 0;  method_no < object_list[i].method_elements;  method_no++)
                    {
                        xxxx[len + 0] = TAG_STRUCTURE;
                        xxxx[len + 1] = 2;
                        xxxx[len + 2] = TAG_INT8;
                        xxxx[len + 3] = object_list[i].methods[method_no].id;
                        xxxx[len + 4] = TAG_BOOLEAN;
                        xxxx[len + 5] = (object_list[i].methods[method_no].access_mode >> 3*(3 - link->association)) & 0x01;
                        len += 6;
                    }
                    /* Check if we reached the end of the list */
                    for (i++;  object_list[i].attr_elements;  i++)
                    {
                        if ((object_list[i].attrs)&&(object_list[i].assoc & (0x01<<(link->association)))) 
                            break;
                    }
                    if (object_list[i].attr_elements == 0)
                        hdr[3] = 1; /* Last item */
                    break;
                }
                n++;
            }
         }
        } 
        break;
    }
    hdr[9] = len;
    hdlc_async_tx_build_iframe(&tx[link->port], link, TRUE, FALSE, hdr, sizeof(hdr));
    hdlc_async_tx_build_iframe_chunk_add(&tx[link->port], link, xxxx, len);
    finalise_and_send_iframe(link, &tx[link->port]);
#if defined(LOG_PACKETS)
    if (block_no <= 1)
        block_len = 0;
    memcpy(&block_log[block_len], xxxx, len);
    block_len += len;
    if (hdr[3])
    {
        printf("Reconstructed block (%d bytes)\n", block_len);
        asn1_to_xml(block_log, block_len, 1);
    }
#endif
}
/*- End of function --------------------------------------------------------*/

Msg_Info msg_info;


void send_get_response_block_b(iec62056_46_link_t *link, uint8_t invoke_id, int32_t block_no)
{
    //async_hdlc_tx_t tx;
    uint8_t hdr[12],hdr2[4];
    uint8_t hdr1_sz=0,hdr2_sz=0;
    uint16_t total_data_sz=0;
    int start;
    uint16_t base=0,size;
   
    current_get_block_no = block_no;
  
    hdr[0] = PDU_GET_RESPONSE;
    hdr[1] = 2;     /* With block */
    hdr[2] = invoke_id;
    hdr[3] = 0;     /* Not the last item */
    unpack32(&hdr[4], ++block_no);
    hdr[8] = 0;     /* Data block follows */

    if(current_get_block_callback && msg_info.template != NULL)
    {
          if(current_get_block_no == 0)
            { // This assumes that the template used here didn't have the ARRAY TAG.
              hdr2[0]=TAG_ARRAY;
              hdr2_sz++;
              if(msg_info.num_entries>127)
              {
                if(msg_info.num_entries>255)
                {
                  hdr2[1]=0x82; //+ (((unsigned char)(msg_info.num_entries >> 8)) & 0x7f);
                  hdr2[2]=(unsigned char) ((msg_info.num_entries >> 8) & 0xFF);
                  hdr2[3]=(unsigned char) (msg_info.num_entries&0xfF);
                  //hdr2[1]=0x80+(msg_info.num_entries<<8);
                  //hdr2[2]=msg_info.num_entries&0xFF;
                  hdr2_sz+=3;
                }
                else
                {
                  hdr2[1]=0x81; //+ (((unsigned char)(msg_info.num_entries >> 8)) & 0x7f);
                  hdr2[2]=(unsigned char) ((msg_info.num_entries) & 0xFF);
                  hdr2_sz+=2;
                }
                
              }
              else
              {
                 hdr2[1]=msg_info.num_entries&0x7F;
                 hdr2_sz++;
              }  
              
              base = last_start;
            }
            
            
            if(access_selector ==2 && SA_To_Column!=0 && SA_From_Column!=0) 
              { size = msg_info.column_szs[SA_To_Column-SA_From_Column];}
            else if(access_selector==1 && SA_Range_Num_Columns > 0)
              {  size = msg_info.column_szs[SA_Range_Num_Columns-1]; }
            else
              {  size = msg_info.sz_template; }
            
            if (last_start + Chunk_Size >= size)
              {
                    current_get_block_chunk_len = size - last_start;
                    current_get_block_chunk_len = interpret_and_fill(&current_get_block_addr[base], &msg_info.template[last_start], current_get_block_chunk_len);
                    msg_info.entries_remaining--;
                    if(msg_info.entries_remaining==0)
                    {
                      current_get_block_chunk_last=1;
                    }
                    else
                    { current_get_block_chunk_last=0;
                      //current_get_block_addr=0;
                        last_start = 0;
                        //base=current_get_block_chunk_len;
//#ifdef 0
                        do{
                            if(current_get_block_chunk_len+size>=Chunk_Size)
                            {
                              current_get_block_chunk_len_temp=Chunk_Size-current_get_block_chunk_len;
                              current_get_block_chunk_len_temp = interpret_and_fill(&current_get_block_addr[current_get_block_chunk_len], &msg_info.template[last_start], current_get_block_chunk_len_temp);
                              last_start=current_get_block_chunk_len_temp;
                              current_get_block_chunk_len+=current_get_block_chunk_len_temp;
                              //base=current_get_block_chunk_len;
                            }  
                            else
                            { current_get_block_chunk_len_temp=size;
                               current_get_block_chunk_len += interpret_and_fill(&current_get_block_addr[current_get_block_chunk_len], &msg_info.template[last_start], current_get_block_chunk_len_temp);
                               last_start=0;
                                msg_info.entries_remaining--;
                                if(msg_info.entries_remaining==0)
                                {
                                  current_get_block_chunk_last=1;
                                  break;
                                }
                                else
                                { current_get_block_chunk_last=0;
                                }
                             // base=current_get_block_chunk_len;  
                            }
                            
                          }
                        while(current_get_block_chunk_len+size<Chunk_Size);
//#endif                    
                    }
              } 
                else
                {
                    current_get_block_chunk_len = Chunk_Size;
                    current_get_block_chunk_last = 0;
                    current_get_block_chunk_len = interpret_and_fill(&current_get_block_addr[base], &msg_info.template[last_start], current_get_block_chunk_len);
                   // current_get_block_addr+=current_get_block_chunk_len;
                    last_start += current_get_block_chunk_len;
                }
              
                hdr[3] = current_get_block_chunk_last;
                total_data_sz = current_get_block_chunk_len + hdr2_sz;
                start = base;
                base=0;
    }
    else
    {
        start = (block_no - 1)*Chunk_Size;
        if(block_no==1)
        {  
          hdr2[0]=current_get_block_datatype;
           hdr2_sz=1;
        }   
        if (start + Chunk_Size >= current_get_block_len)
        {
            hdr[3] = 1;     /* Last chunk */
            total_data_sz = current_get_block_len - start;
        }
        else
        {
            hdr[3] = 0;     /* Not the last chunk */
            total_data_sz = Chunk_Size+hdr2_sz;
        }
        
        if(current_get_block_len<128)
          start+=1;
        else
          start+=2;
          
    }
    
    if(total_data_sz>255)
    {
      hdr[9]=0x82; //+ (((unsigned char)(msg_info.num_entries >> 8)) & 0x7f);
      hdr[10]=(unsigned char) ((total_data_sz >> 8) & 0xFF);
      hdr[11]=(unsigned char) (total_data_sz&0xff);
      hdr1_sz=12;
    }
    else if(total_data_sz>127)
    {  
      hdr[9]=0x81; //+ (((unsigned char)(msg_info.num_entries >> 8)) & 0x7f);
      hdr[10]=(unsigned char) (total_data_sz&0xff);
      hdr1_sz=11;
    }
    else
    {
      hdr[9]=(total_data_sz&0xff);
      hdr1_sz=10;
    }  
    
    hdlc_async_tx_build_iframe(&tx[link->port], link, TRUE, FALSE, hdr, hdr1_sz);
    if(hdr2_sz)
    {  hdlc_async_tx_build_iframe_chunk_add(&tx[link->port], link, hdr2, hdr2_sz);
    hdlc_async_tx_build_iframe_chunk_add(&tx[link->port], link, &current_get_block_addr[start], total_data_sz-hdr2_sz);}
    else
    { hdlc_async_tx_build_iframe_chunk_add(&tx[link->port], link, &current_get_block_addr[start], total_data_sz);}
    finalise_and_send_iframe(link, &tx[link->port]);
#if defined(LOG_PACKETS)
    if (block_no <= 1)
        block_len = 0;
    memcpy(&block_log[block_len], &current_get_block_addr[start], hdr[9]);
    block_len += hdr[9];
    if (hdr[3])
    {
        printf("Reconstructed block (%d bytes)\n", block_len);
        asn1_to_xml(block_log, block_len, 1);
    }
#endif
}
/*- End of function --------------------------------------------------------*/

void send_get_response_block(iec62056_46_link_t *link, uint8_t invoke_id, int32_t block_no)
{
    switch (current_get_block_type)
    {
    case 0:
        send_get_response_block_a(link, invoke_id, block_no);
        break;
    case 1:
        send_get_response_block_b(link, invoke_id, block_no);
        break;
    }
}
/*- End of function --------------------------------------------------------*/

void send_get_response(iec62056_46_link_t *link,
                       uint8_t invoke_id,
                       uint16_t class_id,
                       const uint8_t instance_id[6],
                       uint8_t attribute_id,
                       uint8_t access_selector)
{

    uint8_t response[128];
    int obj;
    int element;
    int len,lenx;
#if defined(LOG_PACKETS)
    printf("send Get-Response\n");
#endif
    link_xxx = link;
    response[0] = PDU_GET_RESPONSE;
    response[1] = 1;   /* Normal */
    response[2] = invoke_id;
    response[3] = 1;   /* Failure */
    response[4] = ACCESS_RESULT_OBJECT_UNDEFINED;
    len = 5;

    if((*authentications[link->association]==2)&&(Authorised==NO))
     {
       response[4] = ACCESS_RESULT_SCOPE_OF_ACCESS_VIOLATED;
        hdlc_async_tx_build_iframe(&tx[link->port], link, TRUE, FALSE, response, len);
     }  
     else

    if (class_id != 999) 
    {
        obj = locate_object_attr(link, class_id, instance_id, attribute_id, access_selector, &element);
        if (obj >= 0)
        {
            /* We found a match for the object */
            if (element == -1)
            {
                if ((object_list[obj].attrs[1].access_mode & (ATTR_READ << 3*(3 - link->association))))
                {                  
                    current_get_block_type = 0;
                    current_get_block_addr = object_list[obj].attrs[1].addr;
                    send_get_response_block(link, invoke_id, 0);
                    return;
                }
                response[4] = ACCESS_RESULT_READ_WRITE_DENIED;
               hdlc_async_tx_build_iframe(&tx[link->port], link, TRUE, FALSE, response, len);  
            }
            else
            {
               if (attribute_id == 1)
                {
                  if ((object_list[obj].attrs[0].access_mode & (ATTR_READ << 3*(3 - link->association))))
                  {                   
                  /* Treat attribute 1 specially. It is always the logical name */
                    response[3] = 0;   /* Success */
                    response[4] = object_list[obj].attrs[element].type;
                    response[5] = 6;
                    memcpy(&response[6], object_list[obj].instance_id, 6);
                    len = 5 + 7;
                  }
                  else
                  {
                   __asm__ __volatile__ (" NOP");
                    response[4]=ACCESS_RESULT_READ_WRITE_DENIED;
                  }  
                  hdlc_async_tx_build_iframe(&tx[link->port], link, TRUE, FALSE, response, len);
                }
                else
                {
                    if ((object_list[obj].attrs[element].access_mode & (ATTR_READ << 3*(3 - link->association))))
                    {
                            msg_info.sz_template=0;
                            msg_info.template=NULL;
                            msg_info.num_entries=0;
                            msg_info.entries_remaining=0;
                            msg_info.column_szs=0;
                            current_get_block_chunk_last=0;
                        /* Its available for read */
                        if (object_list[obj].attrs[element].type == TAG_ARRAY
                            ||
                            object_list[obj].attrs[element].type == TAG_STRUCTURE
                            ||
                            object_list[obj].attrs[element].type == TAG_OCTET_STRING)   
                        {
                            current_get_block_no = -1;
                            if (object_list[obj].attrs[element].callback)
                            {  
                              object_list[obj].attrs[element].callback(object_list[obj].attrs[element].addr, ATTR_READ); 
                            }
                            if(msg_info.template==NULL)
                            {
                               //msg_info.template=NULL;
                               if(*(object_list[obj].attrs[element].addr) & 0x80)
                               { 
                                 msg_info.sz_template=((*object_list[obj].attrs[element].addr&0x7f)<<8)+*(object_list[obj].attrs[element].addr+1);
                               }
                               else
                               { 
                                 msg_info.sz_template=*object_list[obj].attrs[element].addr; 
                               }
                               msg_info.num_entries=1;
                            }   
                            
                            if (msg_info.num_entries==0)//0xffff 
                            {
                              *object_list[obj].attrs[element].addr = 0;
                              *(object_list[obj].attrs[element].addr+1) = 0;
                            }
                            
                            else if((msg_info.sz_template > Chunk_Size)|| (msg_info.num_entries>1) || (msg_info.template !=NULL) )
                            {                           
                                 /* This is something big, which needs to be handled in block mode */
                                 current_get_block_type = 1 ;
                                 current_get_block_datatype = object_list[obj].attrs[element].type;
                                 current_get_block_len = msg_info.sz_template; /*((s[0] & 0x7F) << 8) | s[1]*/;
                                 current_get_block_addr = (uint8_t *) object_list[obj].attrs[element].addr;
                                 current_get_block_callback = object_list[obj].attrs[element].callback;
                                 msg_info.entries_remaining=msg_info.num_entries;
                                 last_start = 0;
                                 send_get_response_block(link, invoke_id, 0);
                                 return;
                             }
                            
    
                        }
                        else if (object_list[obj].attrs[element].callback)
                        { 
                          object_list[obj].attrs[element].callback(object_list[obj].attrs[element].addr, ATTR_READ); 
                        }
                        response[3] = 0;   /* Success */
                        response[4] = object_list[obj].attrs[element].type;
                        lenx = type_copy(&response[5], object_list[obj].attrs[element].addr, object_list[obj].attrs[element].type);
                        hdlc_async_tx_build_iframe(&tx[link->port], link, TRUE, FALSE, response, len);
                        
                        if(object_list[obj].attrs[element].type==TAG_ARRAY && *object_list[obj].attrs[element].addr==0x80)
                          hdlc_async_tx_build_iframe_chunk_add(&tx[link->port], link, object_list[obj].attrs[element].addr+2, lenx);
                        else if(object_list[obj].attrs[element].type==TAG_ARRAY && *object_list[obj].attrs[element].addr!=0x80)
                          hdlc_async_tx_build_iframe_chunk_add(&tx[link->port], link, object_list[obj].attrs[element].addr+1, lenx);
                        else
                          hdlc_async_tx_build_iframe_chunk_add(&tx[link->port], link, response+len, lenx);
                          
                    }
                    else
                    {
                        response[4] = ACCESS_RESULT_READ_WRITE_DENIED;
                        hdlc_async_tx_build_iframe(&tx[link->port], link, TRUE, FALSE, response, len);
                    }
                }
            }
        }
      else{
        hdlc_async_tx_build_iframe(&tx[link->port], link, TRUE, FALSE, response, len);
         }  
    }
    else{
        hdlc_async_tx_build_iframe(&tx[link->port], link, TRUE, FALSE, response, len);
    }
//    hdlc_async_tx_build_iframe(&tx[link->port], link, TRUE, FALSE, response, len);
    
#if defined(LOG_PACKETS)
    printf("Tx len is %d\n", tx.msg_len);
#endif
    finalise_and_send_iframe(link, &tx[link->port]);
}
/*- End of function --------------------------------------------------------*/



static void process_get_request(iec62056_46_link_t *link, const uint8_t msg[], int len)
{
    int i;
    int j;
    uint8_t invoke_id;
    uint16_t class_id;
    uint8_t instance_id[6];
    int8_t attribute_id;
    
    int32_t block_no;

    if (!link->configured)
    {
        send_confirmed_service_error(link, CONFIRMED_SERVICE_ERROR_READ, SERVICE_ERROR_SERVICE, 2);
        return;
    }
    i = 0;
    switch (msg[i++])
    {
    case 1:
#if defined(LOG_PACKETS)
        printf("Get-Request-Normal\n");
#endif
        invoke_id = msg[i++];

        /* Class ID/Instance ID/Attribute or Method is the basic way to specify something */
        class_id = pack16(&msg[i]);
        i += 2;
        for (j = 0;  j < 6;  j++)
            instance_id[j] = msg[i++];
        attribute_id = msg[i++];
        access_selector = -1;
        if (msg[i++])//i=11
        {
            //i=12;
          /* Access selector is present */
            access_selector = msg[i];//
            if(access_selector==0x02)//entry descriptor
            {
              SA_From_Entry = ((uint32_t)msg[i+4]<<24)+((uint32_t)msg[i+5]<<16)+((uint32_t)msg[i+6]<<8)+(uint32_t)msg[i+7];
              SA_To_Entry = ((uint32_t)msg[i+9]<<24)+((uint32_t)msg[i+10]<<16)+((uint32_t)msg[i+11]<<8)+(uint32_t)msg[i+12];
              SA_From_Column = ((uint16_t)msg[i+14]<<8) + msg[i+15];
              SA_To_Column = ((uint16_t)msg[i+17]<<8)+msg[i+18];  
              i+=18;
            }
            else if(access_selector==0x01) //range descriptor
            {
              SA_Range[0].Year = msg[i+23]<<8;
              SA_Range[0].Year |=msg[i+24];
              
              memcpy(&SA_Range[0].Month,msg+i+25,6);
              
              SA_Range[1].Year = msg[i+37]<<8;
              SA_Range[1].Year |=msg[i+38];
              memcpy(&SA_Range[1].Month,msg+i+39,6);
              SA_Range_Num_Columns=msg[i+50];
              i+=50;
            }
            /* There is now a chunk of ASN.1 that describes the access selection */
            extract_cosem_asn1(&msg[i], len - i);
        }
        if (i > len)
        {
            send_get_response(link, invoke_id, 999, instance_id, attribute_id, access_selector);
            break;
        }
#if defined(LOG_PACKETS)
        printf("   invoke-id: %d priority: %d\n", invoke_id & 0x7F, (invoke_id & 0x80) >> 7);
        printf("   class-id %d, %s\n", class_id, class_id_to_str(class_id));
        printf("   instance-id: ");
        display_obis_name(instance_id);
        printf("   attribute-id: %d, %s\n", attribute_id, class_id_attribute_to_str(class_id, attribute_id));
        printf("   access-selector: %d\n", access_selector);
#endif
        send_get_response(link, invoke_id, class_id, instance_id, attribute_id, access_selector);
        break;
    case 2:
        invoke_id = msg[i++];
        block_no = pack32(&msg[i]);
        i += 4;
#if defined(LOG_PACKETS)
        printf("Get-Request-Next-Block after %d\n", block_no);
#endif
        send_get_response_block(link, invoke_id, block_no);
        break;
    case 3:
#if defined(LOG_PACKETS)
        printf("Get-Request-With-List\n");
#endif
        invoke_id = msg[i++];
        break;
    default:
        send_get_response(link, invoke_id, 999, instance_id, attribute_id, access_selector);
        break;
    }
}
/*- End of function --------------------------------------------------------*/



void send_set_response(iec62056_46_link_t *link,
                       uint8_t invoke_id,
                       uint16_t class_id,
                       const uint8_t instance_id[6],
                       uint8_t attribute_id,
                       uint8_t access_selector,
                       uint16_t write_len,
                       uint8_t *set_data  )
{
    //async_hdlc_tx_t tx;
    uint8_t response[4];
    int obj;
    int element;
    int i;
    
#if defined(LOG_PACKETS)
    printf("send Set-Response\n");
#endif
    response[0] = PDU_SET_RESPONSE;
    response[1] = 1;    /* Normal */
    response[2] = invoke_id;
    response[3] = ACCESS_RESULT_OBJECT_UNDEFINED;
    msg_info.start_entry=0;
    if(!(negotiated_conformance[2] & 0x08))
    { 
       response[3] = ACCESS_RESULT_SCOPE_OF_ACCESS_VIOLATED;
    }
    else if((*authentications[link->association]==2)&&(Authorised==NO))
    {
       response[3] = ACCESS_RESULT_SCOPE_OF_ACCESS_VIOLATED;
    }  
    else

    if (class_id != 999)
    {
        obj = locate_object_attr(link, class_id, instance_id, attribute_id, access_selector, &element);
        if (obj >= 0)
        {
            /* We found a match for the object */
            if ((object_list[obj].attrs[element].access_mode & (ATTR_WRITE << 3*(3 - link->association))))
            {
                /* Its available for write */
                /* TODO: perform the write */
                //type_copy(object_list[obj].attrs[element].addr, &data_response[5], object_list[obj].attrs[element].type);
                if((object_list[obj].attrs[element].type!=TAG_STRUCTURE)&&(object_list[obj].attrs[element].type!=TAG_ARRAY))
                {
                  for(i=0;i<write_len-1;i++)
                       {
                         *(object_list[obj].attrs[element].addr+i)=*(set_data+i+1);
                       }
                }
                else //This difference is there because we are representing structures in a different manner in our software
                {
                  *object_list[obj].attrs[element].addr=write_len-1;
                  
                  for(i=0;i<write_len-1;i++)
                       {
                         *(object_list[obj].attrs[element].addr+i+1)=*(set_data+i+1);
                       }
                }  
                               
                if (object_list[obj].attrs[element].callback)
                   {object_list[obj].attrs[element].callback(object_list[obj].attrs[element].addr, ATTR_WRITE);}
                if(msg_info.start_entry!=0xffff)
                response[3] = ACCESS_RESULT_SUCCESS;
                else
                response[3] = ACCESS_RESULT_OTHER_REASON;
            }
            else
            {
                response[3] = ACCESS_RESULT_READ_WRITE_DENIED;
            }
        }
    }
    hdlc_async_tx_build_iframe(&tx[link->port], link, TRUE, FALSE, response, 4);
    finalise_and_send_iframe(link, &tx[link->port]);
}
/*- End of function --------------------------------------------------------*/
int set_obj=0;
int set_element=0;
void send_set_response_datablock(iec62056_46_link_t *link,
                       uint8_t invoke_id,
                       uint16_t class_id,
                       const uint8_t instance_id[6],
                       uint8_t attribute_id,
                       uint8_t access_selector,
                       uint16_t write_len,
                       uint8_t *set_data,
                       uint8_t mode,
                       uint32_t block_num,
                       uint8_t last_block)
{
    //async_hdlc_tx_t tx;
    uint8_t response[8];
   // static int obj;
   // int element;
    int i;
    static uint8_t *data_buffer;

#if defined(LOG_PACKETS)
    printf("send Set-Response\n");
#endif
    response[0] = PDU_SET_RESPONSE;
    response[1] = 2;
    response[2] = invoke_id;
    response[3] = ACCESS_RESULT_OBJECT_UNDEFINED;
    if(!(negotiated_conformance[1] & 0x08))
    { response[3] = ACCESS_RESULT_SCOPE_OF_ACCESS_VIOLATED;}

    else if((*authentications[link->association]==2)&&(Authorised==NO))
     {
       response[3] = ACCESS_RESULT_SCOPE_OF_ACCESS_VIOLATED;
     }  
     else
    
    
    if (class_id != 999)
    {
        if(mode==2)
        { set_obj = locate_object_attr(link, class_id, instance_id, attribute_id, access_selector, &set_element); 
          data_buffer=object_list[set_obj].attrs[set_element].addr;
           }
        if (set_obj >= 0)
        {
            /* We found a match for the object */
            if ((object_list[set_obj].attrs[set_element].access_mode & (ATTR_WRITE << 3*(3 - link->association))))
            {
                /* Its available for write */
                /* TODO: perform the write */
                //type_copy(object_list[obj].attrs[element].addr, &data_response[5], object_list[obj].attrs[element].type);
                               
                if (object_list[set_obj].attrs[set_element].callback && object_list[set_obj].attrs[set_element].type == TAG_ARRAY )
                   {
                     if(mode==2)
                     {
                       msg_info.template=NULL;
                       object_list[set_obj].attrs[set_element].callback(object_list[set_obj].attrs[set_element].addr, ATTR_WRITE);
                       
                         // Check whether the data is as per our expectation
                         if(msg_info.template==NULL || *set_data != TAG_ARRAY || *(set_data+1)!= msg_info.num_entries)
                         { response[3] = ACCESS_RESULT_OTHER_REASON;
                           hdlc_async_tx_build_iframe(&tx[link->port], link, TRUE, FALSE, response, 4);
                           finalise_and_send_iframe(link, &tx[link->port]);
                           return; 
                         }
                         write_len-=2;  // remove the first two bytes which is TAG_ARRAY and Num_Elements
                         for(i=0;i<write_len;i++)
                              {
                                *(data_buffer+i) = *(set_data+2+i);
                              }
                         template_pointer=(uint8_t *)msg_info.template; // Initialize the template pointer
                         setblock_residue_len=0; 
                         msg_info.entries_remaining = msg_info.num_entries;
                     }
                    else // it is expected that data_buffer has the residue data left          
                    {  
                       for(i=0;i<write_len;i++)
                       {
                         *(data_buffer+setblock_residue_len+i) = *(set_data+i);
                       }  
                    }                 
                 
                  compare_and_retrieve(data_buffer, write_len + setblock_residue_len);
                  if(msg_info.start_entry==0xffff){   
                      response[3] = ACCESS_RESULT_OTHER_REASON;
                      hdlc_async_tx_build_iframe(&tx[link->port], link, TRUE, FALSE, response, 4);
                      finalise_and_send_iframe(link, &tx[link->port]);
                      return; }
                 }
                else if (object_list[set_obj].attrs[set_element].type==TAG_OCTET_STRING)
                {
                  if(mode==2)
                  { if(*(set_data+1)&0x80)
                  {setblock_residue_len=2;}
                    else
                  {setblock_residue_len=1;}    
                  }
                  memcpy(object_list[set_obj].attrs[set_element].addr+setblock_residue_len,set_data+1,write_len);
                  setblock_residue_len+=write_len;
                }  

                if(last_block)
                { 
                  if ((object_list[set_obj].attrs[set_element].type==TAG_OCTET_STRING)&&object_list[set_obj].attrs[set_element].callback)
                  {
                    object_list[set_obj].attrs[set_element].callback(object_list[set_obj].attrs[set_element].addr, ATTR_WRITE);
                     if(msg_info.start_entry==0xffff){   
                      response[3] = ACCESS_RESULT_OTHER_REASON;
                      hdlc_async_tx_build_iframe(&tx[link->port], link, TRUE, FALSE, response, 4);
                      finalise_and_send_iframe(link, &tx[link->port]);
                      return; }
                  }  
                  
                  response[1]=3;
                  response[3]=ACCESS_RESULT_SUCCESS;
                  unpack32(&response[4],block_num) ;
                  hdlc_async_tx_build_iframe(&tx[link->port], link, TRUE, FALSE, response, 8);
                  block_num=0;
                }
                else
                {  
                unpack32(&response[3],block_num) ;
                hdlc_async_tx_build_iframe(&tx[link->port], link, TRUE, FALSE, response, 7); 
                }
            }
            else
            {
                response[3] = ACCESS_RESULT_READ_WRITE_DENIED;
                hdlc_async_tx_build_iframe(&tx[link->port], link, TRUE, FALSE, response, 4);
            }
        }
    }
    if(response[3]==ACCESS_RESULT_SCOPE_OF_ACCESS_VIOLATED)
    {hdlc_async_tx_build_iframe(&tx[link->port], link, TRUE, FALSE, response, 4);}
    
    finalise_and_send_iframe(link, &tx[link->port]);
}
/*- End of function --------------------------------------------------------*/


static void process_set_request(iec62056_46_link_t *link, const uint8_t msg[], int len)
{
    int i;
    int j;
    uint8_t invoke_id;
    uint16_t class_id=0;
    uint8_t instance_id[6];
    int8_t attribute_id=0;
    uint8_t access_selector=0;
    uint16_t write_len;
    uint8_t last_block=0;
    uint32_t block_num=0;

    if (!link->configured)
    {
        send_confirmed_service_error(link, CONFIRMED_SERVICE_ERROR_WRITE, SERVICE_ERROR_SERVICE, 2);
        return;
    }
    i = 0;
    switch (msg[i++])
    {
    case 1:
#if defined(LOG_PACKETS)
        printf("Set-request-normal - ");
#endif
        invoke_id = msg[i++];
        class_id = pack16(&msg[i]);
        i += 2;
        for (j = 0;  j < 6;  j++)
            instance_id[j] = msg[i++];
        attribute_id = msg[i++];
        access_selector = msg[i++];

#if defined(LOG_PACKETS)
        printf("   invoke-id: %d priority: %d\n", invoke_id & 0x7F, (invoke_id & 0x80) >> 7);
        printf("   class-id %d, %s\n", class_id, class_id_to_str(class_id));
        printf("   instance-id: ");
        display_obis_name(instance_id);
        printf("   attribute-id: %d, %s\n", attribute_id, class_id_attribute_to_str(class_id, attribute_id));
        printf("   access-selector: %d\n", access_selector);
#endif
        write_len=extract_cosem_asn1(&msg[i], len - i);
        send_set_response(link, invoke_id, class_id, instance_id, attribute_id, access_selector, write_len,(uint8_t *) &msg[i]);
        break;
    case 2:
      //Set request with First Datablock
        invoke_id = msg[i++];
        class_id = pack16(&msg[i]);
        i += 2;
        for (j = 0;  j < 6;  j++)
            instance_id[j] = msg[i++];
        attribute_id = msg[i++];
        access_selector = msg[i++];
        last_block=msg[i++];
        block_num=pack32(&msg[i]);
        i+=4;
        write_len=msg[i++];
        send_set_response_datablock(link, invoke_id, class_id, instance_id, attribute_id, access_selector, write_len,(uint8_t *) &msg[i],2, block_num, last_block);
        break;

    case 3:
        // Set request with Datablock
       __asm__ __volatile__ (" NOP");
        invoke_id = msg[i++];
        last_block=msg[i++];
        block_num=pack32(&msg[i]);
        i+=4;
        write_len=msg[i++];
        send_set_response_datablock(link, invoke_id, class_id, instance_id, attribute_id, access_selector, write_len,(uint8_t *) &msg[i],3, block_num, last_block);
        break;

    

    case 4:
#if defined(LOG_PACKETS)
        printf("Set-request-with-list\n");
#endif
        break;
    case 5:
#if defined(LOG_PACKETS)
        printf("Set-request-with-list-and-first-datablock\n");
#endif
        break;
    default:
        invoke_id = msg[i];
        send_set_response(link, invoke_id, 999, instance_id, attribute_id, access_selector,0,NULL);
        break;
    }
}
/*- End of function --------------------------------------------------------*/



void send_action_response(iec62056_46_link_t *link,
                          uint8_t invoke_id,
                          uint16_t class_id,
                          const uint8_t instance_id[6],
                          uint8_t method_id,
                          uint16_t data_len,
                          const uint8_t *method_data)
{
    //async_hdlc_tx_t tx;
    uint8_t response[128];
    int obj;
    int element;
    int len;
    uint16_t response_data_len;
#if defined(LOG_PACKETS)
    printf("send Action-Response\n");
#endif
    link_xxx = link;
    response[0] = PDU_ACTION_RESPONSE;
    response[1] = 1;    /* Normal */
    response[2] = invoke_id;
    response[3] = ACTION_RESULT_OBJECT_UNDEFINED;
    response[4] = 0;    /* No data present */
    len = 5;
    
    if((!(class_id==CLASS_ID_ASSOCIATION_LN && method_id==0x01))&&(*authentications[link->association]==2)&&(Authorised==NO)) 
     {
       response[3] = ACCESS_RESULT_SCOPE_OF_ACCESS_VIOLATED;
     }  
     else

    if (class_id != 999)
    {
        obj = locate_object_method(link, class_id, instance_id, method_id, &element);
        if (obj >= 0)
        {
            /* We found a match for the object */
            if ((object_list[obj].methods[element].access_mode & (ATTR_READ << 3*(3 - link->association))))
            {
                /* Its available for use */
                if (object_list[obj].methods[element].callback)
                    object_list[obj].methods[element].callback((void *)method_data,data_len,response+4,&response_data_len);
                response[3] = ACCESS_RESULT_SUCCESS;
                if(response_data_len!=0xffff)
                {
                  len+=response_data_len;
                }
                else
                {
                  response[3] = ACCESS_RESULT_OTHER_REASON;
                }  
            }
            else
            {
                response[3] = ACCESS_RESULT_READ_WRITE_DENIED;
            }
        }
    }
    hdlc_async_tx_build_iframe(&tx[link->port], link, TRUE, FALSE, response, len);
#if defined(LOG_PACKETS)
    printf("Tx len is %d\n", tx.msg_len);
#endif
    finalise_and_send_iframe(link, &tx[link->port]);
}
/*- End of function --------------------------------------------------------*/

static void process_action_request(iec62056_46_link_t *link, const uint8_t msg[], int len)
{
    int i;
    int j;
    uint8_t invoke_id;
    uint16_t class_id;
    uint8_t instance_id[6];
    int8_t method_id;
//    int32_t block_no;
    uint16_t data_len;

    if (!link->configured)
    {
        //send_confirmed_service_error(link, CONFIRMED_SERVICE_ERROR_READ, SERVICE_ERROR_SERVICE, 2);
        return;
    }
    i = 0;
    switch (msg[i++])
    {
    case 1:
#if defined(LOG_PACKETS)
        printf("Action-request-normal - ");
#endif
        invoke_id = msg[i++];
        class_id = pack16(&msg[i]);
        i += 2;
        for (j = 0;  j < 6;  j++)
            instance_id[j] = msg[i++];
        method_id = msg[i++];
#if defined(LOG_PACKETS)
        printf("priority %d, invoke ID 0x%X, ", (invoke_id & 0x80) >> 7, invoke_id & 0x7F);
        printf("class-id %s (%d), ", class_id_to_str(class_id), class_id);
        printf("instance-id ");
        display_obis_name(instance_id);
        printf("method-id %d\n", method_id);
#endif
        if (msg[i++])
        {
          
            /* Get any method-invocation-parameters */
            data_len=extract_cosem_asn1(&msg[i], len - i);
        }
        send_action_response(link, invoke_id, class_id, instance_id, method_id,data_len,&msg[i]);
        break;
    case 2:
#if defined(LOG_PACKETS)
        printf("Action-request-next-pblock\n");
#endif
        invoke_id = msg[i++];
//        block_no = pack32(&msg[i]);
        i += 4;
        break;
    case 3:
#if defined(LOG_PACKETS)
        printf("Action-request-with-list\n");
#endif
        invoke_id = msg[i++];
        break;
    case 4:
#if defined(LOG_PACKETS)
        printf("Action-request-with-first-pblock\n");
#endif
        invoke_id = msg[i++];
        class_id = pack16(&msg[i]);
        i += 2;
        for (j = 0;  j < 6;  j++)
            instance_id[j] = msg[i++];
        method_id = msg[i++];
        break;
    case 5:
#if defined(LOG_PACKETS)
        printf("Action-request-with-list-and-first-pblock\n");
#endif
        invoke_id = msg[i++];
        break;
    case 6:
#if defined(LOG_PACKETS)
        printf("Action-request-with-pblock\n");
#endif
        invoke_id = msg[i++];
        break;
    }
}
/*- End of function --------------------------------------------------------*/



static void process_event_notification_request(iec62056_46_link_t *link, const uint8_t msg[], int len)
{
    int i;
//    int8_t attribute_id;

#if defined(LOG_PACKETS)
    printf("Event-notification-request - ");
#endif
    if (!link->configured)
    {
        send_confirmed_service_error(link, CONFIRMED_SERVICE_ERROR_READ, SERVICE_ERROR_SERVICE, 2);
        return;
    }
    i = 0;
//    attribute_id = msg[i];
    i++;
    /* TODO */

#if defined(LOG_PACKETS)
    printf("attribute-id %d\n", attribute_id);
#endif
}
/*- End of function --------------------------------------------------------*/


void process_i_body(iec62056_46_link_t *link, const uint8_t msg[], int len)
{
    uint8_t msg_type;
    
    /* Check the LLC field */
    /* The COSEM spec says the destination LSAP (first byte) is always 0xE6; the
       source LSAP (second byte) is 0xE6 for a request, and 0xE7 for a response;
       The control byte (third byte) is always zero */
    if (len < 4
        ||
        msg[0] != IEC62056_46_LSAP_SOURCE
        ||
        msg[1] != IEC62056_46_LSAP_DEST_REQUEST
        ||
        msg[2] != 0x00)
    {
        /* If this section isn't recognisable, or is a response, ignore the message */
        /* A server only expects requests. Not responses. */
        return;
    }

#if defined(ANALYZE_PDU)
    analyze_pdu(&msg[3], len - 3);
#endif

    msg_type = msg[3];
    msg += 4;
    len -= 4;
    switch (msg_type)
    {
    case PDU_INITIATE_REQUEST:
        return;

    case PDU_READ_REQUEST:
        if ((negotiated_conformance[0] & 0x10))
        {
            process_read_request(link, msg, len);
            return;
        }
        break;

    case PDU_WRITE_REQUEST:
        if ((negotiated_conformance[0] & 0x08))
        {
            process_write_request(link, msg, len);
            return;
        }
        break;

    case PDU_UNCONFIRMED_WRITE_REQUEST:
        if ((negotiated_conformance[0] & 0x04))
        {
            return;
        }
        break;

    case PDU_INFORMATION_REPORT_REQUEST:
        if ((negotiated_conformance[1] & 0x01))
        {
            return;
        }
        break;

    case PDU_AARQ:
        /* If the length of the AARQ message does not match the length of the entire message
           this message is bogus. Ignore it. */
        if (msg[0] == len - 1)
        {
            process_aarq(link, &msg[1], msg[0]);
            return;
        }
        break;

    case PDU_GET_REQUEST:
//printf("Get %x\n", negotiated_conformance[1]);
        if ((negotiated_conformance[2] & 0x10))
        {
            process_get_request(link, msg, len);
            return;
        }
        else
        {
         __asm__ __volatile__ (" NOP");
        } 
        break;

    case PDU_SET_REQUEST:
        if ((negotiated_conformance[2] & 0x08))
        {
            process_set_request(link, msg, len);
            return;
        }
        break;

    case PDU_EVENT_NOTIFICATION_REQUEST:
        if ((negotiated_conformance[2] & 0x02))
        {
            process_event_notification_request(link, msg, len);
            return;
        }
        break;

    case PDU_ACTION_REQUEST:
        if ((negotiated_conformance[2] & 0x01))
        {
            process_action_request(link, msg, len);
            return;
        }
        break;

    case PDU_GLO_GET_REQUEST:
        //if ((negotiated_conformance[2] & 0x10))
        {
            return;
        }
        break;

    case PDU_GLO_SET_REQUEST:
        if ((negotiated_conformance[2] & 0x08))
        {
            return;
        }
        break;

    case PDU_GLO_EVENT_NOTIFICATION_REQUEST:
        return;
    case PDU_GLO_ACTION_REQUEST:
        return;

    case PDU_DED_GET_REQUEST:
        if ((negotiated_conformance[2] & 0x10))
        {
            return;
        }
        break;

    case PDU_DED_SET_REQUEST:
        if ((negotiated_conformance[2] & 0x08))
        {
            return;
        }
        break;

    case PDU_DED_EVENT_NOTIFICATION_REQUEST:
        return;
    case PDU_DED_ACTION_REQUEST:
        return;
    case PDU_RLRQ:
    	//end of plc communication
    	release_plc_communication(link->port);
    	break;

    default:
     __asm__ __volatile__ (" NOP");
    }
    /* Unrecognised message */
    send_confirmed_service_error(link, CONFIRMED_SERVICE_ERROR_READ, SERVICE_ERROR_SERVICE, 2);
}
/*- End of function --------------------------------------------------------*/

extern void aes_encrypt(unsigned char *,unsigned char *);
void aes_decrypt(unsigned char *,unsigned char *);
extern uint16_t challenge_c2s_len,challenge_s2c_len;
extern uint8_t challenge_c2s[16],reply_c2s[16];
extern uint8_t challenge_s2c[16];

extern uint8_t auth2[]; 
extern uint8_t auth3[];


void reply_to_hls_auth(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{

  int i;
  
  // encrypt the challenge that was sent to the Client
  // for(i=0;i<(challenge_s2c_len/16);i++)
   aes_encrypt(challenge_s2c ,auth2+2); 
  // encrypt the challenge that was sent by the Client
  for(i=0;i<(challenge_c2s_len/16);i++)
  { aes_encrypt(challenge_c2s+i*16,auth2+2); }
 /* 
  // Copy the reply sent by the Client
  reply_len=*data;
  for(i=0;i<reply_len;i++)
  { reply[i]=*(data+i); } */

    memcpy(reply_c2s,data+2,16);
  
  // Compare measured with actual
  if(memcmp(reply_c2s,challenge_s2c,challenge_c2s_len)==0)
  {
   __asm__ __volatile__ (" NOP");
    Authorised=YES;
    
  }  
  else
  {
    Authorised=NO;
    *response_len=0xffff;
    return;
  }  
  
     response[0] = 0x01;
               // response[5] = 0x01;
   response[1] = TAG_OCTET_STRING;    /* Data present */
   response[2] = 16;
   *response_len = 18;
                //type_copy(&response[5], object_list[obj].attrs[element].addr, object_list[obj].attrs[element].type);
                memcpy(response+3,challenge_c2s,16);

  
 /* *data=TAG_OCTET_STRING;
  *(data+1)=16;
  //send the answer back to the client
  memcpy(challenge_c2s, data+2, challenge_c2s_len);
  */
     aes_decrypt(challenge_s2c,auth2+2); 
    
}

/*- End of file ------------------------------------------------------------*/

void reply_to_hls_auth_MS(uint8_t *data,uint16_t data_len,uint8_t *response,uint16_t *response_len)
{

  int i;
  
  // encrypt the challenge that was sent to the Client
  // for(i=0;i<(challenge_s2c_len/16);i++)
   aes_encrypt(challenge_s2c ,auth3+2); 
  // encrypt the challenge that was sent by the Client
  for(i=0;i<(challenge_c2s_len/16);i++)
  { aes_encrypt(challenge_c2s+i*16,auth3+2); }
 /* 
  // Copy the reply sent by the Client
  reply_len=*data;
  for(i=0;i<reply_len;i++)
  { reply[i]=*(data+i); } */

    memcpy(reply_c2s,data+2,16);
  
  // Compare measured with actual
  if(memcmp(reply_c2s,challenge_s2c,challenge_c2s_len)==0)
  {
   __asm__ __volatile__ (" NOP");
    Authorised=YES;
    
  }  
  else
  {
    Authorised=NO;
    *response_len=0xffff;
    return;
  }  
  
     response[0] = 0x01;
               // response[5] = 0x01;
   response[1] = TAG_OCTET_STRING;    /* Data present */
   response[2] = 16;
   *response_len = 18;
                //type_copy(&response[5], object_list[obj].attrs[element].addr, object_list[obj].attrs[element].type);
                memcpy(response+3,challenge_c2s,16);

  
 /* *data=TAG_OCTET_STRING;
  *(data+1)=16;
  //send the answer back to the client
  memcpy(challenge_c2s, data+2, challenge_c2s_len);
  */
     aes_decrypt(challenge_s2c,auth3+2); 
    
}

/*- End of file ------------------------------------------------------------*/
