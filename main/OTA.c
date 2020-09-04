/*
 * OTA.c
 *
 *  Created on:  2019
 *      Author: AviahiArbiser
 */

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_partition.h"
#include "esp_flash_partitions.h"
#include "esp_log.h"
#include "esp_ota_ops.h"

#include "OTA.h"

#include "nvs.h"
#include "nvs_flash.h"
#include "driver/gpio.h"


#define BUFFSIZE 1024
#define HASH_LEN 32 /* SHA-256 digest length */

bool image_header_was_checked;
int binary_file_length;
esp_ota_handle_t update_handle = 0 ;

static const char *TAG = "native_ota_example";
/*an ota data write buffer ready to write to the flash*/
static char ota_write_data[BUFFSIZE + 1] = { 0 };
//extern const uint8_t server_cert_pem_start[] asm("_binary_ca_cert_pem_start");
//extern const uint8_t server_cert_pem_end[] asm("_binary_ca_cert_pem_end");

const esp_partition_t *configured = NULL;
const esp_partition_t *running  = NULL;
const esp_partition_t *update_partition = NULL;

static void print_sha256 (const uint8_t *image_hash, const char *label)
{
    char hash_print[HASH_LEN * 2 + 1];
    hash_print[HASH_LEN * 2] = 0;
    for (int i = 0; i < HASH_LEN; ++i) {
        sprintf(&hash_print[i * 2], "%02x", image_hash[i]);
    }
    ESP_LOGI(TAG, "%s: %s", label, hash_print);
}



esp_err_t starting_ota(void)
{

	 ESP_LOGI(TAG, "Starting OTA example");


	 configured = esp_ota_get_boot_partition();
	 running = esp_ota_get_running_partition();

	if (configured != running) {
		ESP_LOGW(TAG, "Configured OTA boot partition at offset 0x%08x, but running from offset 0x%08x",
				 configured->address, running->address);
		ESP_LOGW(TAG, "(This can happen if either the OTA boot data or preferred boot image become corrupted somehow.)");
	}
	ESP_LOGI(TAG, "Running partition type %d subtype %d (offset 0x%08x)",
			 running->type, running->subtype, running->address);



	update_partition = esp_ota_get_next_update_partition(NULL);
	ESP_LOGI(TAG, "Writing to partition subtype %d at offset 0x%x",
			 update_partition->subtype, update_partition->address);
	assert(update_partition != NULL);

	image_header_was_checked = false;
	binary_file_length = 0;

	return ESP_OK;
}


esp_err_t ota_task(uint8_t * ota_write_data ,int data_read)
{

	 esp_err_t err;

    /*deal with all receive packet*/



	if (data_read > 0)
	{
		if (image_header_was_checked == false) {
			uint8_t app_infoarr[sizeof(esp_app_desc_t)];
			esp_app_desc_t * new_app_info = (esp_app_desc_t*)app_infoarr;
			if (data_read > sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t) + sizeof(esp_app_desc_t)) {
				// check current version with downloading
				memcpy(app_infoarr, ota_write_data+(sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t)), sizeof(esp_app_desc_t));
				ESP_LOGI(TAG, "New firmware version: %s", new_app_info->version);

				esp_app_desc_t running_app_info;
				if (esp_ota_get_partition_description(running, &running_app_info) == ESP_OK) {
					ESP_LOGI(TAG, "Running firmware version: %s", running_app_info.version);
				}

				const esp_partition_t* last_invalid_app = esp_ota_get_last_invalid_partition();
				esp_app_desc_t invalid_app_info;
				if (esp_ota_get_partition_description(last_invalid_app, &invalid_app_info) == ESP_OK) {
					ESP_LOGI(TAG, "Last invalid firmware version: %s", invalid_app_info.version);
				}

				// check current version with last invalid partition
				if (last_invalid_app != NULL) {
					if (memcmp(invalid_app_info.version, new_app_info->version, sizeof(new_app_info->version)) == 0) {
						ESP_LOGW(TAG, "New version is the same as invalid version.");
						ESP_LOGW(TAG, "Previously, there was an attempt to launch the firmware with %s version, but it failed.", invalid_app_info.version);
						ESP_LOGW(TAG, "The firmware has been rolled back to the previous version.");
					//	return 1;
					}
				}

				//	if (memcmp(new_app_info->version, running_app_info.version, sizeof(new_app_info.->version)) == 0) {
						//		ESP_LOGW(TAG, "Current running version is the same as a new. We will not continue the update.");
						//		return 2;
						//	}


				image_header_was_checked = true;
				err = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &update_handle);
				if (err != ESP_OK) {
					ESP_LOGE(TAG, "esp_ota_begin failed (%s)", esp_err_to_name(err));
				   // task_fatal_error();
				   //ToDO: to see if need to do same task like Roll back or ...
				   return 3;
				}
				ESP_LOGI(TAG, "esp_ota_begin succeeded \n");
			} else {
				ESP_LOGE(TAG, "received package is not fit len");
			   // http_cleanup(client);
			   // task_fatal_error();
			   //ToDO: to see if need to do same task like Roll back or ...
			   return 4;
			}


		}//if (image_header_was_checked == false)

		printf(" OTA write to flash \n");
		err = esp_ota_write( update_handle, (const void *)ota_write_data, data_read);


		if (err != ESP_OK) {
			 //ToDO: to see if need to do same task like Roll back or ...
			printf(" OTA Written image err %d \n", err);
			ESP_LOGD(TAG, "Written image err %d", err);
		   return 5;
		}
		binary_file_length += data_read;
		//ESP_LOGD(TAG, "Written image length %d", binary_file_length);
		printf(" OTA Written image length %d \n", binary_file_length);

	}




    return ESP_OK;
}


esp_err_t OTA_End(void)
{
	 esp_err_t err;

	 uint8_t readData[2000];


	//err = esp_ota_read(update_handle, (const void *)readData, 1000);
	//ESP_LOGI(TAG, "esp_ota_read failed (%s)!", esp_err_to_name(err));




	if (esp_ota_end(update_handle) != ESP_OK) {
		ESP_LOGE(TAG, "esp_ota_end failed!");
		 //ToDO: to see if need to do same task like Roll back or ...
		   return 1;
	}



	err = esp_ota_set_boot_partition(update_partition);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "esp_ota_set_boot_partition failed (%s)!", esp_err_to_name(err));
		 //ToDO: to see if need to do same task like Roll back or ...
		  return 2;

	}



	ESP_LOGI(TAG, "Prepare to restart system!");
	esp_restart();
	return ESP_OK;

}

void OTA_init(void)
{
    uint8_t sha_256[HASH_LEN] = { 0 };
    esp_partition_t partition;

    // get sha256 digest for the partition table
    partition.address   = ESP_PARTITION_TABLE_OFFSET;
    partition.size      = ESP_PARTITION_TABLE_MAX_LEN;
    partition.type      = ESP_PARTITION_TYPE_DATA;
    esp_partition_get_sha256(&partition, sha_256);
    print_sha256(sha_256, "SHA-256 for the partition table: ");

    // get sha256 digest for bootloader
    partition.address   = ESP_BOOTLOADER_OFFSET;
    partition.size      = ESP_PARTITION_TABLE_OFFSET;
    partition.type      = ESP_PARTITION_TYPE_APP;
    esp_partition_get_sha256(&partition, sha_256);
    print_sha256(sha_256, "SHA-256 for bootloader: ");

    // get sha256 digest for running partition
    esp_partition_get_sha256(esp_ota_get_running_partition(), sha_256);
    print_sha256(sha_256, "SHA-256 for current firmware: ");


    const esp_partition_t *running_partition = esp_ota_get_running_partition();


    esp_ota_img_states_t ota_state;
    if (esp_ota_get_state_partition(running_partition, &ota_state) == ESP_OK) {
        if (ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
            // run diagnostic function ...
           //bool diagnostic_is_ok = diagnostic();
          // if (diagnostic_is_ok) {
               ESP_LOGI(TAG, "Diagnostics completed successfully! Continuing execution ...");
                esp_ota_mark_app_valid_cancel_rollback();
         //   } else {
        //        ESP_LOGE(TAG, "Diagnostics failed! Start rollback to the previous version ...");
        //       esp_ota_mark_app_invalid_rollback_and_reboot();
       //     }
        }
    }

    // Initialize NVS.

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // OTA app partition table has a smaller NVS partition size than the non-OTA
        // partition table. This size mismatch may cause NVS initialization to fail.
        // If this happens, we erase NVS partition and initialize NVS again.
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );







}

