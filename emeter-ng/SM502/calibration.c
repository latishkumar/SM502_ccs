

#include "Logger.h"
/*
   D.
   TODO. add MAC address and serial number at the end of this structure
*/
/* The main per-phase non-volatile parameter structure */
#pragma DATA_SECTION(nv_parms, ".infoD")
const struct info_mem_s nv_parms =
{
    {
    {
        0xFFFF,
            {
                {
                    // (IRMS_SUPPORT)
                    {DEFAULT_I_RMS_SCALE_FACTOR_A},
                    0,

                    // GAIN_STAGES == 1
                    DEFAULT_P_SCALE_FACTOR_A_LOW,

                    DEFAULT_I_DC_ESTIMATE << 16,
                    //(PHASE_CORRECTION_SUPPORT)
                    // GAIN_STAGES == 1
                    DEFAULT_BASE_PHASE_A_CORRECTION_LOW,

                },
                // (VRMS_SUPPORT)
                DEFAULT_V_RMS_SCALE_FACTOR_A,

                DEFAULT_V_DC_ESTIMATE << 16,
            },
        //(NEUTRAL_MONITOR_SUPPORT)
        {
            //(IRMS_SUPPORT)
            DEFAULT_I_RMS_SCALE_FACTOR_NEUTRAL,
            0,

            DEFAULT_P_SCALE_FACTOR_NEUTRAL,
            DEFAULT_I_DC_ESTIMATE << 16,
            //(PHASE_CORRECTION_SUPPORT)
            DEFAULT_NEUTRAL_BASE_PHASE_CORRECTION,
        },

#if defined(TEMPERATURE_SUPPORT)
        25,
        DEFAULT_TEMPERATURE_OFFSET,
        DEFAULT_TEMPERATURE_SCALING,
#endif
        //(CORRECTED_RTC_SUPPORT)
        0,
        {
            0,
            0,
            0,
            0,
            0,
            0
        },
        0
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
*                         3:Shunt power
*                         4:Shunt current
*/

void updateCalibrationFactor(int16_t ErrorPercent,uint8_t type)
{
    if(type > 4)
        return;
    
    uint8_t sign = ErrorPercent<0?1:0;
    if(sign)
      ErrorPercent = -ErrorPercent;

    uint32_t z  = 0;
    int a = 0;
    int *address;

    switch(type)
    {
    case 0: //Live power scaler
        z = (uint32_t)nv_parms.seg_a.s.chan1.current.P_scale_factor[0] * ErrorPercent;
        z /= 10000;

        if(sign)
            a= nv_parms.seg_a.s.chan1.current.P_scale_factor[0] + (uint16_t)z;
        else
        {
            a= nv_parms.seg_a.s.chan1.current.P_scale_factor[0] - (uint16_t)z;
            if(a <= 0)
            {
                return;
            }
        }
        address = (int *)(&nv_parms.seg_a.s.chan1.current.P_scale_factor[0]);
        break;
    
    case 1: //CT current scaler
        z = (uint32_t)nv_parms.seg_a.s.chan1.current.I_rms_scale_factor * ErrorPercent;
        z /= 10000;
    
        if(sign)
            a= nv_parms.seg_a.s.chan1.current.I_rms_scale_factor + (uint16_t)z;
        else
        {
            a= nv_parms.seg_a.s.chan1.current.I_rms_scale_factor - (uint16_t)z;
            if(a <= 0)
            {
                return;
            }
        }
        address = (int *)(&nv_parms.seg_a.s.chan1.current.I_rms_scale_factor);
        break;

    case 2: //voltage scaler
        z = (uint32_t)nv_parms.seg_a.s.chan1.V_rms_scale_factor * ErrorPercent;
        z /= 10000;

        if(sign)
            a= nv_parms.seg_a.s.chan1.V_rms_scale_factor + (uint16_t)z;
        else
        {
            a= nv_parms.seg_a.s.chan1.V_rms_scale_factor - (uint16_t)z;
            if(a <= 0)
            {
                return;
            }
        }
        address = (int *)(&nv_parms.seg_a.s.chan1.V_rms_scale_factor);
        break;

    case 3: //Neutral power scaler
        z = (uint32_t)nv_parms.seg_a.s.neutral.P_scale_factor[0] * ErrorPercent;
        z /= 10000;

        if(sign)
            a= nv_parms.seg_a.s.neutral.P_scale_factor[0] + (uint16_t)z;
        else
        {
            a= nv_parms.seg_a.s.neutral.P_scale_factor[0] - (uint16_t)z;
            if(a <= 0)
            {
                return;
            }
        }
        address = (int *)(&nv_parms.seg_a.s.neutral.P_scale_factor[0]);
        break;

    case 4: //Neutral current scaler
        z = (uint32_t)nv_parms.seg_a.s.neutral.I_rms_scale_factor * ErrorPercent;
        z /= 10000;

        if(sign)
            a= nv_parms.seg_a.s.neutral.I_rms_scale_factor + (uint16_t)z;
        else
        {
            a= nv_parms.seg_a.s.neutral.I_rms_scale_factor - (uint16_t)z;
            if(a <= 0)
            {
                return;
            }
        }
        address = (int *)(&nv_parms.seg_a.s.neutral.I_rms_scale_factor);
        break;

       default:
         return;
    }
     flash_replace16(address,a);    
}




