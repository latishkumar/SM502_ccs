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
//  File: xframe.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: xframe.c,v 1.6 2011/07/11 10:44:58 a0393626 Exp $
//
//
//--------------------------------------------------------------------------
//
//#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include "cosem.h"
#include "obis.h"
#include "iec62056_46_link.h"
#include "iec62056_46_user.h"

#define FALSE 0
#define TRUE (!FALSE)

void process_ua_frame(iec62056_46_link_t *link, const uint8_t msg[], int len)
{
    int i;
    int j;
    int k;
    int format_identifier;
    int group_identifier;
    int group_length;
    int type;
    uint32_t value;

#if defined(LOG_PACKETS)
    printf("Body: ");
    for (i = 0;  i < len;  i++)
        printf("%02X ", msg[i]);
    printf("\n");
#endif

    /* IEC62056-46/6.4.4.4.3.1 */
    for (i = 0;  i < len;  i++)
    {
        format_identifier = msg[i++];
        group_identifier = msg[i++];
        group_length = msg[i++];
#if defined(LOG_PACKETS)
        printf("Format 0x%X, group ID 0x%X, group length %d\n", format_identifier, group_identifier, group_length);
#endif
        if (format_identifier != IEC62056_46_UA_FORMAT_IDENTIFIER  ||  group_identifier != IEC62056_46_UA_GROUP_IDENTIFIER)
        {
            /* We only understand one value for this pair of fields. */
            return;
        }
        group_length += i;
        /* Set defaults */
        link->max_tx_info = MAX_TX_LEN;
        link->max_rx_info = MAX_RX_LEN;
        link->tx_window = TX_WINDOW;
        link->rx_window = RX_WINDOW;
        for (  ;  i < group_length;  )
        {
            type = msg[i++];
            j = msg[i++];
            value = 0;
            for (k = 0;  k < j;  k++)
                value = (value << 8) | msg[i + k];
            i += j;
            switch (type)
            {
            case IEC62056_46_UA_TX_INFO:
                link->max_tx_info = value;
                break;
            case IEC62056_46_UA_RX_INFO:
                link->max_rx_info = value;
                break;
            case IEC62056_46_UA_TX_WINDOW:
                link->tx_window = value;
                break;
            case IEC62056_46_UA_RX_WINDOW:
                link->rx_window = value;
                break;
            }
        }
    }
#if defined(LOG_PACKETS)
    printf("Max info field - transmit: %d\n", link->max_tx_info);
    printf("Max info field - receive: %d\n", link->max_rx_info);
    printf("Window size - transmit: %d\n", link->tx_window);
    printf("Window size - receive: %d\n", link->rx_window);
#endif
//    if (link->timeout_id >= 0)
//    {
//        cancel_timeout(link, link->timeout_id);
//        link->timeout_id = -1;
//        /* Start another background timeout */
//        link->timeout_id = schedule_timeout(link, TO_WAIT_RESP);
//    }
    link->next_nr = 0;
    link->next_ns = 0;
    link->ns = 0;
    link->disconnected = FALSE;
    /* IEC62056-46/5.2.1.4, 5.3.3 and 6.2.1.5 */
    /* The MA_CONNECT_confirm condition passes transparently through as DL_CONNECT_confirm */
    dl_connect_confirm(link, DL_CONNECT_CONFIRM_OK);
    /*** Now ready to go ***/
    //send_aarq(link);
}
/*- End of function --------------------------------------------------------*/

void process_snrm_frame(iec62056_46_link_t *link, const uint8_t msg[], int len)
{
    int i;
    int j;
    int k;
    int format_identifier;
    int group_identifier;
    int group_length;
    int type;
    uint32_t value;
    
#if defined(LOG_PACKETS)
    printf("Body: ");
    for (i = 0;  i < len;  i++)
        printf("%02X ", msg[i]);
    printf("\n");
#endif
    
    for (i = 0;  i < len;  i++)
    {
        format_identifier = msg[i++];
        group_identifier = msg[i++];
        group_length = msg[i++];
#if defined(LOG_PACKETS)
        printf("Format 0x%X, group ID 0x%X, group length %d\n", format_identifier, group_identifier, group_length);
#endif
        if (format_identifier != 0x81  ||  group_identifier != 0x80)
        {
           /* We only understand one value for this pair of fields. */
          __asm__ __volatile__ (" NOP");
          return;
        }
        group_length += i;
        /* Set defaults */
        link->max_tx_info = MAX_TX_LEN;
        link->max_rx_info = MAX_RX_LEN;
        link->tx_window = TX_WINDOW;
        link->rx_window = RX_WINDOW;
        for (  ;  i < group_length;  )
        {
            type = msg[i++];
            j = msg[i++];
            value = 0;
            for (k = 0;  k < j;  k++)
                value = (value << 8)  | msg[i + k];
            i += j;
            switch (type)
            {
            case IEC62056_46_UA_TX_INFO:
                link->max_tx_info = value;
                break;
            case IEC62056_46_UA_RX_INFO:
                link->max_rx_info = value;
                break;
            case IEC62056_46_UA_TX_WINDOW:
                link->tx_window = value;
                break;
            case IEC62056_46_UA_RX_WINDOW:
                link->rx_window = value;
                break;
            }
        }
    }
#if defined(LOG_PACKETS)
    printf("Max info field - transmit: %d\n", link->max_tx_info);
    printf("Max info field - receive: %d\n", link->max_rx_info);
    printf("Window size - transmit: %d\n", link->tx_window);
    printf("Window size - receive: %d\n", link->rx_window);
#endif
    /* IEC62056-46/6.4.4.4.3.1 */
    link->next_nr = 0;
    link->next_ns = 0;
    link->ns = 0;
    link->disconnected = FALSE;
    /* IEC62056-46/5.2.1.2/3, 5.3.3 and 6.2.1.3/4 */
    /* The MA_CONNECT_indication and MA_CONNECT_response conditions pass transparently
       through as DL_CONNECT_indication and DL_CONNECT_response */
    if (dl_connect_indication(link) == DL_CONNECT_RESPONSE_OK)
        send_ua(link);
    else
        send_dm(link);
}
/*- End of function --------------------------------------------------------*/
void configure_uart_port(int port, int mode);
extern uint8_t Transmit;
extern volatile uint8_t IEC_Tx_Done;
extern int local_comm_exchange_mode_flag;
void process_disc_frame(iec62056_46_link_t *link, const uint8_t msg[], int len)
{
    /* IEC62056-46/6.4.4.4.3.3 */
    dl_disconnect_indication(link, 0);
    if (link->disconnected)
        send_dm(link);
    else
        send_ua(link);
    link->disconnected = TRUE;
    link->configured = FALSE;
    link->far_msap = 0;
    if(link->port==1 && local_comm_exchange_mode_flag==0)
    {
      while(Transmit&&!IEC_Tx_Done);
      configure_uart_port(link->port,0);
    }
}
/*- End of function --------------------------------------------------------*/

void process_dm_frame(iec62056_46_link_t *link, const uint8_t msg[], int len)
{
    /* IEC62056-46/6.4.4.4.3.1 */
    link->next_nr = 0;
    link->next_ns = 0;
    link->ns = 0;
    /* TODO: stop timer */
    link->disconnected = TRUE;
    link->configured = FALSE;
    /*** Now not ready to go ***/
    /* IEC62056-46/5.2.1.4, 5.3.3 and 6.2.1.5 */
    /* The MA_CONNECT_confirm condition passes transparently through as DL_CONNECT_confirm */
    dl_connect_confirm(link, DL_CONNECT_CONFIRM_NOK_REMOTE);
}
/*- End of function --------------------------------------------------------*/

void process_frmr_frame(iec62056_46_link_t *link, const uint8_t msg[], int len)
{
    /* See IEC13239/5.5.3.4.1 */
}
/*- End of function --------------------------------------------------------*/

void process_ui_frame(iec62056_46_link_t *link, const uint8_t msg[], int len)
{
}
/*- End of function --------------------------------------------------------*/
