  #include "dev_io.h"
#include "driver/rtc_io.h"
#include "driver/adc.h"
#include "protocol.h"
#include "dev_logic.h"
#include "driver/i2c.h"



#define ESP_INTR_FLAG_DEFAULT 0
int8_t position_PV = 0 ;//in %
bool dev_direction = 0;
uint16_t Device_Volt;
int io_val ;
uint8_t io_dig_mode = 0; //0 = ESp 1 = Server
float si7021_temperature;
float si7021_humidity;


//interapt function for pulse input (speed)
static void IRAM_ATTR pulse_isr_handler(void* arg)
{
  //  uint32_t gpio_num = (uint32_t) arg;
    //check gpio input state
    if (dev_direction == true)
    {
		dev_position_counter--;
    }
    else
    {
    	dev_position_counter++;

    }

	if(dev_position_counter<0)
	{
		dev_position_counter=0;
	}

	dev_counter++;
	//speed_count++;

	uint64_t micro =  motion_timer_get_time();
	uint64_t tim = micro - lst_micro_time;
	if(tim != 0) {
		dev_speed = 1000000/(tim);
	}
	lst_micro_time = micro;
    //int16_t pos = dev_position_counter - lst_position_counter;
    //pos = pos > 0 ? pos : pos * -1;

   //
   // uint16_t tim = (micro - lst_pulse_time);
  //  tim = tim == 0 ? 1 : tim;
    //dev_speed = 1000000 / tim ;// 1000 * 1000 ; //time in sec
   // ets_printf("dev_speed %d tim %d\n",dev_speed,tim);
  //  lst_pulse_time = micro;
    //lst_position_counter = dev_position_counter;

}


uint32_t mili_dly;



void IO_init()
{

	mili_dly = 0;
	//set analog output

	//adc2_config_channel_atten( DEV_VSYS, ADC_ATTEN_11db );
	adc1_config_width(ADC_WIDTH_12Bit);
	adc1_config_channel_atten( DEV_R_CURRENT, ADC_ATTEN_11db );
	adc1_config_channel_atten( DEV_VSYS, ADC_ATTEN_11db );

	dac_output_enable(DEV_VREFF_OUT_CH);
	//set digital input/output
	gpio_config_t io_conf;

	//disable interrupt
	io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
	//set as output mode
	io_conf.mode = GPIO_MODE_OUTPUT;
	//bit mask of the pins that you want to set,e.g.GPIO18/19
	io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
	//disable pull-down mode
	io_conf.pull_down_en = 0;
	//disable pull-up mode
	io_conf.pull_up_en = 0;
	//configure GPIO with the given settings
	gpio_config(&io_conf);

	//interrupt of rising edge
	io_conf.intr_type = GPIO_PIN_INTR_POSEDGE;
	//bit mask of the pins, use GPIO4/5 here
	io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
	//set as input mode
	io_conf.mode = GPIO_MODE_INPUT;
	//enable pull-up mode
	io_conf.pull_up_en = 1;
	gpio_config(&io_conf);

	//set EN1 (BLDC driver enable pin) to output and input mode
	//esp_err_t err1 =  gpio_set_direction(DEV_EN_PIN, GPIO_MODE_INPUT_OUTPUT );

	//change gpio intrrupt type for one pin
	//gpio_set_intr_type(DEV_PULSE_PIN, GPIO_INTR_ANYEDGE);

	//install gpio isr service
	gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
	//hook isr handler for specific gpio pin
	gpio_isr_handler_add(DEV_PULSE_PIN, pulse_isr_handler, (void*) DEV_PULSE_PIN);



	//set Brake in driver to high by default
	//WriteToDigital(DEV_BRAKE_PIN,1);
	//WriteToDigital(OUT_LED_R_PIN,1);

}






esp_err_t WriteToAnloag(uint8_t ch ,uint8_t val)
{

	return dac_output_voltage(ch, val);

}


esp_err_t ESPWriteToDigital(uint8_t ch ,uint8_t val)
{
// if flav==false

	return gpio_set_level(ch, val);

}


esp_err_t WriteToDigital(uint8_t ch ,uint8_t val)
{
// if flav==false
	if(io_dig_mode == 0)
	{
		return ESPWriteToDigital(ch, val);
	}
	return ESP_OK;
}

//#define CHECKIO 0


int shutter_pulse,IN1=1,IN2,IN3,SW1;
uint32_t timetoreadana;
void IO_loop()
{

	int read_raw;

	if ((xTaskGetTickCount() -timetoreadana ) > 500)
	{
		timetoreadana = xTaskGetTickCount();
		//esp_err_t r = adc2_get_raw( DEV_VSYS, ADC_WIDTH_12Bit, &read_raw);
		read_raw = adc1_get_raw(DEV_VSYS);
		//if ( r == ESP_OK ) {
		Device_Volt =  read_raw * 220 / 2224 + 20;
		//printf("ADC2 = %d V = %d \n", read_raw,Device_Volt );
		//} else if ( r == ESP_ERR_TIMEOUT ) {
		//	printf("ADC2 used by Wi-Fi.\n");
		//}
		io_val = (gpio_get_level(IN1_PIN) & 1) | ((gpio_get_level(IN2_PIN) & 1) << 1) | ((gpio_get_level(IN3_PIN) & 1) << 2);



	}


	//check IO loop
	if ( gpio_get_level(IN2_PIN) != IN2)
	{

	/*	IN2 = gpio_get_level(IN2_PIN);
		printf(" WriteToDigital R2 %d  \n", IN2);
		//WriteToDigital(OUT_R2_PIN,IN2);
		if(IN2==0){

			motor_status =E_S_ONLINE;//E_S_WAKEUP;//
			motor_memory.DataValid = 99;
			printf(" Motor go direction %d , Speed %d \n", 1, 50);

			 motion_step = E_MOTION_SPEED;
			 go_by_speed_direction = 0;
			 go_by_speed_speed_sp = 10;
		}
		else
		{
			printf(" io stop \n");
			stop();
			motion_step = E_DONE;
		}
		*/
	}


	if ( gpio_get_level(SHUTTER_PULSE_PIN) != shutter_pulse)
	{

		shutter_pulse = gpio_get_level(SHUTTER_PULSE_PIN);
		if(shutter_pulse==0){

			//motor_status =E_S_ONLINE;//E_S_WAKEUP;//
			//motor_memory.DataValid = 99;
			//last_valid_connection = xTaskGetTickCount();


			printf(" Motor go direction %d , Speed %d \n", 1, 200);

		}
		else
		{
			printf(" io stop \n");

		}
	}


	if ( gpio_get_level(IN1_PIN) != IN1)
	{
/*
		IN1 = gpio_get_level(IN1_PIN);
		if(IN1==0){

			motor_status =E_S_ONLINE;//E_S_WAKEUP;//
			motor_memory.DataValid = 99;
			printf(" Motor go direction %d , Speed %d \n", 1, 50);

			 motion_step = E_MOTION_SPEED;
			 go_by_speed_direction = 1;
			 go_by_speed_speed_sp = 50;
		}
		else
		{
			printf(" io stop \n");
			stop();
			motion_step = E_DONE;
		}
		*/
	}




	if ( gpio_get_level(IN3_PIN) != IN3)
		{

			IN3 = gpio_get_level(IN3_PIN);
			if(IN3==0){


			}
			else
			{
				//motor_status = E_S_ONLINE;

			}
		}





}
