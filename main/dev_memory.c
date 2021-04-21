/*
 * dev_memory.c
 *
 *  Created on: 24 αιεμι 2019
 *      Author: AviahiArbiser
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "dev_logic.h"
#include "dev_wifi.h"
#include "protocol.h"
#include "dev_memory.h"

DEVICE_IDs_STD DeviceIDs;
uint8_t tot_value=0;
uint8_t tot_index = 0 ;
uint32_t tot_write_count = 0;

void memory_init()
{

	printf(" memory_init initialize NVS\n");

	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		// NVS partition was truncated and needs to be erased
		// Retry nvs_flash_init
		printf(" nvs_flash_init ERROR: Unable to initialize NVS\n");
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
	}

	ESP_ERROR_CHECK( err );

	if (err!= ESP_OK)
	{
		printf("FATAL ERROR: Unable to initialize NVS\n");
		return;
	}

	// open the partition in RW mode
	// NVS handler
	nvs_handle my_handle;
	err = nvs_open("nvs", NVS_READWRITE, &my_handle);
	if (err != ESP_OK)
	{
		printf("open nvs ERROR: Unable to open NVS err code %d\n",err);
		return;
	}


	// Read run time blob
	size_t required_size;
	uint8_t* hourArray = malloc(sizeof(VALVE_STD)*4);
	required_size = sizeof(VALVE_STD)*4;
	// obtain required memory space to store blob being read from NVS
	err = nvs_get_blob(my_handle, "hour_parm", hourArray, &required_size);
	if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND)
	{
		free(hourArray);
		printf("nvs_get ERROR: HOUR_TIMES_STD err code %d\n",err);
		//return;
	}
	else
	{
		if(required_size > 0)
		{
			memcpy(&valve_parm,hourArray,sizeof(VALVE_STD)*4);
			free(hourArray);
		}
	}


	uint8_t* wifiArray = malloc(sizeof(WIFI_PARM_STD)+1);
	required_size = sizeof(WIFI_PARM_STD);
	// obtain required memory space to store blob being read from NVS
	err = nvs_get_blob(my_handle, "WIFI_parm", wifiArray, &required_size);
	if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND)
	{
		free(wifiArray);
		printf("nvs_get ERROR: WIFI_parm err code %d\n",err);
		//return;
	}
	else
	{
		if(required_size > 0)
		{
			memcpy(&wifi_station_parm,wifiArray,sizeof(WIFI_PARM_STD));

			printf("nvs WIFI_parm read ssid %s \n",wifi_station_parm.ssid);
			free(wifiArray);
		}
	}

	//read IDs from flash
	uint8_t* IDsArray = malloc(sizeof(DEVICE_ID_STD)+1);
	required_size = sizeof(DEVICE_ID_STD);
	// obtain required memory space to store blob being read from NVS
	err = nvs_get_blob(my_handle, "IDs_parm", IDsArray, &required_size);
	if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND)
	{
		free(IDsArray);
		printf("nvs_get ERROR: IDsArray err code %d\n",err);
		//return;
	}
	else
	{
		if(required_size > 0)
		{
			memcpy(&DeviceIDs,IDsArray,sizeof(DEVICE_ID_STD));

			printf("nvs IDsArray read ID1 %d \n",DeviceIDs.ID1);
			free(IDsArray);
		}
	}




	nvs_close(my_handle);
	return;

}




void SaveDeviceIDsToMemory()
{


	nvs_handle my_handle;
	esp_err_t err = nvs_open("nvs", NVS_READWRITE, &my_handle);
	if (err != ESP_OK)
	{
		printf("open nvs ERROR: Unable to open NVS err code %d\n",err);
		return;
	}

	uint8_t* IDsArray = malloc(sizeof(DEVICE_ID_STD));

	memcpy(IDsArray ,&DeviceIDs,sizeof(DEVICE_ID_STD));

	err = nvs_set_blob(my_handle, "IDs_parm", IDsArray, sizeof(DEVICE_ID_STD));

	free(IDsArray);

	if (err != ESP_OK)
	{
		printf("SaveDeviceIDsToMemory ERROR: set DEVICE_ID_STD err code %d\n",err);
	}

	// Commit
	err = nvs_commit(my_handle);
	if (err != ESP_OK)
	{
		printf("SaveDeviceIDsToMemory ERROR: commit DEVICE_ID_STD err code %d\n",err);
	}

	// Close
	nvs_close(my_handle);

}





void SaveHoursToMemory()
{
	nvs_handle my_handle;
	esp_err_t err = nvs_open("nvs", NVS_READWRITE, &my_handle);
	if (err != ESP_OK)
	{
		printf("open nvs ERROR: Unable to open NVS err code %d\n",err);
		return;
	}

	uint8_t* hourArray = malloc(sizeof(VALVE_STD)*4);

	memcpy(hourArray ,&valve_parm,sizeof(VALVE_STD)*4);

	err = nvs_set_blob(my_handle, "hour_parm", hourArray, sizeof(VALVE_STD)*4);

	free(hourArray);

	if (err != ESP_OK)
	{
		printf("SaveHoursToMemory ERROR: set HOUR_TIMES_STD err code %d\n",err);
	}

	// Commit
	err = nvs_commit(my_handle);
	if (err != ESP_OK)
	{
		printf("SaveHoursToMemory ERROR: commit HOUR_TIMES_STD err code %d\n",err);
	}

	// Close
	nvs_close(my_handle);

}


void Save_WIFI_parmToMemory()
{
	nvs_handle my_handle;
	esp_err_t err = nvs_open("nvs", NVS_READWRITE, &my_handle);
	if (err != ESP_OK)
	{
		printf("open nvs ERROR: Unable to open NVS err code %d\n",err);
		return;
	}

	//uint8_t headerData[sizeof(CONNECTION_STRACT_STD)];


	uint8_t* memArray = malloc(sizeof(WIFI_PARM_STD)+1);
	//WIFI_PARM_STD * Header = (WIFI_PARM_STD *)memArray;

	printf("Save_WIFI_parmToMemory memcpy\n");
	memcpy(memArray ,&wifi_station_parm,sizeof(WIFI_PARM_STD));

	size_t required_size = sizeof(WIFI_PARM_STD);

	printf("Save_WIFI_parmToMemory nvs_set_blob %d \n",required_size);

	err = nvs_set_blob(my_handle, "WIFI_parm", memArray, required_size);

	printf("Save_WIFI_parmToMemory nvs_set_blob done\n");


	if (err != ESP_OK)
	{
		printf("Save_WIFI_parmToMemory ERROR: set WIFI_PARM_STD err code %d\n",err);
	}

	// Commit
	err = nvs_commit(my_handle);
	if (err != ESP_OK)
	{
		printf("Save_WIFI_parmToMemory ERROR: commit WIFI_PARM_STD err code %d\n",err);
	}

	printf("Save_WIFI_parmToMemory nvs_commit done\n");

	free(memArray);

	printf("Save_WIFI_parmToMemory free(memArray) done\n");
	// Close
	nvs_close(my_handle);

}

