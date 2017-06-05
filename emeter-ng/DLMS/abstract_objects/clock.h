/*
 * clock.h
 *
 *  Created on: May 12, 2017
 *      Author: abi
 */

#ifndef DLMS_ABSTRACT_OBJECTS_CLOCK_H_
#define DLMS_ABSTRACT_OBJECTS_CLOCK_H_

extern int16_t clock_time_zone;             // time zone
extern uint8_t clock_status;                // status
extern uint8_t daylights_savings_begin[];   // daylights savings begin
extern uint8_t daylights_savings_end[] ;    // daylights savings end
extern int8_t clock_dst_deviation ;         // daylights savings deviation
extern int8_t clock_dst_enabled ;           // daylights savings enabled
extern uint8_t clock_base ;                 // internal crystal
extern uint8_t max_allowed_time_shift;      // Maximum allowed time shift without registration of a time shift event
extern uint16_t clock_time_shift_invalid_lim; // Maximum allowed time shift without setting IV measurement status to 1
extern uint8_t last_clock_synchronized_date_time[] ; // Date (corrected) of last clock synchronisation / setting
void get_date_time_string(void *data, int direction);



#endif /* DLMS_ABSTRACT_OBJECTS_CLOCK_H_ */
