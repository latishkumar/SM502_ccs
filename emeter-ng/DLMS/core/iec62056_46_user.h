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
//  File: iec62056_46_user.h
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: iec62056_46_user.h,v 1.3 2010/05/13 07:47:36 a0393626 Exp $
//
//
//--------------------------------------------------------------------------
//

#if !defined(_IEC62056_46_USER_H_)
#define _IEC62056_46_USER_H_

/* IEC62056-46/5.2.1.3 */
typedef enum
{
    DL_CONNECT_RESPONSE_OK = 0,
    DL_CONNECT_RESPONSE_NOK = 1,
    DL_CONNECT_RESPONSE_NO_RESPONSE = 3
} dl_connect_response_result_t;

/* IEC62056-46/5.2.1.4 */
typedef enum
{
    DL_CONNECT_CONFIRM_OK = 0,
    DL_CONNECT_CONFIRM_NOK_REMOTE = 1,
    DL_CONNECT_CONFIRM_NOK_LOCAL = 2,
    DL_CONNECT_CONFIRM_NO_RESPONSE = 3
} dl_connect_confirm_result_t;

/* IEC62056-46/5.2.2.3 */
typedef enum
{
    DL_DISCONNECT_INDICATION_REMOTE = 0,
    DL_DISCONNECT_INDICATION_LOCAL_DL = 1,
    DL_DISCONNECT_INDICATION_LOCAL_PHY = 2
} dl_disconnect_indication_t;

/* IEC62056-46/5.2.2.4 */
typedef enum
{
    DL_DISCONNECT_RESPONSE_OK = 0,
    DL_DISCONNECT_RESPONSE_NOK = 1,
    DL_DISCONNECT_RESPONSE_NO_RESPONSE = 3
} dl_disconnect_response_result_t;

/* IEC62056-46/5.2.2.5 */
typedef enum
{
    DL_DISCONNECT_CONFIRM_OK = 0,
    DL_DISCONNECT_CONFIRM_NOK = 1,
    DL_DISCONNECT_CONFIRM_NO_RESPONSE = 3
} dl_disconnect_confirm_result_t;



#if defined(__cplusplus)
extern "C"
{
#endif

/* IEC62056-46/5.2.1.1 */
int dl_connect_request(iec62056_46_link_t *link);

/* IEC62056-46/5.2.1.2 & 3 */
/* This is a routine to be provided by the application */
/* DL_CONNECT_response is the return value from this function. */ 
int dl_connect_indication(iec62056_46_link_t *link);

/* IEC62056-46/5.2.1.4 */
/* This is a routine to be provided by the application */
void dl_connect_confirm(iec62056_46_link_t *link, int response);

/* IEC62056-46/5.2.2.2 */
int dl_disconnect_request(iec62056_46_link_t *link);

/* IEC62056-46/5.2.2.3 & 4 */
/* This is a routine to be provided by the application */
/* DL_DISCONNECT_response is the return value from this function. */ 
int dl_disconnect_indication(iec62056_46_link_t *link, int reason);

/* IEC62056-46/5.2.2.5 */
/* This is a routine to be provided by the application */
void dl_disconnect_confirm(iec62056_46_link_t *link, int response);

/* IEC62056-46/5.2.3.2 */
int dl_data_request(iec62056_46_link_t *link,
                    uint8_t destination_lsap,       /* Always 0xE6 with COSEM */
                    uint8_t source_lsap,            /* Always 0xE6 for a request with COSEM */
                    uint8_t llc_quality,            /* Always 0 with COSEM */
                    uint32_t destination_msap,
                    uint32_t source_msap,
                    uint8_t frame_type,             /* I_FIRST_FRAGMENT, I_FRAGMENT, or I_LAST_FRAGMENT */
                    const uint8_t *msg,
                    int len);

/* IEC62056-46/5.2.3.3 */
/* This is a routine to be provided by the application */
int dl_data_indication(iec62056_46_link_t *link,
                       uint8_t destination_lsap,    /* Always 0xE6 with COSEM */
                       uint8_t source_lsap,         /* Always 0xE6 for a request, or 0xE7 for a response with COSEM */
                       int llc_quality,             /* Always 0 with COSEM */
                       uint32_t destination_msap,
                       uint32_t source_msap,
                       uint8_t frame_type,
                       const uint8_t *msg,
                       int len);



/* IEC62056-46/5.2.3.4 */
int dl_data_confirm(iec62056_46_link_t *link,
                    uint8_t destination_lsap,       /* Always 0xE6 with COSEM */
                    uint8_t source_lsap,            /* Always 0xE7 for a response with COSEM */
                    uint8_t llc_quality,            /* Always 0 with COSEM */
                    uint32_t destination_msap,
                    uint32_t source_msap,
                    uint8_t frame_type,
                    const uint8_t *msg,
                    int len);

#if defined(__cplusplus)
}
#endif

#endif

/*- End of file ------------------------------------------------------------*/
