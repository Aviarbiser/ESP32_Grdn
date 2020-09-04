/*
 * dev_wifi.h
 *
 *  Created on: 1 срхїз 2019
 *      Author: AviahiArbiser
 */

#ifndef MAIN_DEV_WIFI_H_
#define MAIN_DEV_WIFI_H_

typedef struct
{
	char ssid[32];
	char password[64];
}WIFI_PARM_STD;

typedef struct
{
	char IP[10];//ip4addr_ntoa() to char
	int8_t rssi;
}WIFI_STATUS_STD;



extern WIFI_PARM_STD wifi_station_parm;
extern WIFI_STATUS_STD wifi_status;

void wifi_init_sta(void);
void wifi_config_sta();


#endif /* MAIN_DEV_WIFI_H_ */
