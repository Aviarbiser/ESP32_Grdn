/*
 * dev_io.h
 *
 *  Created on: 20 במאי 2019
 *      Author: AviahiArbiser
 */

#ifndef MAIN_DEV_IO_H_
#define MAIN_DEV_IO_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/dac.h"
#include "sdkconfig.h"
#include "dev_logic.h"


//analog
#define DEV_VREFF_OUT_CH 1 //DAC channel 1 is GPIO25

#define DEV_VSYS ADC1_CHANNEL_3  //SENSOR_VN ADC GPIO4 = ADC2_CHANNEL_0
#define DEV_R_CURRENT ADC1_CHANNEL_7 //GPIO-35 GPIO-36 SENSOR

#define DEV_AI_PIN 1 //current/speed (voltage ref)

#define DEV_AI_PIN 1 //AI4

//output
#define DEV_EN_PIN 33 //close 1 7
#define DEV_DIRECTION_PIN 27 //open1 8
#define DEV_BRAKE_PIN  23 //close 2 4

#define DEV_PULSE_PIN 19 //open 2 9


#define SHUTTER_PULSE_PIN 34 //J10
#define IN1_PIN 35 //J14
#define IN2_PIN 36 //J11 SENSOR_VP
#define IN3_PIN 4 //J13 GPIO4  SENSOR_VN 39

#define OUT_R1_PIN 5
#define OUT_R2_PIN 2 //CMD (19)
#define OUT_LED_R_PIN 32
#define OUT_LED_G_PIN 26

#define SDA_PIN 21
#define SCL_PIN 22

//#define SW1_PIN 0 //
//#define SW2_PIN 0 //EN




#define GPIO_OUTPUT_PIN_SEL ((1ULL<<DEV_DIRECTION_PIN) | (1ULL<<DEV_BRAKE_PIN) |(1ULL<<OUT_R1_PIN)\
      | (1ULL<<DEV_PULSE_PIN) |(1ULL<<OUT_LED_R_PIN)  |(1ULL<<OUT_LED_G_PIN) |(1ULL<<OUT_R2_PIN) |(1ULL<<DEV_EN_PIN))



#define GPIO_INPUT_PIN_SEL  (  (1ULL<<IN1_PIN)  | \
		(1ULL<<IN2_PIN)  | (1ULL<<IN3_PIN))


volatile int32_t dev_position_counter;
volatile int32_t dev_counter;

volatile int32_t lst_counter;
volatile uint64_t lst_pulse_time;
volatile uint64_t lst_micro_time;


volatile int16_t dev_speed;
volatile int16_t speed_count;

extern uint16_t Device_Volt;
extern uint8_t io_dig_mode;

extern float si7021_temperature;
extern float si7021_humidity;

extern int io_val ;
extern bool dev_direction;
extern int8_t position_PV;

extern esp_err_t WriteToAnloag(uint8_t ch ,uint8_t val);
extern esp_err_t WriteToDigital(uint8_t ch ,uint8_t val);
esp_err_t ESPWriteToDigital(uint8_t ch ,uint8_t val);
extern void IO_loop();
extern void IO_init();

#endif /* MAIN_DEV_IO_H_ */
