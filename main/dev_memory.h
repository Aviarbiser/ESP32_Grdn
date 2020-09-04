/*
 * dev_memory.h
 *
 *  Created on: 24 αιεμι 2019
 *      Author: AviahiArbiser
 */

#ifndef MAIN_DEV_MEMORY_H_
#define MAIN_DEV_MEMORY_H_

typedef struct
{
	uint8_t Angle;
	uint8_t Position;
}Motor_Position_STD;

typedef struct
{
	uint16_t ID1;
	uint16_t ID2;
	uint16_t ID3;
	uint16_t ID4;
}DEVICE_IDs_STD;


extern DEVICE_IDs_STD DeviceIDs;

void memory_init();
void SaveHoursToMemory();
void SaveMotorParamToMemory();
void save_tot_in_nvs();
void Save_WIFI_parmToMemory();
void SaveDeviceIDsToMemory();

#endif /* MAIN_DEV_MEMORY_H_ */
