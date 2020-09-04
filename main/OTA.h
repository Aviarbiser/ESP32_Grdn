/*
 * OTA.h
 *
 *  Created on: 3 בספט׳ 2019
 *      Author: AviahiArbiser
 */

#ifndef MAIN_OTA_H_
#define MAIN_OTA_H_





void OTA_init(void);
esp_err_t ota_task(uint8_t * ota_write_data ,int data_read);
esp_err_t starting_ota(void);
esp_err_t OTA_End(void);

#endif /* MAIN_OTA_H_ */
