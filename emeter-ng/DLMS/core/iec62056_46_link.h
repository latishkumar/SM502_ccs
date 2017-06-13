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
//  File: iec62056_46_link.h
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: iec62056_46_link.h,v 1.6 2011/07/11 10:44:58 a0393626 Exp $
//
//
//--------------------------------------------------------------------------
//

#if !defined(_IEC62056_46_LINK_H_)
#define _IEC62056_46_LINK_H_

/* IEC62056-46/6.4.4.3.3 Inter-octet time out */
//#define TIO                                     25  /* 25ms, but may vary with the medium */

/* IEC62056-46/6.4.4.3.4 Idle HDLC channel state time out */
//#define TIDLE                                   25  /* 25ms, but may vary with the medium */

/* IEC62056-46/6.4.4.10.1 Frame response timeout for all frame types - SNRM, DISC, I */
/* TO_WAIT_RESP > RespTime + 2*MaxTxTime */
//#define TO_WAIT_RESP                            5000

/* IEC62056-46/6.4.4.10.2 */
//#define MAX_NB_OF_RETRIES                       42

/* IEC62056-46/6.4.4.10.3 */
#define TIMEOUT_2_INACTIVITY                    20000

/* IEC62056-46/6.4.4.10.4 */
//#define TIMEOUT_3_INTRA_FRAME                   25
#define TIMEOUT_3_INTRA_FRAME                   200

#define MAX_RX_LEN                              128
#define MAX_TX_LEN                              128
#define RX_WINDOW                               1
#define TX_WINDOW                               1

#define IEC62056_46_UA_FORMAT_IDENTIFIER        0x81
#define IEC62056_46_UA_GROUP_IDENTIFIER         0x80

/* The COSEM spec defines the following as the only valid LSAP values */
#define IEC62056_46_LSAP_SOURCE                 0xE6
#define IEC62056_46_LSAP_DEST_REQUEST           0xE6
#define IEC62056_46_LSAP_DEST_RESPONSE          0xE7
#define IEC62056_46_LSAP_DEST_BROADCAST         0xFF

enum
{
    IEC62056_46_UA_TX_INFO = 0x05,
    IEC62056_46_UA_RX_INFO = 0x06,
    IEC62056_46_UA_TX_WINDOW = 0x07,
    IEC62056_46_UA_RX_WINDOW = 0x08
};

#define TLV_INT1_TO_BYTES(t,v) t, 1, (v & 0xFF)
#define TLV_INT2_TO_BYTES(t,v) t, 2, ((v >> 8) & 0xFF), (v & 0xFF)
#define TLV_INT3_TO_BYTES(t,v) t, 3, (((uint32_t) v >> 16) & 0xFF), ((v >> 8) & 0xFF), (v & 0xFF)
#define TLV_INT4_TO_BYTES(t,v) t, 4, (((uint32_t) v >> 24) & 0xFF), (((uint32_t) v >> 16) & 0xFF), ((v >> 8) & 0xFF), (v & 0xFF)

/* State information for a single instance of an IEC62056-46 data link. */
typedef struct
{
    int8_t next_nr;
    int8_t next_ns;
    int8_t ns;
    uint8_t disconnected;
    uint8_t configured;
    uint8_t ready;
    uint8_t association;
    /* The local and far HDLC addresses/MSAPs - MAC sub-layer Service Access Point */
    uint32_t local_msap;
    uint32_t far_msap;
    int max_tx_info;
    int max_rx_info;
    int tx_window;
    int rx_window;
    int port;
    int timeout_id;
} iec62056_46_link_t;

typedef struct
{
    /* The start and end flag octets are in addition to the actual size of the frame */
    int msg_len;
    int msg_ptr;
    int hdr_crc_pos;
    uint8_t msg[CHUNK_SIZE+32];
} async_hdlc_tx_t;

typedef struct
{
    int expected_len;
    int msg_len;
    int8_t msap_len;
    uint16_t crc;
    int8_t state;
    int8_t segmented;
    uint8_t control_field;
    uint32_t dest_msap;
    uint32_t source_msap;
    int8_t dest_msap_len;
    int8_t source_msap_len;
    int good_packets;
    uint8_t msg[128+32];
} async_hdlc_rx_t;



/* IEC62056/46 6.4.5 */
enum
{
    MAC_STATE_WAIT_CONNECTION,
    MAC_STATE_WAIT_DISCONNECTION,
    MAC_STATE_NRM_RECEIVE,
    MAC_STATE_NDM_RECEIVE,
    MAC_STATE_NRM_SEND,
    MAC_STATE_NDM_SEND
};

#if defined(__cplusplus)
extern "C"
{
#endif

void send_aarq(iec62056_46_link_t *link);

void send_aare(iec62056_46_link_t *link, const uint8_t context[], const uint8_t conformance[], int ok);

int ma_data_request(iec62056_46_link_t *link, const uint8_t *msg, int len);

/* This is a routine to be provided by the application */
int ma_data_indication(iec62056_46_link_t *link, const uint8_t *msg, int len);

/* This is a routine to be provided by the application */
void send_frame(iec62056_46_link_t *link, async_hdlc_tx_t *tx);

int schedule_timeout(iec62056_46_link_t *link, int32_t timeout);

void cancel_timeout(iec62056_46_link_t *link, int timeout_id);

void iec62056_46_link_init(iec62056_46_link_t *link, int port, uint32_t local_msap);

void iec62056_46_set_far_msap(iec62056_46_link_t *link, uint32_t far_msap);

void send_snrm(iec62056_46_link_t *link);

void send_disc(iec62056_46_link_t *link);

void send_ua(iec62056_46_link_t *link);

void send_dm(iec62056_46_link_t *link);

void send_frmr(iec62056_46_link_t *link);

void send_ui(iec62056_46_link_t *link);

    
void process_i_body(iec62056_46_link_t *link, const uint8_t msg[], int len);

#include "xframe.h"

void iec62056_46_link_idle_restart(iec62056_46_link_t *link);

void iec62056_46_link_idle_timeout(iec62056_46_link_t *link);

void iec62056_46_rx_timeout(iec62056_46_link_t *link, async_hdlc_rx_t *rx);

void iec62056_46_rx_byte(iec62056_46_link_t *link, async_hdlc_rx_t *rx, uint8_t byte);

void hdlc_async_rx_init(async_hdlc_rx_t *rx);

int hdlc_async_tx_add_header(async_hdlc_tx_t *tx, uint32_t dest_msap, uint32_t source_msap, uint8_t control_field);

int hdlc_async_tx_add_body(async_hdlc_tx_t *tx, const uint8_t buf[], int len);

int hdlc_async_tx_add_hdr_crc(async_hdlc_tx_t *tx);

int hdlc_async_tx_end(async_hdlc_tx_t *tx);

void hdlc_async_tx_init(async_hdlc_tx_t *tx);

int hdlc_async_tx_build(async_hdlc_tx_t *tx,
                        uint32_t dest_msap,
                        uint32_t source_msap,
                        uint8_t control_field,
                        const uint8_t buf[],
                        int len);

int hdlc_async_tx_build_iframe(async_hdlc_tx_t *tx,
                               iec62056_46_link_t *link,
                               int poll_final_bit,
                               int request,
                               const uint8_t buf[],
                               int len);

int hdlc_async_tx_build_iframe_chunk_add(async_hdlc_tx_t *tx,
                                         iec62056_46_link_t *link,
                                         const uint8_t buf[],
                                         int len);

void finalise_and_send_iframe(iec62056_46_link_t *link, async_hdlc_tx_t *tx);


void finalise_and_send_frame(iec62056_46_link_t *link, async_hdlc_tx_t *tx);


int iec62056_46_link_timeout(iec62056_46_link_t *link);

int analyze_pdu(const uint8_t msg[], int len);
void release_plc_communication(int port);

#if defined(__cplusplus)
}
#endif

#endif

/*- End of file ------------------------------------------------------------*/
