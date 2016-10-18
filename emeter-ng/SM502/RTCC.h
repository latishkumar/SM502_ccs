/*
 * RTCC.h
 *
 *  Created on: May 18, 2013
 *      Author: Elec_Lab
 */

#ifndef RTCC_H_
#define RTCC_H_

typedef struct
{
	int Year;
	int Month;
	int Day;
	int Hour;
	int Minute;
	int Second;
}RTCtime;

void RTCCinit();


#endif /* RTCC_H_ */
