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
//  File: iec62056_46_link.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: iec62056_46_link.c,v 1.14 2013/01/30 05:36:31 a0393626 Exp $
//
//
//--------------------------------------------------------------------------
//
//#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#if defined(LOG_PACKETS)
#include <sys/time.h>
#endif

#include "cosem.h"
#include "obis.h"
#include "iec62056_46_link.h"
#include "iec62056_46_user.h"
#include "../uart_comms.h"
extern async_hdlc_tx_t tx[];

#ifndef IEC_PORT
#define IEC_PORT 1 
#endif 

#ifndef PLC_PORT
#define PLC_PORT 0
#endif 


#define FALSE 0
#define TRUE (!FALSE)

#define MAX_SEGGY_LEN   192

extern async_hdlc_tx_t queued_tx;

uint16_t Dest_MSAP_Len=0, Source_MSAP_Len=0;
/* States for the byte by byte packet receive analysis. We need to do things very incrementally, because
   of the poor design on the packet structure, so we end up with lots of states. */

extern uint16_t Chunk_Size;
extern uint8_t Frame_Received[];
enum
{
    ASYNC_RX_STATE_IDLE = 0,
    ASYNC_RX_STATE_FLAGGED,
    ASYNC_RX_STATE_AT_LEN,
    ASYNC_RX_STATE_AT_DEST_MSAP,
    ASYNC_RX_STATE_AT_SOURCE_MSAP,
    ASYNC_RX_STATE_AT_CTRL,
    ASYNC_RX_STATE_AT_HDR_CRC_A,
    ASYNC_RX_STATE_AT_HDR_CRC_B,
    ASYNC_RX_STATE_AT_BODY,
    ASYNC_RX_PROTOCOL_E_STEP1,
    ASYNC_RX_PROTOCOL_E_STEP2,
    ASYNC_RX_PROTOCOL_E_STEP3,
    ASYNC_RX_PROTOCOL_E_STEP4,
    ASYNC_RX_PROTOCOL_E_STEP11,
    ASYNC_RX_PROTOCOL_E_STEP12,
    ASYNC_RX_PROTOCOL_E_STEP13,
    ASYNC_RX_PROTOCOL_E_STEP14
};

/* CRC lookup table. This consumes significant memory, but CRC calculations are
   rather slow without the acceleration this provides. */
const uint16_t crc_itu16_table[] =
{
    0x0000, 0x1189, 0x2312, 0x329B, 0x4624, 0x57AD, 0x6536, 0x74BF,
    0x8C48, 0x9DC1, 0xAF5A, 0xBED3, 0xCA6C, 0xDBE5, 0xE97E, 0xF8F7,
    0x1081, 0x0108, 0x3393, 0x221A, 0x56A5, 0x472C, 0x75B7, 0x643E,
    0x9CC9, 0x8D40, 0xBFDB, 0xAE52, 0xDAED, 0xCB64, 0xF9FF, 0xE876,
    0x2102, 0x308B, 0x0210, 0x1399, 0x6726, 0x76AF, 0x4434, 0x55BD,
    0xAD4A, 0xBCC3, 0x8E58, 0x9FD1, 0xEB6E, 0xFAE7, 0xC87C, 0xD9F5,
    0x3183, 0x200A, 0x1291, 0x0318, 0x77A7, 0x662E, 0x54B5, 0x453C,
    0xBDCB, 0xAC42, 0x9ED9, 0x8F50, 0xFBEF, 0xEA66, 0xD8FD, 0xC974,
    0x4204, 0x538D, 0x6116, 0x709F, 0x0420, 0x15A9, 0x2732, 0x36BB,
    0xCE4C, 0xDFC5, 0xED5E, 0xFCD7, 0x8868, 0x99E1, 0xAB7A, 0xBAF3,
    0x5285, 0x430C, 0x7197, 0x601E, 0x14A1, 0x0528, 0x37B3, 0x263A,
    0xDECD, 0xCF44, 0xFDDF, 0xEC56, 0x98E9, 0x8960, 0xBBFB, 0xAA72,
    0x6306, 0x728F, 0x4014, 0x519D, 0x2522, 0x34AB, 0x0630, 0x17B9,
    0xEF4E, 0xFEC7, 0xCC5C, 0xDDD5, 0xA96A, 0xB8E3, 0x8A78, 0x9BF1,
    0x7387, 0x620E, 0x5095, 0x411C, 0x35A3, 0x242A, 0x16B1, 0x0738,
    0xFFCF, 0xEE46, 0xDCDD, 0xCD54, 0xB9EB, 0xA862, 0x9AF9, 0x8B70,
    0x8408, 0x9581, 0xA71A, 0xB693, 0xC22C, 0xD3A5, 0xE13E, 0xF0B7,
    0x0840, 0x19C9, 0x2B52, 0x3ADB, 0x4E64, 0x5FED, 0x6D76, 0x7CFF,
    0x9489, 0x8500, 0xB79B, 0xA612, 0xD2AD, 0xC324, 0xF1BF, 0xE036,
    0x18C1, 0x0948, 0x3BD3, 0x2A5A, 0x5EE5, 0x4F6C, 0x7DF7, 0x6C7E,
    0xA50A, 0xB483, 0x8618, 0x9791, 0xE32E, 0xF2A7, 0xC03C, 0xD1B5,
    0x2942, 0x38CB, 0x0A50, 0x1BD9, 0x6F66, 0x7EEF, 0x4C74, 0x5DFD,
    0xB58B, 0xA402, 0x9699, 0x8710, 0xF3AF, 0xE226, 0xD0BD, 0xC134,
    0x39C3, 0x284A, 0x1AD1, 0x0B58, 0x7FE7, 0x6E6E, 0x5CF5, 0x4D7C,
    0xC60C, 0xD785, 0xE51E, 0xF497, 0x8028, 0x91A1, 0xA33A, 0xB2B3,
    0x4A44, 0x5BCD, 0x6956, 0x78DF, 0x0C60, 0x1DE9, 0x2F72, 0x3EFB,
    0xD68D, 0xC704, 0xF59F, 0xE416, 0x90A9, 0x8120, 0xB3BB, 0xA232,
    0x5AC5, 0x4B4C, 0x79D7, 0x685E, 0x1CE1, 0x0D68, 0x3FF3, 0x2E7A,
    0xE70E, 0xF687, 0xC41C, 0xD595, 0xA12A, 0xB0A3, 0x8238, 0x93B1,
    0x6B46, 0x7ACF, 0x4854, 0x59DD, 0x2D62, 0x3CEB, 0x0E70, 0x1FF9,
    0xF78F, 0xE606, 0xD49D, 0xC514, 0xB1AB, 0xA022, 0x92B9, 0x8330,
    0x7BC7, 0x6A4E, 0x58D5, 0x495C, 0x3DE3, 0x2C6A, 0x1EF1, 0x0F78
};

uint16_t crc_itu16(uint8_t octet, uint16_t crc)
{
    crc = (crc >> 8) ^ crc_itu16_table[(crc ^ octet) & 0xFF];
    return crc;
}
/*- End of function --------------------------------------------------------*/

#if defined(LOG_PACKETS)
void log_header(int32_t source_msap, int32_t dest_msap, int segmented, uint8_t control_field)
{
    struct timeval tv;
    uint64_t current;

    gettimeofday(&tv, NULL);
    current = (uint64_t) tv.tv_sec*1000000ULL + (uint64_t) tv.tv_usec;

    printf("%11llu.%06llu - ", current/1000000ULL, current%1000000ULL);
    printf("Addresses 0x%X (0x%X/0x%X) -> 0x%X (0x%X/0x%X)\n",
           source_msap,
           ((source_msap >> 14) & 0x3FFF),
           (source_msap & 0x3FFF),
           dest_msap,
           ((dest_msap >> 14) & 0x3FFF),
           (dest_msap & 0x3FFF));
    printf("Segmented %d, frame type 0x%X\n", segmented, control_field);
}
/*- End of function --------------------------------------------------------*/
#endif

void finalise_and_send_frame(iec62056_46_link_t *link, async_hdlc_tx_t *tx)
{
  if(link->port != PLC_PORT)//Don Added this 
     hdlc_async_tx_end(tx);
  
    send_frame(link, tx);
}
/*- End of function --------------------------------------------------------*/

void finalise_and_send_iframe(iec62056_46_link_t *link, async_hdlc_tx_t *tx)
{
#if defined(ANALYZE_PDU)
    if (tx->hdr_crc_pos)
        analyze_pdu(&tx->msg[tx->hdr_crc_pos + 2 + 3], tx->msg_len - (tx->hdr_crc_pos + 2 + 3));
#endif
    finalise_and_send_frame(link, tx);
}
/*- End of function --------------------------------------------------------*/

static void send_rr(iec62056_46_link_t *link)
{
    //async_hdlc_tx_t tx;
    int poll_final_bit;

#if defined(LOG_PACKETS)
    printf("send RR\n");
#endif
    poll_final_bit = 0x10;
    hdlc_async_tx_build(&tx[link->port], link->far_msap, link->local_msap, (link->next_nr << 5) | poll_final_bit | 0x01, NULL, 0);
    finalise_and_send_frame(link, &tx[link->port]);
}
/*- End of function --------------------------------------------------------*/

#if 0
static void send_rnr(iec62056_46_link_t *link)
{
    //async_hdlc_tx_t tx;
    int poll_final_bit;

#if defined(LOG_PACKETS)
    printf("send RNR\n");
#endif
    poll_final_bit = 0x10;
    hdlc_async_tx_build(&tx[link->port], link->far_msap, link->local_msap, (link->next_nr << 5) | poll_final_bit | 0x05, NULL, 0);
    finalise_and_send_frame(link, &tx[link->port]);
}
/*- End of function --------------------------------------------------------*/
#endif

void send_snrm(iec62056_46_link_t *link)
{
    //async_hdlc_tx_t tx;
    int poll_bit;

#if defined(LOG_PACKETS)
    printf("send SNRM - 0x%X to 0x%X\n", link->local_msap, link->far_msap);
#endif
    poll_bit = 0x10;
    hdlc_async_tx_build(&tx[link->port], link->far_msap, link->local_msap, poll_bit | UFRAME_TYPE_SNRM, NULL, 0);
    finalise_and_send_frame(link, &tx[link->port]);
}
/*- End of function --------------------------------------------------------*/

void send_disc(iec62056_46_link_t *link)
{
    //async_hdlc_tx_t tx;
    int poll_bit;

#if defined(LOG_PACKETS)
    printf("send DISC\n");
#endif
    poll_bit = 0x10;
    hdlc_async_tx_build(&tx[link->port], link->far_msap, link->local_msap, poll_bit | UFRAME_TYPE_DISC, NULL, 0);
    finalise_and_send_frame(link, &tx[link->port]);
}
/*- End of function --------------------------------------------------------*/

void send_ua(iec62056_46_link_t *link)
{
    /* Use a preconfigured message body, as we don't normally want to change it */
    static const uint8_t body[] =
    {
        IEC62056_46_UA_FORMAT_IDENTIFIER,
        IEC62056_46_UA_GROUP_IDENTIFIER,
        20, /* Length */
            TLV_INT2_TO_BYTES(IEC62056_46_UA_TX_INFO, MAX_TX_LEN),
            TLV_INT2_TO_BYTES(IEC62056_46_UA_RX_INFO, MAX_RX_LEN),
            TLV_INT4_TO_BYTES(IEC62056_46_UA_TX_WINDOW, TX_WINDOW),
            TLV_INT4_TO_BYTES(IEC62056_46_UA_RX_WINDOW, RX_WINDOW)
    };
    //async_hdlc_tx_t tx;
    int final_bit;

#if defined(LOG_PACKETS)
    printf("send UA\n");
#endif
    final_bit = 0x10;
    hdlc_async_tx_build(&tx[link->port], link->far_msap, link->local_msap, final_bit | UFRAME_TYPE_UA, body, sizeof(body));
    finalise_and_send_frame(link, &tx[link->port]);
}
/*- End of function --------------------------------------------------------*/

void send_dm(iec62056_46_link_t *link)
{
    //async_hdlc_tx_t tx;
    int final_bit;

#if defined(LOG_PACKETS)
    printf("send DM\n");
#endif
    final_bit = 0x10;
    hdlc_async_tx_build(&tx[link->port], link->far_msap, link->local_msap, final_bit | UFRAME_TYPE_DM, NULL, 0);
    finalise_and_send_frame(link, &tx[link->port]);
}
/*- End of function --------------------------------------------------------*/

void send_frmr(iec62056_46_link_t *link)
{
    //async_hdlc_tx_t tx;
    int final_bit;

#if defined(LOG_PACKETS)
    printf("send FRMR\n");
#endif
    final_bit = 0x10;
    hdlc_async_tx_build(&tx[link->port], link->far_msap, link->local_msap, final_bit | UFRAME_TYPE_FRMR, NULL, 0);
    finalise_and_send_frame(link, &tx[link->port]);
}
/*- End of function --------------------------------------------------------*/

void send_ui(iec62056_46_link_t *link)
{
    //async_hdlc_tx_t tx;
    int poll_final_bit;

#if defined(LOG_PACKETS)
    printf("send UI\n");
#endif
    poll_final_bit = 0x10;
    hdlc_async_tx_build(&tx[link->port], link->far_msap, link->local_msap, poll_final_bit | UFRAME_TYPE_UI, NULL, 0);
    finalise_and_send_frame(link, &tx[link->port]);
}
/*- End of function --------------------------------------------------------*/

static int check_rx_nr(iec62056_46_link_t *link, int8_t nr)
{
    if (link->next_ns != nr)
    {
        /* They appear to be acknowledging I frames we have sent */
        if (nr == ((link->next_ns + 1) & 0x7)
            &&
                ((queued_tx.msg_len == 0  &&  nr == link->ns)
                 ||
                 (queued_tx.msg_len != 0  &&  ((nr + 1) & 0x7) == link->ns)))
        {
            link->next_ns = nr;
            if (queued_tx.msg_len > 0)
            {
#if defined(LOG_PACKETS)
                printf("Sending queued frame\n");
#endif
                finalise_and_send_frame(link, &queued_tx);
                queued_tx.msg_len = 0;
                return -1;
            }
        }
        else
        {
#if defined(LOG_PACKETS)
            printf("nr seems to have jumped - %d %d %d\n", link->ns, link->next_ns, nr);
#endif
            send_frmr(link);
            return -1;
        }
    }
    return 0;
}
/*- End of function --------------------------------------------------------*/

int seggy_len = 0;
uint8_t seggy[MAX_SEGGY_LEN];

int iec62056_46_check_and_set_far_msap(iec62056_46_link_t *link, uint32_t far_msap)
{
    extern uint32_t known_far_msap_list[];
    int i;

    /* See if the msap is in this systems list of recognised remote msaps. If it is, make
       this the current msap, and set the association number to the position of the msap
       in the list. The list is terminated by a zero entry. */
    for (i = 0;  known_far_msap_list[i];  i++)
    {
        if (far_msap == known_far_msap_list[i])
        {
            link->association = i;
            link->far_msap = far_msap;
            return 0;
        }
    }
    return -1;
}
/*- End of function --------------------------------------------------------*/

void process_rx_frame(iec62056_46_link_t *link, async_hdlc_rx_t *rx)
{
  if(link->port == PLC_PORT)
  {
    process_i_body(link, rx->msg, rx->msg_len);
  }
  else {
    uint8_t frame_type;
    int nr;
    int ns;
#if defined(LOG_PACKETS)
    int pf;
    int k;
#endif

#if defined(LOG_PACKETS)
    printf("Rx frame:\n");
    log_header(rx->source_msap, rx->dest_msap, rx->segmented, rx->control_field);
    printf("Body (%d) ", rx->msg_len);
    for (k = 0;  k < rx->msg_len;  k++)
        printf("%02x ", rx->msg[k]);
    printf("\n");
#endif
#if !defined(PROMISCUOUS)
    if (rx->dest_msap != link->local_msap)
        return;
    if (link->far_msap == 0)
    {
        /* See if the proposed far_msap is one we recognise */
        if (iec62056_46_check_and_set_far_msap(link, rx->source_msap))
            return;
    }
    else
    {
        /* Once we leave the disconnected state, we need to have the far end address of this link locked down. */
        if (rx->source_msap != link->far_msap)
            return;
    }
#endif
    frame_type = rx->control_field;
#if defined(LOG_PACKETS)
    pf = (frame_type >> 4) & 0x01;
#endif
    if ((frame_type & 0x01) == 0x00)
    {
#if !defined(PROMISCUOUS)
          if (rx->msg_len > MAX_RX_LEN)
            {
              asm("NOP");
              send_frmr(link);
            }  

        if (link->disconnected)
        {
            send_dm(link);
        }
        else
        {
            /* Information frame. */
            nr = (frame_type >> 5) & 0x07;
            ns = (frame_type >> 1) & 0x07;
#if defined(LOG_PACKETS)
            printf("I frame, rx seq = %d, tx seq = %d, P/F = %d\n", nr, ns, pf);
#endif
            if (check_rx_nr(link, nr) == 0)
            {
                if (link->next_nr == ns)
                {
#if defined(LOG_PACKETS)
printf("This appears to be a new I-frame\n");
#endif
                    /* This is the next frame in sequence, so acknowledge and process. */
                    link->next_nr = (link->next_nr + 1) & 7;
                    if (rx->segmented)
                    {
                        if (seggy_len + rx->msg_len <= MAX_SEGGY_LEN)
                        {
                            memcpy(&seggy[seggy_len], rx->msg, rx->msg_len);
                            seggy_len += rx->msg_len;
                        }
                        else
                        {
                            seggy_len = MAX_SEGGY_LEN + 1;
                        }
                        send_rr(link);
                    }
                    else
                    {
                        if (seggy_len)
                        {
                            if (seggy_len + rx->msg_len <= MAX_SEGGY_LEN)
                            {
                                memcpy(&seggy[seggy_len], rx->msg, rx->msg_len);
                                seggy_len += rx->msg_len;
                                process_i_body(link, seggy, seggy_len);
                            }
                            seggy_len = 0;
                        }
                        else
                        {
                            process_i_body(link, rx->msg, rx->msg_len);
                        }
                    }
                }
                else
                {
                    if (ns != ((link->next_nr - 1) & 0x7))
                        send_rr(link);
                }
            }
        }
#else
        process_i_body(link, rx->msg, rx->msg_len);
#endif
    }
    else if ((frame_type & 0x03) == 0x01) //RR and RNR frames 
    {
        /* Supervisory frame. These do NOT contain an information field. */
        nr = (frame_type >> 5) & 0x07;
        /* Only RR and RNR are defined in IEC62056. REJ and SREJ (bit 3 = 1) are not used. */
        if ((frame_type & 0x08) == 0)
        {
            if (check_rx_nr(link, nr) == 0)
            {
                if ((frame_type & 0x04))
                {
#if defined(LOG_PACKETS)
                    printf("RNR (receiver not ready) frame, seq = %d, P/F = %d\n", nr, pf);
#endif
                    link->ready = FALSE;
                }
                else
                {
#if defined(LOG_PACKETS)
                    printf("RR (receiver ready) frame, seq = %d, P/F = %d\n", nr, pf);
#endif
                    send_rr(link);
                    link->ready = TRUE;
                }
            }
        }
        else
        {
            send_frmr(link);
        }
    }
    else if ((frame_type & 0x03) == 0x03)// SNRA, DICS, UA, DM, FRMR,UI
    {
        /* Unnumbered frame. */
        /* HDLC defines the following unnumbered format messages, but IEC62056-46 uses only a subset of them.
           The ones not used in IEC62056-46 are:
            * Set Asynchronous Response Mode (SARM)
            * Set Asynchronous Balanced Mode (SABM)
            * Set Normal Response Mode Extended (SNRME)
            * Set Initialization Mode (SIM)
            * Unnumbered Poll (UP)
            * Exchange Identification (XID)
            * Reset (RSET)
            * Test (TEST)
            * Request Initialization Mode (RIM)
            * Request Disconnect (RD)
         */
        switch ((frame_type & 0xEF))
        {
        case UFRAME_TYPE_SNRM:
            /* This frame SHOULD contain an information field. */
#if defined(LOG_PACKETS)
            printf("SNRM (set normal response mode) frame, P = %d\n", pf);
#endif
            if (link->disconnected  &&  link->far_msap == 0)
                iec62056_46_set_far_msap(link, rx->source_msap);
            process_snrm_frame(link, rx->msg, rx->msg_len);
            break;
        case UFRAME_TYPE_DISC:
            /* This frame SHOULD NOT contain an information field. */
#if defined(LOG_PACKETS)
            printf("DISC (disconnect) frame, P = %d\n", pf);
#endif
            process_disc_frame(link, rx->msg, rx->msg_len);
            break;
        case UFRAME_TYPE_UA:
            /* This frame MAY contain an information field. */
#if defined(LOG_PACKETS)
            printf("UA (unnumbered acknowledge) frame, F = %d\n", pf);
#endif
            process_ua_frame(link, rx->msg, rx->msg_len);
            break;
        case UFRAME_TYPE_DM:
            /* This frame MAY contain an information field. */
#if defined(LOG_PACKETS)
            printf("DM (disconnected mode) frame, F = %d\n", pf);
#endif
            process_dm_frame(link, rx->msg, rx->msg_len);
            break;
        case UFRAME_TYPE_FRMR:
            /* This frame SHOULD contain an information field. */
#if defined(LOG_PACKETS)
            printf("FRMR (frame reject) frame, F = %d\n", pf);
#endif
            process_frmr_frame(link, rx->msg, rx->msg_len);
            break;
        case UFRAME_TYPE_UI:
            /* This frame SHOULD contain an information field. */
#if defined(LOG_PACKETS)
            printf("UI (unnumbered information) frame, P/F = %d\n", pf);
#endif
            process_ui_frame(link, rx->msg, rx->msg_len);
            break;
        default:
            /* Any frame type we aren't expecting is BAD */
            send_frmr(link);
            break;
        }
    }
#if defined(LOG_PACKETS)
    printf("\n\n\n");
#endif
  }
}
/*- End of function --------------------------------------------------------*/

void iec62056_46_link_init(iec62056_46_link_t *link, int port, uint32_t local_msap)
{
    link->next_nr = 0;
    link->next_ns = 0;
    link->ns = 0;
    link->disconnected = TRUE;
    link->configured = FALSE;
    link->ready = TRUE;
    link->max_tx_info = MAX_TX_LEN;
    link->max_rx_info = MAX_RX_LEN;
    link->tx_window = TX_WINDOW;
    link->rx_window = RX_WINDOW;
    link->local_msap = local_msap;
    link->far_msap = 0;
    link->port = port;
    /* Just schedule a timeout after a short delay, to get things running */
   // link->timeout_id = schedule_timeout(link, 1000);
}
/*- End of function --------------------------------------------------------*/

void iec62056_46_set_far_msap(iec62056_46_link_t *link, uint32_t far_msap)
{
    link->far_msap = far_msap;
}
/*- End of function --------------------------------------------------------*/

static void hdlc_async_rx_end_in_hdr(async_hdlc_rx_t *rx, uint8_t byte)
{
    /* We are here because the length count has been reached, but we are still in the
       frame header. Something must be wrong. We can test the CRC to see if we have
       received something which looks structurally OK, but we can't actually use what
       we received in any meaningful way. All we can do it check if we have received a
       flag, and on that basis decide whether to look for more flags, or start
       accepting a new frame. */
    if (byte != 0x7E)
    {
        /* The closing flag was not were it ought to be. */
#if defined(LOG_PACKETS)
        printf("Bad HDLC frame - %d bytes - 0x%X\n", rx->msg_len - 2, rx->crc);
#endif
        rx->state = ASYNC_RX_STATE_IDLE;
//P1OUT ^= BIT1;
        return;
    }
    if ((rx->crc & 0xFFFF) == 0xF0B8)
    {
#if defined(LOG_PACKETS)
        printf("Good HDLC frame with no body - %d bytes - 0x%X\n", rx->msg_len - 2, rx->crc);
#endif
    }
    else
    {
#if defined(LOG_PACKETS)
        printf("Bad HDLC frame - %d bytes - 0x%X\n", rx->msg_len - 2, rx->crc);
#endif
    }
    /* The flag we have just seen is a perfectly good start flag for the next
       frame, so we just go back to looking for the first byte of the next frame.
       There should probably be a timeout, so on non-continuous reception we
       demand another flag as reception resumes. */
    rx->state = ASYNC_RX_STATE_FLAGGED;
#if defined(__MSP430__)
    P1OUT ^= BIT0;
#endif
}
/*- End of function --------------------------------------------------------*/

void iec62056_46_link_idle_timeout(iec62056_46_link_t *link)
{
    dl_disconnect_indication(link, DL_DISCONNECT_INDICATION_LOCAL_DL);
    link->disconnected = TRUE;
    link->configured = FALSE;
    link->far_msap=0;
}
/*- End of function --------------------------------------------------------*/

void iec62056_46_rx_timeout(iec62056_46_link_t *link, async_hdlc_rx_t *rx)
{
    rx->state = ASYNC_RX_STATE_IDLE;
#if defined(__MSP430__)
    P1OUT ^= BIT1;
#endif
}
/*- End of function --------------------------------------------------------*/

void iec62056_46_rx_byte(iec62056_46_link_t *link, async_hdlc_rx_t *rx, uint8_t byte)
{
    //async_hdlc_tx_t tx;

    /* The strange design of the DLMS HDLC frame makes is messy to parse, and subject to
       possible trouble if its length field is corrupt. It could take a long time to
       recover if the length field erroneously contains a large number. To minimise the
       chance of this happening, we need to check the header CRC which, is inserted between
       the header and the body of the frame. Since the header is of variable length, and the
       CRC field is not delimited in any way, we need to incrementally analyse the header as
       it is received, try to figure out which are the CRC bytes, check them, and determine
       whether we should believe the length field is genuine. */
    switch (rx->state)
    {
    case ASYNC_RX_STATE_IDLE:
        /* Looking for a flag byte */
        if (byte == 0x7E)
        {
            rx->state = ASYNC_RX_STATE_FLAGGED;
#if defined(__MSP430__)
            P1OUT ^= BIT0;
#endif
            break;
        }
        if ((byte & 0x7F) == '/')
        {
            rx->state = ASYNC_RX_PROTOCOL_E_STEP1;
            break;
        }
        if ((byte & 0x7F) == 0x06)
        {
            rx->state = ASYNC_RX_PROTOCOL_E_STEP11;
            break;
        }
        break;
    case ASYNC_RX_PROTOCOL_E_STEP1:
        if ((byte & 0x7F) == '?')
        {
            rx->state = ASYNC_RX_PROTOCOL_E_STEP2;
            break;
        }
        rx->state = ASYNC_RX_STATE_IDLE;
#if defined(__MSP430__)
        //P1OUT ^= BIT1;
#endif
        break;
    case ASYNC_RX_PROTOCOL_E_STEP2:
        if ((byte & 0x7F) == '!')
        {
            rx->state = ASYNC_RX_PROTOCOL_E_STEP3;
            break;
        }
        if ((byte & 0x7F) >= '0'  &&  (byte & 0x7F) <= '9')
            break;
        rx->state = ASYNC_RX_STATE_IDLE;
#if defined(__MSP430__)
        //P1OUT ^= BIT1;
#endif
        break;
    case ASYNC_RX_PROTOCOL_E_STEP3:
        if ((byte & 0x7F) == '\r')
        {
            rx->state = ASYNC_RX_PROTOCOL_E_STEP4;
            break;
        }
        rx->state = ASYNC_RX_STATE_IDLE;
#if defined(__MSP430__)
        //P1OUT ^= BIT1;
#endif
        break;
    case ASYNC_RX_PROTOCOL_E_STEP4:
        if ((byte & 0x7F) == '\n')
        {
#if defined(LOG_PACKETS)
printf("Got a 'Switch to mode E' request\n");
#endif
            tx[link->port].msg[0] = '/';
            tx[link->port].msg[1] = 0xA0;
            tx[link->port].msg[2] = 'D';
            tx[link->port].msg[3] = 'V';
            tx[link->port].msg[4] = 'E';
            tx[link->port].msg[5] = '5'; //9600bd
            tx[link->port].msg[6] = '\\';
            tx[link->port].msg[7] = '2';
            tx[link->port].msg[8] = 'T';
            tx[link->port].msg[9] = 'I';
            tx[link->port].msg[10] = '\r';
            tx[link->port].msg[11] = '\n';
            tx[link->port].msg_len = 12;
            tx[link->port].msg_ptr=0;
            tx[link->port].hdr_crc_pos=0;;
            send_frame(link, &tx[link->port]);
        }
        rx->state = ASYNC_RX_STATE_IDLE;
#if defined(__MSP430__)
        //P1OUT ^= BIT1;
#endif
        break;
    case ASYNC_RX_PROTOCOL_E_STEP11:
        if ((byte & 0x7F) >= '0'  &&  (byte & 0x7F) <= '9')
            break;
        if ((byte & 0x7F) == '\r')
        {
            rx->state = ASYNC_RX_PROTOCOL_E_STEP12;
        }
        break;
    case ASYNC_RX_PROTOCOL_E_STEP12:
        if ((byte & 0x7F) == '\n')
        {
#if defined(LOG_PACKETS)
printf("Got a 'Switch to mode E' stage 2 request\n");
#endif
            configure_uart_port(link->port, 1);
            tx[link->port].msg[0] = 0x06;
            tx[link->port].msg[1] = 0xA0;
            tx[link->port].msg[2] = 0xB2;
            tx[link->port].msg[3] = 0xB5;
            tx[link->port].msg[4] = 0xB2;
            tx[link->port].msg[5] = 0x0D;
            tx[link->port].msg[6] = 0x0A;
            tx[link->port].msg_len = 7;
            tx[link->port].msg_ptr=0;
            send_frame(link, &tx[link->port]);
        }
        rx->state = ASYNC_RX_STATE_IDLE;
        configure_uart_port(link->port, 2);
#if defined(__MSP430__)
        P1OUT ^= BIT1;
#endif
        break;
    case ASYNC_RX_STATE_FLAGGED:
        /* The byte after the flag should always have 1010 in the top four bits. Therefore,
           more 0x7E bytes are sure to be more flags. */
        if (byte == 0x7E)
            break;
        if ((byte & 0xF0) != 0xA0)
        {
            /* The frame type bits are wrong. Back to hunting for a flag. */
            rx->state = ASYNC_RX_STATE_IDLE;
#if defined(__MSP430__)
            //P1OUT ^= BIT1;
#endif
            break;
        }
        /* This is the first byte of the frame type + length field. */
        rx->crc = crc_itu16(byte, 0xFFFF);
        rx->msg_len = 0;
        /* Initialise the header information. */
        rx->segmented = byte & 0x08;
        rx->expected_len = (byte & 0x7) << 8;
        rx->control_field = 0;
        rx->dest_msap = 0;
        rx->source_msap = 0;
        rx->dest_msap_len = 0;
        rx->source_msap_len = 0;
        rx->state = ASYNC_RX_STATE_AT_LEN;
        break;
    case ASYNC_RX_STATE_AT_LEN:
        /* This is the second byte of the frame type + length field. */
        /* The two frame type + length bytes are as follows:
                1 0 1 0 S L L L L L L L L L L L
           where L is a length bit, and S indicated segmentation when set.
           The 1010 part specifies an HDLC type 3 frame, which DLMS messages
           should always be.
         */
        rx->expected_len |= byte;
        rx->crc = crc_itu16(byte, rx->crc);
        rx->msg_len = 2;
        if (rx->expected_len < 4)
        {
            /* That's too short. Something is wrong. Give up on this frame. */
            rx->state = ASYNC_RX_STATE_IDLE;
#if defined(__MSP430__)
            //P1OUT ^= BIT1;
#endif
            break;
        }
        rx->state = ASYNC_RX_STATE_AT_DEST_MSAP;
        break;
    case ASYNC_RX_STATE_AT_DEST_MSAP:
    case ASYNC_RX_STATE_AT_SOURCE_MSAP:
        if (rx->msg_len >= rx->expected_len)
        {
            hdlc_async_rx_end_in_hdr(rx, byte);
            break;
        }
        if (rx->state == ASYNC_RX_STATE_AT_DEST_MSAP)
        {
            rx->dest_msap = (rx->dest_msap << 7) | (byte >> 1);
            rx->dest_msap_len++;
        }
        else
        {
            rx->source_msap = (rx->source_msap << 7) | (byte >> 1);
            rx->source_msap_len++;
        }
        rx->crc = crc_itu16(byte, rx->crc);
        rx->msg_len++;
        if (byte & 1)
        {
            if (rx->state == ASYNC_RX_STATE_AT_DEST_MSAP)
            {
                if (rx->dest_msap_len == 1)
                  { rx->dest_msap <<= 14;
                   rx->dest_msap+=(link->local_msap&0x0fff);} 
                else if (rx->dest_msap_len == 2)
                { rx->dest_msap = ((rx->dest_msap << 7) & 0x1FC000) | (rx->dest_msap & 0x7F);
                  rx->dest_msap+=(link->local_msap&0x0fff);}
            }
            else
            {
                if (rx->source_msap_len == 1)
                    rx->source_msap <<= 14;
                else if (rx->source_msap_len == 2)
                    rx->source_msap = ((rx->source_msap << 7) & 0x1FC000) | (rx->source_msap & 0x7F);
            }
            /* End of address marker - step to the next address, or the control field */
            rx->state++;
        }
        break;
    case ASYNC_RX_STATE_AT_CTRL:
        if (rx->msg_len >= rx->expected_len)
        {
            hdlc_async_rx_end_in_hdr(rx, byte);
            break;
        }
        rx->control_field = byte;
        rx->crc = crc_itu16(byte, rx->crc);
        rx->msg_len++;
        rx->state = ASYNC_RX_STATE_AT_HDR_CRC_A;
        break;
    case ASYNC_RX_STATE_AT_HDR_CRC_A:
    case ASYNC_RX_STATE_AT_HDR_CRC_B:
        if (rx->msg_len >= rx->expected_len)
        {
            hdlc_async_rx_end_in_hdr(rx, byte);
            break;
        }
        rx->crc = crc_itu16(byte, rx->crc);
        rx->msg_len++;
        if (rx->state == ASYNC_RX_STATE_AT_HDR_CRC_A)
        {
            rx->state = ASYNC_RX_STATE_AT_HDR_CRC_B;
        }
        else
        {
            if ((rx->crc & 0xFFFF) == 0xF0B8)
            {
#if defined(LOG_PACKETS)
                printf("Header CRC OK\n");
#endif
                /* Now adjust the length, so we look for the correct body length */
                rx->expected_len -= rx->msg_len;
                rx->msg_len = 0;
                rx->state = ASYNC_RX_STATE_AT_BODY;
            }
            else
            {
                /* If things are wrong at this point, we really need to look for
                   another flag byte before proceeding. */
                rx->state = ASYNC_RX_STATE_IDLE;
#if defined(__MSP430__)
                //P1OUT ^= BIT1;
#endif
#if defined(LOG_PACKETS)
                printf("Header CRC failed - 0x%X (%d)\n", rx->crc, rx->msg_len);
#endif
            }
        }
        break;
    case ASYNC_RX_STATE_AT_BODY:
        /* If the frame has no body (e.g. it is a supervisory frame), the header CRC is also
           the final CRC. This code needs to handle this situation correctly. */
        if (rx->msg_len >= rx->expected_len)
        {
            if (byte != 0x7E)
            {
                /* The closing flag was not were it ought to be. */
#if defined(LOG_PACKETS)
                printf("Bad HDLC frame - %d bytes - 0x%X\n", rx->msg_len - 2, rx->crc);
#endif
                
                rx->state = ASYNC_RX_STATE_IDLE;
#if defined(__MSP430__)
                //P1OUT ^= BIT1;
#endif
                break;
            }
            /* Only deduct the length of final CRC if there are two CRCs */
            if (rx->msg_len >= 2)
                rx->msg_len -= 2;
            if ((rx->crc & 0xFFFF) == 0xF0B8)
            {
                /* Deduct the CRC bytes from the length, to leave the actual body length */
#if defined(LOG_PACKETS)
                printf("Good HDLC frame - %d bytes (%d)\n", rx->msg_len, rx->expected_len - 2);
#endif
                rx->good_packets++;
                iec62056_46_link_idle_restart(link);
                Dest_MSAP_Len=rx->dest_msap_len;
                Source_MSAP_Len=rx->source_msap_len;
                Frame_Received[link->port]=1;
                //process_rx_frame(link, rx);
            }
            else
            {
#if defined(LOG_PACKETS)
                printf("Bad HDLC frame - %d bytes - 0x%X\n", rx->msg_len - 2, rx->crc);
#endif
            }
            /* The flag we have just seen is a perfectly good start flag for the next
               frame, so we just go back to looking for the first byte of the next frame.
               There should probably be a timeout, so on non-continuous reception we
               demand another flag as reception resumes. */
            rx->state = ASYNC_RX_STATE_FLAGGED;
#if defined(__MSP430__)
            P1OUT ^= BIT0;
#endif
            rx->crc = 0xFFFF;
            break;
        }
        rx->crc = crc_itu16(byte, rx->crc);
        if (rx->msg_len < MAX_RX_LEN+2) // +2 for CRC bytes
            rx->msg[rx->msg_len++] = byte;
        break;
    }
#if defined(__MSP430__)
#if 0
if (rx->state > 1)
    P1OUT |= BIT0;
else
    P1OUT &= ~BIT0;
#endif
#endif
 
}
/*- End of function --------------------------------------------------------*/

void hdlc_async_rx_init(async_hdlc_rx_t *rx)
{
    rx->state = ASYNC_RX_STATE_IDLE;
    rx->segmented = FALSE;
    rx->control_field = 0;
    rx->dest_msap = 0;
    rx->dest_msap_len = 0;
    rx->source_msap = 0;
    rx->source_msap_len = 0;
    seggy_len = 0;

    rx->good_packets = 0;
}
/*- End of function --------------------------------------------------------*/

static void hdlc_async_tx_add_address(async_hdlc_tx_t *tx, uint32_t address, uint16_t d_or_s)
{
  int msap_len;

  if(d_or_s)
  { msap_len=Dest_MSAP_Len;}
  else
  { msap_len=Source_MSAP_Len;}  
  /* The addresses are packed into a variable length string of bytes. Each byte
       contains 7 address bits in the top 7 bits. The LSB is set in the last byte
       of the byte string. */
    /* The spec only defines 3 cases for the output - 1, 2 and 4 bytes. Therefore, we
       can only represent 28 bits of address */
    if (msap_len==1)//((address & 0x0FE03FFF) == 0)
    {
        /* Lower (physical) address is zero, and the top 7 bits of the upper (logical)
           address are zero. Therefore 1 byte addressing may be used. */
        tx->msg[tx->msg_len++] = ((address >> 13) & 0xFE) | 0x01;
    }
    else if (msap_len==2)//((address & 0x0FE03F80) == 0)
    {
        /* The top 7 bits of both the lower (physical) address, and the upper (logical)
           address are zero. Therefore 2 byte addressing may be used. */
        tx->msg[tx->msg_len++] = (address >> 13) & 0xFE;
        tx->msg[tx->msg_len++] = ((address << 1) & 0xFE) | 0x01;
    }
    else
    {
        /* 4 byte addressing must be used */
        tx->msg[tx->msg_len++] = (address >> 20) & 0xFE;
        tx->msg[tx->msg_len++] = (address >> 13) & 0xFE;
        tx->msg[tx->msg_len++] = (address >> 6) & 0xFE;
        tx->msg[tx->msg_len++] = ((address << 1) & 0xFE) | 0x01;
    }
}
/*- End of function --------------------------------------------------------*/

int hdlc_async_tx_add_hdr_crc(async_hdlc_tx_t *tx)
{
    /* Make space for the header CRC. We will set the correct value later. */
    tx->hdr_crc_pos = tx->msg_len;
    tx->msg_len += 2;
    return tx->msg_len;
}
/*- End of function --------------------------------------------------------*/

int hdlc_async_tx_add_header(async_hdlc_tx_t *tx,
                             uint32_t dest_msap,
                             uint32_t source_msap,
                             uint8_t control_field)
{
    /* The frame header consists of:
        - The type+length field (already inserted at this point).
        - The destination address (1 to 32 bits).
        - The source address (1 to 32 bits).
        - The frame type or control field (a byte).
        - A 2 byte CRC.
     */
    hdlc_async_tx_add_address(tx, dest_msap, 0);
    hdlc_async_tx_add_address(tx, source_msap, 1 );
    tx->msg[tx->msg_len++] = control_field;
    return tx->msg_len;
}
/*- End of function --------------------------------------------------------*/

int hdlc_async_tx_add_body(async_hdlc_tx_t *tx, const uint8_t buf[], int len)
{
    int i;

    /* If we are inserting some body data, and there is no header CRC in the frame
       so far, we need to add one. */
    if (tx->hdr_crc_pos == 0)
        hdlc_async_tx_add_hdr_crc(tx);
    if (tx->msg_len + len > Chunk_Size + 32)
        return -1;
    for (i = 0;  i < len;  i++)
        tx->msg[tx->msg_len++] = buf[i];
    return tx->msg_len;
}
/*- End of function --------------------------------------------------------*/

int hdlc_async_tx_end(async_hdlc_tx_t *tx)
{
    int i;
    uint8_t byte;
    uint16_t word;
    uint16_t crc;

    /* Finalise the message in the transmit buffer - i.e. insert the correct length
       and CRC values. */

    /* We can only insert the length, and calculate and insert the 2 CRC fields
       now that the complete message is available for transmission. */

    /* Now we can fill in the length. It will be the current length + 2 bytes of
       CRC - the flag byte at the start of the buffer. */
    word = tx->msg_len + 2 - 1;
    tx->msg[1] |= (word >> 8);
    tx->msg[2] = word & 0xFF;
    
    crc = 0xFFFF;
    i = 1;
    if (tx->hdr_crc_pos > 0)
    {
        /* Calculate up to the header CRC position. Don't include the first byte in
           the buffer, as that is a flag. */
        for (  ;  i < tx->hdr_crc_pos;  i++)
            crc = crc_itu16(tx->msg[i], crc);
        /* Now we have the header CRC ready, and can insert it. */
        word = crc ^ 0xFFFF;
        tx->msg[tx->hdr_crc_pos] = word & 0xFF;
        tx->msg[tx->hdr_crc_pos + 1] = (word >> 8) & 0xFF;
    }
    /* Carry on calculating the rest of the CRC, including the 2 header CRC bytes
       we just inserted in this calculation. */
    for (  ;  i < tx->msg_len;  i++)
        crc = crc_itu16(tx->msg[i], crc);

    /* Append the CRC and the final flag octet, to complete the message. */
    crc ^= 0xFFFF;
    for (i = 0;  i < 2;  i++)
    {
        byte = crc & 0xFF;
        tx->msg[tx->msg_len++] = byte;
        crc >>= 8;
    }
    tx->msg[tx->msg_len++] = 0x7E;
    return tx->msg_len;
}
/*- End of function --------------------------------------------------------*/

void hdlc_async_tx_init(async_hdlc_tx_t *tx)
{
    tx->msg[0] = 0x7E;
    tx->msg[1] = 0xA0;
    /* Make space for the type+length. We will set the correct value later. */
    tx->msg_len = 3;
    tx->msg_ptr = 0;
    tx->hdr_crc_pos = 0;
}
/*- End of function --------------------------------------------------------*/

int hdlc_async_tx_build(async_hdlc_tx_t *tx,
                        uint32_t dest_msap,
                        uint32_t source_msap,
                        uint8_t control_field,
                        const uint8_t buf[],
                        int len)
{
#if defined(LOG_PACKETS)
    int k;

    printf("Tx: Build Body (%d) ", len);
    for (k = 0;  k < len;  k++)
        printf("%02x ", buf[k]);
    printf("\n");
    log_header(source_msap, dest_msap, FALSE, control_field);
#endif
    hdlc_async_tx_init(tx);
    hdlc_async_tx_add_header(tx, dest_msap, source_msap, control_field);
    if (len > 0)
        hdlc_async_tx_add_body(tx, buf, len);
    return tx->msg_len;
}
/*- End of function --------------------------------------------------------*/

int hdlc_async_tx_build_iframe(async_hdlc_tx_t *tx,
                               iec62056_46_link_t *link,
                               int poll_final_bit,
                               int request,
                               const uint8_t buf[],
                               int len)
{
  
  if(link->port == PLC_PORT)
  {
        tx->msg_len = 0;
        tx->msg_ptr = 0;
        tx->hdr_crc_pos = 5;    
        hdlc_async_tx_add_body(tx, (request)  ?  (uint8_t const *) "\xE6\xE6\x00"  :  (uint8_t const *) "\xE6\xE7\x00", 3);
        if (len > 0)
           hdlc_async_tx_add_body(tx, buf, len);
  }
  else
  {
        uint8_t control_field;
    #if defined(LOG_PACKETS)
        int k;
    #endif

        control_field = (link->next_nr << 5) | (link->ns << 1);
        if (poll_final_bit)
            control_field |= 0x10;
        link->ns = (link->ns + 1) & 0x7;

    #if defined(LOG_PACKETS)
        printf("Tx: Build2 Body (%d) ", len);
        for (k = 0;  k < len;  k++)
            printf("%02x ", buf[k]);
        printf("\n");
        log_header(link->local_msap, link->far_msap, FALSE, control_field);
    #endif
        hdlc_async_tx_init(tx);
        hdlc_async_tx_add_header(tx, link->far_msap, link->local_msap, control_field);
        hdlc_async_tx_add_body(tx, (request)  ?  (uint8_t const *) "\xE6\xE6\x00"  :  (uint8_t const *) "\xE6\xE7\x00", 3);
        if (len > 0)
            hdlc_async_tx_add_body(tx, buf, len);
  }
    return tx->msg_len;
}
/*- End of function --------------------------------------------------------*/

int hdlc_async_tx_build_iframe_chunk_add(async_hdlc_tx_t *tx,
                                         iec62056_46_link_t *link,
                                         const uint8_t buf[],
                                         int len)
{
    if (len > 0)
    {
        if (hdlc_async_tx_add_body(tx, buf, len) < 0)
            return -1;
    }
    return tx->msg_len;
}
/*- End of function --------------------------------------------------------*/

int iec62056_46_link_timeout(iec62056_46_link_t *link)
{
    if (link->disconnected)
    {
        dl_connect_confirm(link, DL_CONNECT_CONFIRM_NO_RESPONSE);
    }
    else
    {
#if defined(LOG_PACKETS)
        printf("Link timeout\n");
        if (link->configured  &&  link->ready)
            printf("Ready to send\n");
#endif
       // link->timeout_id = schedule_timeout(link, TO_WAIT_RESP);
    }
    return 0;
}
/*- End of function --------------------------------------------------------*/

int dl_connect_request(iec62056_46_link_t *link)
{
    send_snrm(link);
    //link->timeout_id = schedule_timeout(link, TO_WAIT_RESP);
    return 0;
}
/*- End of function --------------------------------------------------------*/
/*- End of file ------------------------------------------------------------*/
