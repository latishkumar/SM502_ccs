
#ifndef LPBU_H
#define LPBU_H
#include <stdint.h>

#define __infoAmem__ _Pragma("LOCATION(\"INFOB\")")
// 0x0080, 0x1900)")


/**
* Low power back up flash structure 
*/
struct INFOA_BACK_UP
{
      //Energy Registers  
      int32_t active_power_counter;
      int32_t active_energy_counter;
      
      uint32_t active_energy_counter_QI;
      uint32_t active_energy_counter_QII;
      uint32_t active_energy_counter_QIII;
      uint32_t active_energy_counter_QIV;
      
      
      uint32_t active_energy_import_counter;
      uint32_t active_energy_export_counter;
      
      
      uint32_t consumed_active_energy_QI;
      uint32_t consumed_active_energy_QII;
      uint32_t consumed_active_energy_QIII;
      uint32_t consumed_active_energy_QIV;
        
      
      int32_t reactive_power_counter;      
      uint32_t reactive_energy_counter_QI;
      uint32_t reactive_energy_counter_QII;
      uint32_t reactive_energy_counter_QIII;
      uint32_t reactive_energy_counter_QIV;
           
      uint32_t consumed_reactive_energy_QI;
      uint32_t consumed_reactive_energy_QII;
      uint32_t consumed_reactive_energy_QIII;
      uint32_t consumed_reactive_energy_QIV;
          
      
      //balance 
      uint32_t CurrentBalance;
      //Last Energy
      uint32_t TempLastEnergyValue;
      
      //Time Stump
      uint32_t RTCLow; // this is the RTC_Back up time, also the Power out Timestump
      uint32_t RTCHigh;
         
      uint32_t ConsumptionSinceLastBilling;
      uint32_t valid_backup;
};


struct info_mem_a_s
{
    union
    {
        struct INFOA_BACK_UP s;
        int8_t x[128];
    } seg_a;
};

//__infoAmem__ extern const struct info_mem_a_s backup;
__infoAmem__ extern const struct info_mem_a_s backup;
#endif
