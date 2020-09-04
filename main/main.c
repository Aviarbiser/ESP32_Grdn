/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "drv_rs485.h"
#include "protocol.h"
#include "dev_io.h"
#include "dev_logic.h"
#include "dev_memory.h"
#include "OTA.h"
#include "dev_wifi.h"
#include "dev_web_server.h"
#include "dev_time.h"
#include "esp32/rom/ets_sys.h"
#include "soc/rtc_periph.h"
#include "soc/sens_periph.h"

uint64_t DeviceID;
uint8_t DeviceMAC[6];



/* Can run 'make menuconfig' to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define BLINK_GPIO CONFIG_BLINK_GPIO
#define VALID_TIME_CONNECTION 60000

static void read_t_sens()
{

	SET_PERI_REG_BITS(SENS_SAR_MEAS_WAIT2_REG, SENS_FORCE_XPD_SAR, 3, SENS_FORCE_XPD_SAR_S);
	SET_PERI_REG_BITS(SENS_SAR_TSENS_CTRL_REG, SENS_TSENS_CLK_DIV, 10, SENS_TSENS_CLK_DIV_S);
	CLEAR_PERI_REG_MASK(SENS_SAR_TSENS_CTRL_REG, SENS_TSENS_POWER_UP);
	CLEAR_PERI_REG_MASK(SENS_SAR_TSENS_CTRL_REG, SENS_TSENS_DUMP_OUT);
	SET_PERI_REG_MASK(SENS_SAR_TSENS_CTRL_REG, SENS_TSENS_POWER_UP_FORCE);
	SET_PERI_REG_MASK(SENS_SAR_TSENS_CTRL_REG, SENS_TSENS_POWER_UP);
	ets_delay_us(100);
	SET_PERI_REG_MASK(SENS_SAR_TSENS_CTRL_REG, SENS_TSENS_DUMP_OUT);
	ets_delay_us(5);
	int res = GET_PERI_REG_BITS2(SENS_SAR_SLAVE_ADDR3_REG, SENS_TSENS_OUT, SENS_TSENS_OUT_S);
	printf("res=%d\n", res);
}


static void main_task(void *pvParameters)
{

	while(1) {
			//protocol_loop();
			logic_loop();
			//UART_read_task();
			IO_loop();
			/* Blink off (output low) */

			vTaskDelay(10);

		   // vTaskDelay(1000 / portTICK_PERIOD_MS);
		}

}



void app_main()
{


	//printf("starting main");
	printf("starting init \n");

	//get default mac addrass
	esp_err_t retErr =  esp_efuse_mac_get_default((uint8_t *)DeviceMAC);
	printf("Successfully received Local MAC Address : %02x:%02x:%02x:%02x:%02x:%02x\n",
			DeviceMAC[0],DeviceMAC[1],DeviceMAC[2],DeviceMAC[3],DeviceMAC[4],DeviceMAC[5]);

	printf("Program vertion is : %d . %d \n",DEVICE_VERSION,SUB_VERSION);
	//pack to 64 bit
	uint32_t dl1 = UINT32_FROM_BUFF(DeviceMAC, 0);
	uint32_t dl2 = UINT16_FROM_BUFF(DeviceMAC, 4);
	DeviceID = (((uint64_t)dl1) | ((uint64_t)dl2 << 32));
	OTA_init();
	rs485_setup();
	printf("done init rs485 \n");
	IO_init();
	printf("done init IO \n");
	//wifi_init_sta();
	//webserver_init();
	memory_init();
	logic_init();

	webserver_init();
	wifi_init_sta();

	printf("done init logic \n");
	//printf("done memory_init \n");
	//to do: change to status wakeup
	//motor_status = E_S_WAKEUP;//E_S_AUTO;//

	WriteToDigital(OUT_LED_R_PIN,1);
	//xTaskCreate(main_task, "main_task", 2048, NULL, 12, NULL);
	//xTaskCreate(UART_read_task, "uart_echo_task", 1024, NULL, 10, NULL);
	//gpio_pad_select_gpio(BLINK_GPIO);
	/* Set the GPIO as a push/pull output */
	//gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);


	while(1) {
		//protocol_loop();
		logic_loop();
		//UART_read_task();
		IO_loop();

		//check last valid connection if pass long time
		//set motor_status to Wakeup
		uint32_t mil = xTaskGetTickCount();

		//read_t_sens();
		vTaskDelay(1);

	   // vTaskDelay(1000 / portTICK_PERIOD_MS);
	}

}
