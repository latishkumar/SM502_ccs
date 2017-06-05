/*
 * scaler_units.h
 *
 *  Created on: May 12, 2017
 *      Author: abi
 */

#ifndef DLMS_ABSTRACT_OBJECTS_SCALER_UNITS_H_
#define DLMS_ABSTRACT_OBJECTS_SCALER_UNITS_H_
#include "cosem.h"
#include "obis.h"
/*
 * Scalar Units
 */

const uint8_t scalar_unit_Time_Shift_EvtLim[]=
{
   5,
      2,
         TAG_INT8,INJECT8(0),
         TAG_ENUM, OBIS_UNIT_TIME_SECOND
};

const uint8_t scalar_unit_ThresholdVoltageSag_Swell[] =
{
    5,
        2,
            TAG_INT8, INJECT8(-2),
            TAG_ENUM, OBIS_UNIT_PERCENT
};

const uint8_t scalar_unit_KW[] =
{
    5,
        2,
            TAG_INT8, INJECT8(-2),
            TAG_ENUM, OBIS_UNIT_ACTIVE_POWER_WATT
};

const uint8_t scalar_unit_dem_colseToContractPower[]=
{
   5,
      2,
         TAG_INT8,INJECT8(-2),
         TAG_ENUM, OBIS_UNIT_PERCENT
};

const uint8_t scalar_unit_Time[]=
{
   5,
      2,
         TAG_INT8,INJECT8(0),
         TAG_ENUM, OBIS_UNIT_TIME_SECOND
};

const uint8_t scalar_unit_total_active_energy_import[] =
{
    5,
        2,
            TAG_INT8, INJECT8(0),
            TAG_ENUM, OBIS_UNIT_ACTIVE_ENERGY_WATT_HOUR
};

const uint8_t scalar_unit_reactive_energy[] =
{
    5,
        2,
            TAG_INT8, INJECT8(0),
            TAG_ENUM, OBIS_UNIT_REACTIVE_ENERGY_VAR_HOUR
};

const uint8_t scalar_unit_V[] =
{
    5,
        2,
            TAG_INT8, INJECT8(-2),
            TAG_ENUM, OBIS_UNIT_VOLTAGE_VOLT
};

const uint8_t scalar_unit_I[] =
{
    5,
        2,
            TAG_INT8, INJECT8(-3),
            TAG_ENUM, OBIS_UNIT_CURRENT_AMPERE
};

const uint8_t scalar_unit_active_power[] =
{
    5,
        2,
            TAG_INT8, INJECT8(-2),
            TAG_ENUM, OBIS_UNIT_ACTIVE_POWER_WATT
};

const uint8_t scalar_unit_PF[] =
{
    5,
        2,
            TAG_INT8, INJECT8(-4),
            TAG_ENUM, OBIS_UNIT_UNITLESS_COUNT
};

const uint8_t scalar_unit_referance_voltage[] =
{
    5,
        2,
            TAG_INT8, INJECT8(0),
            TAG_ENUM, OBIS_UNIT_VOLTAGE_VOLT
};

const uint8_t scalar_unit_Frequency[] =
{
    5,
        2,
            TAG_INT8, INJECT8(-2),
            TAG_ENUM, OBIS_UNIT_FREQUENCY_HERTZ
};
const uint8_t scalar_unit_KVA[] =
{
    5,
        2,
            TAG_INT8, INJECT8(0),
            TAG_ENUM, OBIS_UNIT_APPARENT_POWER_VA
};

const uint8_t scalar_unit_power_current_voltage_calib[] =
{
    5,
        2,
            TAG_INT8, INJECT8(0),
            TAG_ENUM, OBIS_UNIT_UNITLESS_COUNT
};
const uint8_t scalar_unit_KVAr[] = //not used
{
    5,
        2,
            TAG_INT8, 0,
            TAG_ENUM, OBIS_UNIT_REACTIVE_POWER_VAR
};
#endif /* DLMS_ABSTRACT_OBJECTS_SCALER_UNITS_H_ */
