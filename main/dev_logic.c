
/*
 * dev_logic.c
 *
 *  Created on: 20
 *      Author: AviahiArbiser
 */

#include "dev_logic.h"
#include "dev_time.h"
#include <inttypes.h>
#include "dev_memory.h"
#include "protocol.h"


uint8_t check_if_sun_in_reange();
uint8_t PID(int16_t vinput, int16_t vsetpoint);
#define MINMUM_PWM 0



float  PIDintegral, PIDlastError, PIDoutputCv;
int32_t   lastTime,prevmotorPos;
uint16_t speed_sp ;

VALVE_STD valve_parm[2];

int scen_count =0;


#define NOT_MOVE_DLY 20 //5 msec
#define WINDOW_SUN_POS_DB 5//5%

//#define SPEED_DLY 5000 //3 sec
//#define SPEED_START 15 // 50/255
//#define SPEED_RUN 50 //150/255
//#define RUN_TIMER 100000 * 60 * 2 //2 minute x
//#define POSITION_DB 2
//#define SPEED_ERROR_DB 10 //low speed detection
//#define COUNT_WINDOW_LENGTH 100000

uint8_t step=0;
uint8_t DeviceErrCount=0;

int16_t max_speed;
//device state 0->stop 1->running 3->error
uint8_t preodic_DeviceState = 0;
uint8_t preodic_DeviceStep = 0;

double sunZenith;
double sunAzimuth;

uint8_t window_sun_pos ;

//static const char* TAG = "motion";

/*
 * driver paramters
 */

int8_t position_SP ;//in % set from protocol
extern int8_t position_PV ;

uint8_t go_by_speed_direction ;//set from protocol
uint16_t go_by_speed_speed_sp ;//set from protocol




//internal use
uint32_t milifrom_start_motion ;
uint64_t mili_motion_dly ;


/*
 * timer microsec for motion
 * calc pulse count frenquncy
 * if working more then xx minute stop motion
 */
static void oneshot_timer_callback(void* arg);
esp_timer_handle_t oneshot_timer_handle;

uint32_t GetMili()
{

	return (uint32_t)xTaskGetTickCount() / portTICK_PERIOD_MS;
}



void logic_init()
{




	const esp_timer_create_args_t oneshot_timer_args = {
			.callback = &oneshot_timer_callback,
			/* argument specified here will be passed to timer callback function */
			//.arg = (void*) periodic_timer,
			.name = "one-shot"
		};

		ESP_ERROR_CHECK(esp_timer_create(&oneshot_timer_args, &oneshot_timer_handle));
}





static void oneshot_timer_callback(void* arg)
{
	 //int64_t time_since_boot = esp_timer_get_time();
	 //stop motion
	// stop();
	 printf("timer stop \n");
	// motion_step = E_ERROR;
	// driver_error_status = E_ERR_MOVING_TO_MUCH;

}

uint64_t motion_timer_get_time()
{

	return (uint64_t)esp_timer_get_time();
}



void VoltageErrorCheck()
{

	static uint32_t ErrorTimestamp = 0 ;
	/*
	 * Alarm voltage to low
	 * if Voltage above 12V retry
	 */

	if (Device_Volt < 80  )//<8.0V
	{
		if ( ErrorTimestamp == 0)
		{
			ErrorTimestamp = GetMili();
		}
		else
		{
			if(GetMili() > (ErrorTimestamp + 50))
			{

					printf("Device voltage to low %d \n",Device_Volt);

			}
		}
	}
	else if (Device_Volt > 120)
	{

		if(ErrorTimestamp == 0 )
		{
			ErrorTimestamp = GetMili();
		}

		if(GetMili() > (ErrorTimestamp + 50))
		{
			ErrorTimestamp = 0;
		}
	}
}




/*
 * function to check if time to move
 */
//DEV_EN_PIN 33 //close 1 7
//DEV_DIRECTION_PIN 27 //open1 8

int RelayOUT_Open[2] = {DEV_DIRECTION_PIN,DEV_PULSE_PIN};
int RelayOUT_Close[2] = {DEV_EN_PIN,DEV_BRAKE_PIN};
void CheckIfTime(int Index)
{

	static int minindayos = 0 ;
	time_t now;
	struct tm  timeinfo;



	setenv("IDT", "UTC+2", 1);
	time(&now);
	localtime_r(&now, &timeinfo);

	//get day of week
	int Dayw = timeinfo.tm_wday;
	int mininday = timeinfo.tm_hour * 60 + timeinfo.tm_min;

	// new day
	if (mininday == 0  )
	{
		if((valve_parm[Index].timeselect > 0) & (minindayos == 0))
		{
			minindayos = 1;
			valve_parm[Index].timeselect = 0;
		}
	}
	else
	{
		minindayos = 0;
	}


	//check if time in first time sp
	if( (mininday >= valve_parm[Index].hour_parm[Dayw].StartHour1 ) && (mininday < (valve_parm[Index].hour_parm[Dayw].StartHour1+valve_parm[Index].hour_parm[Dayw].Duration1) ) )
	{
		if ((valve_parm[Index].timeselect == 0))
		{
			valve_parm[Index].timeselect = 1;
			//open valve
			WriteToDigital(RelayOUT_Open[Index],1);
			vTaskDelay(10);
			WriteToDigital(RelayOUT_Open[Index],0);
			printf("Time loop R%d out = 1 1 \n ",RelayOUT_Open[Index]);
			//set position angle (in shutter)
		}
	}
	else
	{
		if(valve_parm[Index].timeselect == 1)
		{
			valve_parm[Index].timeselect = 0;
			//close valve
			WriteToDigital(RelayOUT_Close[Index],1);
			vTaskDelay(10);
			WriteToDigital(RelayOUT_Close[Index],0);
			printf("Time loop R%d out = 0 1 \n ",RelayOUT_Close[Index]);
		}
	}

	if( (mininday >= valve_parm[Index].hour_parm[Dayw].StartHour2 ) && (mininday <(valve_parm[Index].hour_parm[Dayw].StartHour2+valve_parm[Index].hour_parm[Dayw].Duration2) ) )
	{
		if ((valve_parm[Index].timeselect == 0))
		{
			valve_parm[Index].timeselect = 2;
			//open valve
			WriteToDigital(RelayOUT_Open[Index],1);
			vTaskDelay(10);
			WriteToDigital(RelayOUT_Open[Index],0);
			printf("Time loop R%d out = 1 2 \n ",RelayOUT_Open[Index]);
		}
			//set position angle (in shutter)
	}
	else
	{
		if(valve_parm[Index].timeselect == 2)
		{
			valve_parm[Index].timeselect = 0;
			//close valve
			WriteToDigital(RelayOUT_Close[Index],1);
			vTaskDelay(10);
			WriteToDigital(RelayOUT_Close[Index],0);
			printf("Time loop R%d out = 0 2 \n ",RelayOUT_Close[Index]);
		}
	}

	if( (mininday >= valve_parm[Index].hour_parm[Dayw].StartHour3 ) && (mininday < (valve_parm[Index].hour_parm[Dayw].StartHour3+valve_parm[Index].hour_parm[Dayw].Duration3)) )
	{
		if ((valve_parm[Index].timeselect == 0))
		{
			valve_parm[Index].timeselect = 3;
			//open valve
			WriteToDigital(RelayOUT_Open[Index],1);
			vTaskDelay(10);
			WriteToDigital(RelayOUT_Open[Index],0);
			printf("Time loop R%d out = 1 3 \n ",RelayOUT_Open[Index]);
		}
		//set position angle (in shutter)
	}
	else
	{
		if(valve_parm[Index].timeselect == 3)
		{
			valve_parm[Index].timeselect = 0;
			//close valve
			WriteToDigital(RelayOUT_Close[Index],1);
			vTaskDelay(10);
			WriteToDigital(RelayOUT_Close[Index],0);
			printf("Time loop R%d out = 0 3 \n ",RelayOUT_Close[Index]);
		}
	}



}


void logic_loop()
{
	//static uint8_t inMinute = 0;

	uint32_t mili =  GetMili();

	//if ((mili % 500)==0)
	//{
		CheckIfTime(0);
		CheckIfTime(1);
//	}
}






/*
 * in this function we check if the sun is in azimuth and Zenith to the window
 */
uint8_t check_if_sun_in_reange()
{



	uint16_t Sunrise,Sunset;
	getSPAdata( &sunZenith ,&sunAzimuth,&Sunrise,&Sunset);



	printf("Zenith %f, Azimuth %f Sunrise %d Sunset %d \n",sunZenith, sunAzimuth,Sunrise,Sunset);
	uint8_t pos = 0;

	return pos;
}






