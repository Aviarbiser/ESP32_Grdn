/*
 * dev_wifi.c
 *
 *  Created on: 1 срхїз 2019
 *      Author: AviahiArbiser
 */
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "dev_time.h"
#include "mdns.h"

#include "dev_web_server.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "dev_wifi.h"

#define EXAMPLE_ESP_WIFI_SSID      "xarbiser"
#define EXAMPLE_ESP_WIFI_PASS      "0508942777"
#define EXAMPLE_ESP_MAXIMUM_RETRY  3

#define AP_WIFI_SSID      "ESP_bcddc2c1b2b9"
#define AP_WIFI_PASS      "esp123!@#"
#define AP_MAX_STA_CONN  2
extern uint8_t DeviceMAC[6];


 //mdns_server_t * mdns = NULL;


WIFI_PARM_STD wifi_station_parm;

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about one event
 * - are we connected to the AP with an IP? */
const int WIFI_CONNECTED_BIT = BIT0;

static const char *TAG = "wifi station";

static int s_retry_num = 0;

WIFI_STATUS_STD wifi_status;

 void init_wifi_ap()
{

	char mymac[20];
		sprintf(mymac,"%02x:%02x:%02x:%02x:%02x:%02x",DeviceMAC[0],DeviceMAC[1],DeviceMAC[2],DeviceMAC[3],DeviceMAC[4],DeviceMAC[5]);
		char ap_name[32];

		sprintf(ap_name,"ESP_%02x%02x%02x%02x%02x%02x",DeviceMAC[0],DeviceMAC[1],DeviceMAC[2],DeviceMAC[3],DeviceMAC[4],DeviceMAC[5]);
		printf("  mymac  ========= %s \n",mymac);

		wifi_config_t wifi_config = {
			.ap = {

					.ssid_len = strlen(ap_name),
					.authmode = WIFI_AUTH_WPA_WPA2_PSK,
					.password = AP_WIFI_PASS,
					.max_connection = AP_MAX_STA_CONN
				}

			};

		memcpy( wifi_config.ap.ssid,ap_name, sizeof(ap_name));
	//	wifi_config.ap.ssid_len = strlen(ap_name);



		tcpip_adapter_ip_info_t ip_info;
		IP4_ADDR(&ip_info.ip, 192, 168, 2, 1);
		IP4_ADDR(&ip_info.gw, 192, 168, 2, 1);
		IP4_ADDR(&ip_info.netmask, 255, 255, 255, 0);

		esp_wifi_stop();
		ESP_ERROR_CHECK(tcpip_adapter_ap_start((uint8_t *)mymac, &ip_info));

		ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP) );
		ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config) );
		ESP_ERROR_CHECK(esp_wifi_start() );



}

 static void start_mdns_service()
 {
     //initialize mDNS service on STA interface
     /*esp_err_t err = mdns_init(TCPIP_ADAPTER_IF_STA, &mdns);
     if (err) {
         printf("MDNS Init failed: %d\n", err);
         return;
     }
	*/
	 esp_err_t mdns_init();
     //set hostname  mdns
	 mdns_hostname_set("hostname");
	 mdns_instance_name_set("ESP32");

     printf("MDNS Init ok ");
 }

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        }
        else
        {
        	//cano't connect to AP open ESP in AP mode

        	init_wifi_ap();
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:%s",ip4addr_ntoa(&event->ip_info.ip));
        sprintf(wifi_status.IP,"%s",ip4addr_ntoa(&event->ip_info.ip));
        wifi_ap_record_t wifidata;
        if (esp_wifi_sta_get_ap_info(&wifidata)==0){
        	printf("rssi:%d\r\n", wifidata.rssi);
        	wifi_status.rssi = wifidata.rssi;
        }
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        //start http server
       // webserver_init();
        start_mdns_service();
        start_webserver();
        init_time();
        printf(" start_webserver STA\n");
    } else if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);

        start_webserver();
      //  webserver_init();
        printf(" start_webserver AP\n");
     //
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }

}

void wifi_config_sta()
{
	wifi_config_t wifi_config = {
			.sta = {

					.scan_method = WIFI_FAST_SCAN
				}
			};

	//ESP_LOGI(TAG, "wifi_config_sta start %d  pas %d.",strlen(wifi_station_parm.ssid),strlen(wifi_station_parm.password));

	//check if wifi ssid is null?
	if(strlen(wifi_station_parm.ssid) <= 0)
	{
		ESP_LOGI(TAG, "wifi_config_sta ssid is empty.");
		init_wifi_ap();
		return;
	}



	memset(wifi_config.sta.ssid,0,sizeof(wifi_config.sta.ssid));
	memset(wifi_config.sta.password,0,sizeof(wifi_config.sta.password));
	memcpy( wifi_config.sta.ssid,wifi_station_parm.ssid, strlen(wifi_station_parm.ssid));
	memcpy( wifi_config.sta.password,wifi_station_parm.password, strlen(wifi_station_parm.password));



	esp_wifi_stop();
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
	ESP_ERROR_CHECK(esp_wifi_start() );

	ESP_LOGI(TAG, "wifi_init_sta finished.");
	ESP_LOGI(TAG, "connect to ap SSID:%s password:%s",
			wifi_config.sta.ssid, wifi_config.sta.password);

}


/* WiFi init  */
void wifi_init_sta(void)
{
	s_wifi_event_group = xEventGroupCreate();

	//tcpip_adapter_init();

	//ESP_ERROR_CHECK(esp_event_loop_create_default());
//	esp_netif_create_default_wifi_ap();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
	ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

	wifi_config_sta();

	//ESP_LOGI(TAG, "wifi_init_sta finished.");
	//ESP_LOGI(TAG, "connect to ap SSID:%s password:%s",
		//	 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
}
