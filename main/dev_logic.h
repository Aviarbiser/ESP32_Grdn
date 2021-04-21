/*
 * dev_logic.h
 *
 *  Created on: 20 במאי 2019
 *      Author: AviahiArbiser
 */

#ifndef MAIN_DEV_LOGIC_H_
#define MAIN_DEV_LOGIC_H_


#include "dev_io.h"
#include <stdio.h>
#include <string.h>
#include "esp_timer.h"
#include <math.h>






typedef struct
{
	int16_t StartHour1 ;//1-1440 minute in day
	int16_t StartHour2 ;//1-1440 minute in day
	int16_t StartHour3 ;//1-1440 minute in day
	int16_t Duration1 ; //0-100 %
	int16_t Duration2 ; //0-100 %
	int16_t Duration3 ; //0-100 %
}__attribute__((packed))HOUR_TIMES_STD;

typedef struct
{
	HOUR_TIMES_STD hour_parm[7];
	uint8_t timeselect;
	uint8_t status;

}__attribute__((packed))VALVE_STD;


extern VALVE_STD valve_parm[4];



extern double sunZenith;
extern double sunAzimuth;
extern uint8_t DeviceErrCount;
extern int RelayOUT_Manual[4] ;


extern int8_t position_SP ;


void logic_loop();
void logic_init();
uint64_t motion_timer_get_time();


#endif /* MAIN_DEV_LOGIC_H_ */
