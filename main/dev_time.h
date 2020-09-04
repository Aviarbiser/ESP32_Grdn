/*
 * dev_time.h
 *
 *  Created on: 31 במאי 2019
 *      Author: AviahiArbiser
 */

#ifndef MAIN_DEV_TIME_H_
#define MAIN_DEV_TIME_H_

//#define __BSD_VISIBLE  1
#include <sys/cdefs.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include <stdlib.h>
#include "spa.h"  //include the SPA header file
#include "dev_logic.h"

extern uint16_t mininday;

void update_now_time(uint32_t Newval);
long getLocalTime(char * mytimestring);
void getSPAdata(double * Zenith ,double * Azimuth,uint16_t * Sunrise,uint16_t * Sunset);
void update_time(int yy,int mm,int dd,int hh,int MM,int ss);
void init_time();
void initialize_sntp(void);


#endif /* MAIN_DEV_TIME_H_ */
