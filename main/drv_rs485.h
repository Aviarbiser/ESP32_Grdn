#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "freertos/queue.h"
#include "protocol.h"
#include "esp_system.h"
#include "esp_log.h"


#ifndef _DRV_RS485_H_
#define _DRV_RS485_H_

// SLIP constants
#define SLIP_START   0xC1
#define SLIP_END     0xC2
#define SLIP_ESC     0xC0



#define RS485_BUFFER_SIZE 500
#define RESPONSE_TO_MASTER_SIZE	70




/******************************************************************************************************************
Send a single character to the RS-485 line
******************************************************************************************************************/
extern void rs485_send_char(uint8_t * ch);
extern void rs485_send_data();
extern void stop_send_function();

/******************************************************************************************************************/
extern void rs485_send_buff(char* _buff, int _len);

/******************************************************************************************************************/


/******************************************************************************************************************
Initializing the RS-485 driver
******************************************************************************************************************/
void rs485_setup(void);

#endif // _DRV_RS485_H_
