/*
 * dev_time.c
 *
 *  Created on: 31 במאי 2019
 *      Author: AviahiArbiser
 */
#include "dev_time.h"
#include "esp_sntp.h"


time_t now;
struct tm  timeinfo;

uint16_t mininday;//0-1440


static void obtain_time(void);
void initialize_sntp(void);
void update_time(int yy,int mm,int dd,int hh,int MM,int ss);

void time_sync_notification_cb(struct timeval *tv)
{
   // ESP_LOGI(TAG, "Notification of a time synchronization event");
    printf("Notification of a time synchronization event \n");
}


void update_now_time(uint32_t Newval)
{


	 struct timeval  tv ;
	 struct timezone  tz;

	//set_boot_time(Newval);
	tv.tv_sec = Newval;
	tv.tv_usec =  7200;
	printf("Newval %d  \n",Newval);
   settimeofday(&tv, NULL);


}



void update_time(int yy,int mm,int dd,int hh,int MM,int ss)
{


	 struct tm tm;

	//set_boot_time(Newval);
	 tm.tm_year = yy - 1900;
	 tm.tm_mon = mm;
	 tm.tm_mday = dd;
	 tm.tm_hour = hh;
	 tm.tm_min = MM;
	 tm.tm_sec = ss;
	 time_t t = mktime(&tm);
	 struct timeval now = { .tv_sec = t };

	  settimeofday(&now, NULL);

	printf("set Newval yy %d mm %d dd %d hh %d MM %d ss %d \n",yy,mm,dd,hh,MM,ss);

}

/*
 * Solar Position Algorithm (SPA)
 */
void getSPAdata(double * Zenith ,double * Azimuth,uint16_t * Sunrise,uint16_t * Sunset )
{
	 spa_data spa;  //declare the SPA structure
	int result;
	float min, hour;



	//enter required input values into SPA structure
	time(&now);
	localtime_r(&now, &timeinfo);
	printf("time before tm_hour %d tm_min %d \n ",timeinfo.tm_hour,timeinfo.tm_min);

	now = now + 1 * 60;
	localtime_r(&now, &timeinfo);
	printf("time after tm_hour %d tm_min %d \n ",timeinfo.tm_hour,timeinfo.tm_min);

	spa.year          = timeinfo.tm_year+1900;
	spa.month         = timeinfo.tm_mon+1;
	spa.day           = timeinfo.tm_mday;
	spa.hour          = timeinfo.tm_hour;
	spa.minute        = timeinfo.tm_min;
	spa.second        = timeinfo.tm_sec;
	spa.timezone      = 7200 /3600 ;//motor_memory.timezone;
	spa.delta_ut1     = 0;
	spa.delta_t       = 67;
	spa.longitude     = (double)32351673 / 100000000.0 ;//32.351673, 35.018424
	spa.latitude      = (double)35018424 / 100000000.0 ;
	spa.elevation     = 0;
	spa.pressure      = 820;
	spa.temperature   = 20;
	spa.slope         = 30;
	spa.azm_rotation  = -10;
	spa.atmos_refract = 0.5667;
	spa.function      = SPA_ZA;

	//call the SPA calculate function and pass the SPA structure

	result = spa_calculate(&spa);
	if (result == 0)  //check for SPA errors
	{

		*Zenith = spa.zenith;
		*Azimuth = spa.azimuth;
		min = 60.0*(spa.sunrise - (int)(spa.sunrise));
		hour = (int)(spa.sunrise);
		*Sunrise = hour * 60 + min;
		min = 60.0*(spa.sunset - (int)(spa.sunset));
		hour = (int)(spa.sunset);
		*Sunset = hour * 60 + min;
	}
	else printf("SPA Error Code: %d\n", result);


}



long getLocalTime(char * mytimestring)
{

  //  time_t now;
	setenv("IDT", "UTC+2", 1);
	time(&now);
    localtime_r(&now, &timeinfo);


    sprintf(mytimestring,"%02d,%02d,%04d %02d:%02d:%02d",timeinfo.tm_mday,timeinfo.tm_mon+1,timeinfo.tm_year+1900,timeinfo.tm_hour,timeinfo.tm_min,timeinfo.tm_sec);


    mininday = timeinfo.tm_hour * 60 + timeinfo.tm_min;
    printf("time before tm_hour %d tm_min %d \n ",timeinfo.tm_hour,timeinfo.tm_min);
    long aa = (uint32_t)now;


    return aa;

}

void init_time()
{

	time_t now;
	struct tm timeinfo;
	time(&now);
	localtime_r(&now, &timeinfo);
	// Is time set? If not, tm_year will be (1970 - 1900).
	if (timeinfo.tm_year < (2016 - 1900)) {
		//ESP_LOGI(TAG, "Time is not set yet. Connecting to WiFi and getting time over NTP.");
		printf("Time is not set yet. Connecting to WiFi and getting time over NTP. \n ");
		obtain_time();
		// update 'now' variable with current time
		time(&now);
	}

}

static void obtain_time(void)
{
    //check if wifi is connected
	initialize_sntp();


    // wait for time to be set
    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;
    const int retry_count = 10;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
        //ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        printf("Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

   // setenv("RE", "CST+3", 1);
    setenv("IDT", "UTC+3", 1);
    tzset();
    char strftime_buf[64];
    time(&now);
    localtime_r(&now, &timeinfo);


    update_time(timeinfo.tm_year + 1900,timeinfo.tm_mon,timeinfo.tm_mday,timeinfo.tm_hour+3,timeinfo.tm_min,timeinfo.tm_sec);
    time(&now);
	localtime_r(&now, &timeinfo);

    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    printf("The current date/time in Israel is: %s \n", strftime_buf);

    //ESP_ERROR_CHECK( example_disconnect() );
}


void initialize_sntp(void)
{
   // ESP_LOGI(TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    sntp_init();
}




