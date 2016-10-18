

#include "Logger.h"

/*
   D.
   TODO. add MAC address and serial number at the end of this structure

*/
/* The main per-phase non-volatile parameter structure */
__infomem__ const struct info_mem_s nv_parms =
{
    {
    {
        0xFFFF,
#if defined(SELF_TEST_SUPPORT)
        0xFFFF,
        0xFFFF,
#endif
#if !defined(SINGLE_PHASE)
        {
#endif
            {
                {
#if defined(IRMS_SUPPORT)
                    {DEFAULT_I_RMS_SCALE_FACTOR_A},
    #if defined(LIMP_MODE_SUPPORT)
                    {DEFAULT_I_RMS_LIMP_SCALE_FACTOR_A},
    #endif
                    0,
#endif
#if GAIN_STAGES == 1
                    DEFAULT_P_SCALE_FACTOR_A_LOW,
#else
                    {DEFAULT_P_SCALE_FACTOR_A_LOW, DEFAULT_P_SCALE_FACTOR_A_HIGH},
#endif
                    DEFAULT_I_DC_ESTIMATE << 16,
#if defined(PHASE_CORRECTION_SUPPORT)
    #if GAIN_STAGES == 1
                    DEFAULT_BASE_PHASE_A_CORRECTION_LOW,
    #else
                    {DEFAULT_BASE_PHASE_A_CORRECTION_LOW, DEFAULT_BASE_PHASE_A_CORRECTION_HIGH},
    #endif
#endif
                },
#if defined(VRMS_SUPPORT)
                DEFAULT_V_RMS_SCALE_FACTOR_A,
    #if defined(LIMP_MODE_SUPPORT)
                DEFAULT_V_RMS_LIMP_SCALE_FACTOR_A,
    #endif
#endif
                DEFAULT_V_DC_ESTIMATE << 16,
#if defined(LIMP_MODE_SUPPORT)
                DEFAULT_V_LIMP_DC_ESTIMATE << 16,
#endif
            },
#if !defined(SINGLE_PHASE)
            {
                {
    #if defined(IRMS_SUPPORT)
                    {DEFAULT_I_RMS_SCALE_FACTOR_B},
        #if defined(LIMP_MODE_SUPPORT)
                    {DEFAULT_I_RMS_LIMP_SCALE_FACTOR},
        #endif
                    0,
    #endif
    #if GAIN_STAGES == 1
                    DEFAULT_P_SCALE_FACTOR_B_LOW,
    #else
                    {DEFAULT_P_SCALE_FACTOR_B_LOW, DEFAULT_P_SCALE_FACTOR_B_HIGH},
    #endif
                    DEFAULT_I_DC_ESTIMATE << 16,
    #if defined(PHASE_CORRECTION_SUPPORT)
        #if GAIN_STAGES == 1
                    DEFAULT_BASE_PHASE_B_CORRECTION_LOW,
        #else
                    {DEFAULT_BASE_PHASE_B_CORRECTION_LOW, DEFAULT_BASE_PHASE_B_CORRECTION_HIGH},
        #endif
    #endif
                },
    #if defined(VRMS_SUPPORT)
                DEFAULT_V_RMS_SCALE_FACTOR_B,
        #if defined(LIMP_MODE_SUPPORT)
                DEFAULT_V_RMS_LIMP_SCALE_FACTOR,
        #endif
    #endif
                DEFAULT_V_DC_ESTIMATE << 16,
    #if defined(LIMP_MODE_SUPPORT)
                DEFAULT_V_LIMP_DC_ESTIMATE << 16,
    #endif
            },
            {
                {
    #if defined(IRMS_SUPPORT)
                    {DEFAULT_I_RMS_SCALE_FACTOR_C},
        #if defined(LIMP_MODE_SUPPORT)
                    {DEFAULT_I_RMS_LIMP_SCALE_FACTOR},
        #endif
                    0,
    #endif
    #if GAIN_STAGES == 1
                    DEFAULT_P_SCALE_FACTOR_C_LOW,
    #else
                    {DEFAULT_P_SCALE_FACTOR_C_LOW, DEFAULT_P_SCALE_FACTOR_C_HIGH},
    #endif
                    DEFAULT_I_DC_ESTIMATE << 16,
    #if defined(PHASE_CORRECTION_SUPPORT)
        #if GAIN_STAGES == 1
                    DEFAULT_BASE_PHASE_C_CORRECTION_LOW,
        #else
                    {DEFAULT_BASE_PHASE_C_CORRECTION_LOW, DEFAULT_BASE_PHASE_C_CORRECTION_HIGH},
        #endif
    #endif
                },
    #if defined(VRMS_SUPPORT)
                DEFAULT_V_RMS_SCALE_FACTOR_C,
        #if defined(LIMP_MODE_SUPPORT)
                DEFAULT_V_RMS_LIMP_SCALE_FACTOR,
        #endif
    #endif
                DEFAULT_V_DC_ESTIMATE << 16,
    #if defined(LIMP_MODE_SUPPORT)
                DEFAULT_V_LIMP_DC_ESTIMATE << 16,
    #endif
            }
        },
#endif
#if defined(NEUTRAL_MONITOR_SUPPORT)
        {
    #if defined(IRMS_SUPPORT)
            DEFAULT_I_RMS_SCALE_FACTOR_NEUTRAL,
        #if defined(LIMP_MODE_SUPPORT)
            DEFAULT_I_RMS_LIMP_SCALE_FACTOR_NEUTRAL,
        #endif
            0,
    #endif
            DEFAULT_P_SCALE_FACTOR_NEUTRAL,
            DEFAULT_I_DC_ESTIMATE << 16,
    #if defined(PHASE_CORRECTION_SUPPORT)
            DEFAULT_NEUTRAL_BASE_PHASE_CORRECTION,
    #endif
        },
#endif
#if defined(TEMPERATURE_SUPPORT)
        25,
        DEFAULT_TEMPERATURE_OFFSET,
        DEFAULT_TEMPERATURE_SCALING,
#endif
#if defined(CORRECTED_RTC_SUPPORT)
        0,
#endif
        {
            0,
            0,
            0,
            0,
            0,
            0
        },
        0//,
        //""//,
//        "",
//        ""
    }
    }
};

/*
*  
* The method updates the meter calibration factors based on the error percentage passed to it.     
* Error  = (Refereance - Messured)/Messured
* @param ErrorPercent = The amount of Error in percentage scaled by 100
* @param type the type of error to calibrate. possilbe values are 
*                       = 0:Power
*                         1:RMS Current
*                         2:RMS Voltage
*/

void updateCalibrationFactor(int16_t ErrorPercent,uint8_t type)
{
    if(type > 2)
        return;
    
    uint8_t sign = ErrorPercent<0?1:0;
    if(sign)
      ErrorPercent = -ErrorPercent;
    
    
     uint32_t z  = 0; 
     int a = 0;
     int *address;
     
     
     switch(type)
     {
       case 0:
            z = (uint32_t)nv_parms.seg_a.s.chan1.current.P_scale_factor[0] * ErrorPercent;
            z /= 10000;
            
           if(sign)
              a= nv_parms.seg_a.s.chan1.current.P_scale_factor[0] + (uint16_t)z;
           else
              a= nv_parms.seg_a.s.chan1.current.P_scale_factor[0] - (uint16_t)z;
           
           address = (int *)(&nv_parms.seg_a.s.chan1.current.P_scale_factor[0]);  
           
         break;
       case 1:
            z = (uint32_t)nv_parms.seg_a.s.chan1.current.I_rms_scale_factor * ErrorPercent;
            z /= 10000;  
         
         
           if(sign)
              a= nv_parms.seg_a.s.chan1.current.I_rms_scale_factor + (uint16_t)z;
           else
              a= nv_parms.seg_a.s.chan1.current.I_rms_scale_factor - (uint16_t)z;         
          
           address = (int *)(&nv_parms.seg_a.s.chan1.current.I_rms_scale_factor);  
         break;
       case 2:
           
          z = (uint32_t)nv_parms.seg_a.s.chan1.V_rms_scale_factor * ErrorPercent;
          z /= 10000; 
         
          if(sign)
              a= nv_parms.seg_a.s.chan1.V_rms_scale_factor + (uint16_t)z;
           else
              a= nv_parms.seg_a.s.chan1.V_rms_scale_factor - (uint16_t)z;         
           
          address = (int *)(&nv_parms.seg_a.s.chan1.V_rms_scale_factor);       
         break;
       default:
         return;
     }     
     flash_replace16(address,a);    
}




