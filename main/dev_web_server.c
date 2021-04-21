/*
 * dev_web_server.c
 *
 *  Created on: 1 срхїз 2019
 *      Author: AviahiArbiser
 */

#include <esp_event.h>
#include <esp_log.h>
#include "esp_system.h"
#include <nvs_flash.h>
#include <sys/param.h>
#include "nvs_flash.h"
#include "tcpip_adapter.h"
#include "esp_eth.h"
#include <string.h>
//#include "protocol_examples_common.h"
#include "esp_http_client.h"
#include "esp_http_server.h"
#include "string.h"
#include "cJSON.h"
#include "dev_logic.h"
#include "protocol.h"
#include "dev_wifi.h"
#include "OTA.h"
#include "dev_memory.h"
#include "dev_wifi.h"
#include "dev_time.h"


static const char *TAG = "Http example";


httpd_handle_t server = NULL;
#define BUFFSIZE 1024
static char os_list_data[BUFFSIZE + 1] = { 0 };
uint8_t os_data[BUFFSIZE + 1] = { 0 };


/* An HTTP GET handler */
static esp_err_t hello_get_handler(httpd_req_t *req)
{
    char*  buf;
    size_t buf_len;

    /* Get header value string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Host: %s", buf);
        }
        free(buf);
    }

    buf_len = httpd_req_get_hdr_value_len(req, "Test-Header-2") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_hdr_value_str(req, "Test-Header-2", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Test-Header-2: %s", buf);
        }
        free(buf);
    }

    buf_len = httpd_req_get_hdr_value_len(req, "Test-Header-1") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_hdr_value_str(req, "Test-Header-1", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Test-Header-1: %s", buf);
        }
        free(buf);
    }

    /* Read URL query string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found URL query => %s", buf);
            char param[32];
            /* Get value of expected key from query string */
            if (httpd_query_key_value(buf, "query1", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(TAG, "Found URL query parameter => query1=%s", param);
            }
            if (httpd_query_key_value(buf, "query3", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(TAG, "Found URL query parameter => query3=%s", param);
            }
            if (httpd_query_key_value(buf, "query2", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(TAG, "Found URL query parameter => query2=%s", param);
            }
        }
        free(buf);
    }

    /* Set some custom headers */
    httpd_resp_set_hdr(req, "Custom-Header-1", "Custom-Value-1");
    httpd_resp_set_hdr(req, "Custom-Header-2", "Custom-Value-2");

    /* Send response with custom headers and body set as the
     * string passed in user context*/
    const char* resp_str = (const char*) req->user_ctx;
    httpd_resp_send(req, resp_str, strlen(resp_str));

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

static const httpd_uri_t hello = {
    .uri       = "/hello",
    .method    = HTTP_GET,
    .handler   = hello_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = "Hello World!"
};

/* An HTTP POST handler */
static esp_err_t echo_post_handler(httpd_req_t *req)
{
	 int total_len = req->content_len;
		int cur_len = 0;
		int received = 0;
		if(total_len > 0)
		{


			//size_t len =  heap_caps_get_free_size(MALLOC_CAP_8BIT);
			printf("param_ajax_handler total_len %d  \n",total_len);


		}

	    //printf("param_ajax_handler \n");

	    cJSON *root, *prm,*fmt, *hur;
		root = cJSON_CreateObject();
		cJSON_AddItemToObject(root, "status", fmt=cJSON_CreateObject());
		//cJSON_AddNumberToObject(fmt,"debug_inf",motor_current);
		cJSON_AddNumberToObject(fmt,"Device_version",DEVICE_VERSION);
		cJSON_AddNumberToObject(fmt,"Device_Volt",Device_Volt);
		cJSON_AddNumberToObject(fmt,"SUB_version",SUB_VERSION);
		cJSON_AddStringToObject(fmt,"WIFI_IP",wifi_status.IP);
		cJSON_AddNumberToObject(fmt,"WIFI_Signal",wifi_status.rssi);
		cJSON_AddNumberToObject(fmt,"WIFI_Signal",wifi_status.rssi);

		char mytime[30];
		uint32_t time = getLocalTime(mytime);
		cJSON_AddStringToObject(fmt,"Clock",mytime);



		//loop for load hours time
		cJSON_AddItemToObject(root, "hours", hur=cJSON_CreateObject());
		char objName[15];

		for(int i = 0;i<7;i++)
		{
			//hours for valve 1
			sprintf(objName,"StartHour1_1_%d",(i+1));
			cJSON_AddNumberToObject(hur,objName,valve_parm[0].hour_parm[i].StartHour1);
			sprintf(objName,"StartHour1_2_%d",(i+1));
			cJSON_AddNumberToObject(hur,objName,valve_parm[0].hour_parm[i].StartHour2);
			sprintf(objName,"StartHour1_3_%d",(i+1));
			cJSON_AddNumberToObject(hur,objName,valve_parm[0].hour_parm[i].StartHour3);
			sprintf(objName,"Duration1_1_%d",(i+1));
			cJSON_AddNumberToObject(hur,objName,valve_parm[0].hour_parm[i].Duration1);
			sprintf(objName,"Duration1_2_%d",(i+1));
			cJSON_AddNumberToObject(hur,objName,valve_parm[0].hour_parm[i].Duration2);
			sprintf(objName,"Duration1_3_%d",(i+1));
			cJSON_AddNumberToObject(hur,objName,valve_parm[0].hour_parm[i].Duration3);
			//hours for valve 2
			sprintf(objName,"StartHour2_1_%d",(i+1));
			cJSON_AddNumberToObject(hur,objName,valve_parm[1].hour_parm[i].StartHour1);
			sprintf(objName,"StartHour2_2_%d",(i+1));
			cJSON_AddNumberToObject(hur,objName,valve_parm[1].hour_parm[i].StartHour2);
			sprintf(objName,"StartHour2_3_%d",(i+1));
			cJSON_AddNumberToObject(hur,objName,valve_parm[1].hour_parm[i].StartHour3);
			sprintf(objName,"Duration2_1_%d",(i+1));
			cJSON_AddNumberToObject(hur,objName,valve_parm[1].hour_parm[i].Duration1);
			sprintf(objName,"Duration2_2_%d",(i+1));
			cJSON_AddNumberToObject(hur,objName,valve_parm[1].hour_parm[i].Duration2);
			sprintf(objName,"Duration2_3_%d",(i+1));
			cJSON_AddNumberToObject(hur,objName,valve_parm[1].hour_parm[i].Duration3);
		}


		const char *sys_info = cJSON_Print(root);

		httpd_resp_set_type(req, "application/json");
		httpd_resp_sendstr(req, sys_info);
		free((void *)sys_info);
		cJSON_Delete(root);

	    return ESP_OK;
}


void save_parmdata(char * buf,int recive_count)
{

	//printf("save_parmdata buf %s \n",buf);

	printf(" strlen %d recive_count %d =========================== \n",strlen(buf),recive_count);
	int rcount =strlen(buf);

	//char * bufx = (char*) malloc(rcount);
	//sprintf(bufx,"%.*s", rcount, (char*)buf);
	//printf("%s \n",  (char*)bufx);
	cJSON *root = cJSON_Parse(buf);
	if (root == NULL)
	{
		const char *error_ptr = cJSON_GetErrorPtr();
		if (error_ptr != NULL)
		{
			printf( "Error before: %s\n", error_ptr);
		}

	}
	else
	{


		int yy = cJSON_GetObjectItem(root, "time_y")->valueint;
		int mm = cJSON_GetObjectItem(root, "time_m")->valueint;
		int dd = cJSON_GetObjectItem(root, "time_d")->valueint;
		int hh = cJSON_GetObjectItem(root, "time_h")->valueint;
		int MM = cJSON_GetObjectItem(root, "time_mm")->valueint;
		int ss = cJSON_GetObjectItem(root, "time_s")->valueint;
		/*
		 * postData += '"time_y":' + d.getFullYear() + ',';
		postData += '"time_m":' + d.getMonth() + ',';
		postData += '"time_d":' + d.getDate() + ',';
		postData += '"time_h":' + d.getHours() + ',';
		postData += '"time_mm":' + d.getMinutes() + ',';
		postData += '"time_s":' + d.getSeconds() + '}';
		 *
		 */
		update_time( yy, mm, dd, hh, MM, ss);


		//wifi data


		char objName[15];

		for(int i = 0;i<7;i++)
		{
			sprintf(objName,"StartHour1_1_%d",(i+1));
			valve_parm[0].hour_parm[i].StartHour1 = cJSON_GetObjectItem(root, objName)->valueint;
			sprintf(objName,"StartHour1_2_%d",(i+1));
			valve_parm[0].hour_parm[i].StartHour2 = cJSON_GetObjectItem(root, objName)->valueint;
			sprintf(objName,"StartHour1_3_%d",(i+1));
			valve_parm[0].hour_parm[i].StartHour3 = cJSON_GetObjectItem(root, objName)->valueint;

			sprintf(objName,"Duration1_1_%d",(i+1));
			valve_parm[0].hour_parm[i].Duration1 = cJSON_GetObjectItem(root, objName)->valueint;
			sprintf(objName,"Duration1_2_%d",(i+1));
			valve_parm[0].hour_parm[i].Duration2 = cJSON_GetObjectItem(root, objName)->valueint;
			sprintf(objName,"Duration1_3_%d",(i+1));
			valve_parm[0].hour_parm[i].Duration3 = cJSON_GetObjectItem(root, objName)->valueint;

			sprintf(objName,"StartHour2_1_%d",(i+1));
			valve_parm[1].hour_parm[i].StartHour1 = cJSON_GetObjectItem(root, objName)->valueint;
			sprintf(objName,"StartHour2_2_%d",(i+1));
			valve_parm[1].hour_parm[i].StartHour2 = cJSON_GetObjectItem(root, objName)->valueint;
			sprintf(objName,"StartHour2_3_%d",(i+1));
			valve_parm[1].hour_parm[i].StartHour3 = cJSON_GetObjectItem(root, objName)->valueint;

			sprintf(objName,"Duration2_1_%d",(i+1));
			valve_parm[1].hour_parm[i].Duration1 = cJSON_GetObjectItem(root, objName)->valueint;
			sprintf(objName,"Duration2_2_%d",(i+1));
			valve_parm[1].hour_parm[i].Duration2 = cJSON_GetObjectItem(root, objName)->valueint;
			sprintf(objName,"Duration2_3_%d",(i+1));
			valve_parm[1].hour_parm[i].Duration3 = cJSON_GetObjectItem(root, objName)->valueint;

			//valve 3
			sprintf(objName,"StartHour3_1_%d",(i+1));
			valve_parm[2].hour_parm[i].StartHour1 = cJSON_GetObjectItem(root, objName)->valueint;
			sprintf(objName,"StartHour3_2_%d",(i+1));
			valve_parm[2].hour_parm[i].StartHour2 = cJSON_GetObjectItem(root, objName)->valueint;
			sprintf(objName,"StartHour3_3_%d",(i+1));
			valve_parm[2].hour_parm[i].StartHour3 = cJSON_GetObjectItem(root, objName)->valueint;

			sprintf(objName,"Duration3_1_%d",(i+1));
			valve_parm[2].hour_parm[i].Duration1 = cJSON_GetObjectItem(root, objName)->valueint;
			sprintf(objName,"Duration3_2_%d",(i+1));
			valve_parm[2].hour_parm[i].Duration2 = cJSON_GetObjectItem(root, objName)->valueint;
			sprintf(objName,"Duration3_3_%d",(i+1));
			valve_parm[2].hour_parm[i].Duration3 = cJSON_GetObjectItem(root, objName)->valueint;

			//valve 4
			sprintf(objName,"StartHour4_1_%d",(i+1));
			valve_parm[3].hour_parm[i].StartHour1 = cJSON_GetObjectItem(root, objName)->valueint;
			sprintf(objName,"StartHour4_2_%d",(i+1));
			valve_parm[3].hour_parm[i].StartHour2 = cJSON_GetObjectItem(root, objName)->valueint;
			sprintf(objName,"StartHour4_3_%d",(i+1));
			valve_parm[3].hour_parm[i].StartHour3 = cJSON_GetObjectItem(root, objName)->valueint;

			sprintf(objName,"Duration4_1_%d",(i+1));
			valve_parm[3].hour_parm[i].Duration1 = cJSON_GetObjectItem(root, objName)->valueint;
			sprintf(objName,"Duration4_2_%d",(i+1));
			valve_parm[3].hour_parm[i].Duration2 = cJSON_GetObjectItem(root, objName)->valueint;
			sprintf(objName,"Duration4_3_%d",(i+1));
			valve_parm[3].hour_parm[i].Duration3 = cJSON_GetObjectItem(root, objName)->valueint;

		}
		SaveHoursToMemory();




	}

	printf(" save_parmdata done \n");
	//free(bufx);



}


void save_ID_data(char * buf,int recive_count)
{

	//printf("save_parmdata buf %s \n",buf);

	printf(" strlen %d =========================== \n",strlen(buf));

	char * bufx = (char*) malloc(recive_count);
	sprintf(bufx,"%.*s", recive_count, (char*)buf);
	printf("%s \n",  (char*)bufx);
	cJSON *root = cJSON_Parse(bufx);
	if (root != NULL)
	{


		uint32_t time = cJSON_GetObjectItem(root, "time")->valueint;
		printf("save_parmdata time %d \n",time);
		//wifi data

		//motor param

		DeviceIDs.ID1 = cJSON_GetObjectItem(root, "ID1")->valueint;
		DeviceIDs.ID2 = cJSON_GetObjectItem(root, "ID2")->valueint;
		DeviceIDs.ID3 = cJSON_GetObjectItem(root, "ID3")->valueint;
		DeviceIDs.ID4 = cJSON_GetObjectItem(root, "ID4")->valueint;

		SaveDeviceIDsToMemory();


	}

	printf(" save_ID_data done \n");
	free(bufx);



}


char rbuf[2000];
/* An HTTP POST handler */
static esp_err_t param_ajax_handler(httpd_req_t *req)
{



    int total_len = req->content_len;
	int cur_len = 0;
	int received = 0;
	if(total_len > 0)
	{

		//char * buf = (char*) malloc(total_len);
		//size_t len =  heap_caps_get_free_size(MALLOC_CAP_8BIT);
		printf("param_ajax_handler total_len %d  \n",total_len);

		//memset(buf,0,total_len);


		while (cur_len < total_len) {
			received = httpd_req_recv(req, rbuf + cur_len, total_len);
			printf("param_ajax_handler received %d \n",received);
			if (received <= 0) {
				// Respond with 500 Internal Server Error
				httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
				//free(buf);
				return ESP_FAIL;
			}
			cur_len += received;
		}
		//received = httpd_req_recv(req, buf+cur_len , req->content_len);

		rbuf[total_len] = '\0';
		printf("param_ajax_handler  rbuf %s\n",rbuf);



		save_parmdata(rbuf,total_len);
		//free(buf);

	}

    //printf("param_ajax_handler \n");

    cJSON *root, *prm,*fmt, *hur;
	root = cJSON_CreateObject();
	cJSON_AddItemToObject(root, "status", fmt=cJSON_CreateObject());
	//cJSON_AddNumberToObject(fmt,"debug_inf",motor_current);
	cJSON_AddNumberToObject(fmt,"Device_version",DEVICE_VERSION);
	cJSON_AddNumberToObject(fmt,"Device_Volt",Device_Volt);
	cJSON_AddNumberToObject(fmt,"SUB_version",SUB_VERSION);
	cJSON_AddStringToObject(fmt,"WIFI_IP",wifi_status.IP);
	cJSON_AddNumberToObject(fmt,"WIFI_Signal",wifi_status.rssi);


	//loop for load hours time
	cJSON_AddItemToObject(root, "hours", hur=cJSON_CreateObject());
	char objName[15];

	for(int i = 0;i<7;i++)
	{
		//hours for valve 1
		sprintf(objName,"StartHour1_1_%d",(i+1));
		cJSON_AddNumberToObject(hur,objName,valve_parm[0].hour_parm[i].StartHour1);
		sprintf(objName,"StartHour1_2_%d",(i+1));
		cJSON_AddNumberToObject(hur,objName,valve_parm[0].hour_parm[i].StartHour2);
		sprintf(objName,"StartHour1_3_%d",(i+1));
		cJSON_AddNumberToObject(hur,objName,valve_parm[0].hour_parm[i].StartHour3);
		sprintf(objName,"Duration1_1_%d",(i+1));
		cJSON_AddNumberToObject(hur,objName,valve_parm[0].hour_parm[i].Duration1);
		sprintf(objName,"Duration1_2_%d",(i+1));
		cJSON_AddNumberToObject(hur,objName,valve_parm[0].hour_parm[i].Duration2);
		sprintf(objName,"Duration1_3_%d",(i+1));
		cJSON_AddNumberToObject(hur,objName,valve_parm[0].hour_parm[i].Duration3);
		//hours for valve 2
		sprintf(objName,"StartHour2_1_%d",(i+1));
		cJSON_AddNumberToObject(hur,objName,valve_parm[1].hour_parm[i].StartHour1);
		sprintf(objName,"StartHour2_2_%d",(i+1));
		cJSON_AddNumberToObject(hur,objName,valve_parm[1].hour_parm[i].StartHour2);
		sprintf(objName,"StartHour2_3_%d",(i+1));
		cJSON_AddNumberToObject(hur,objName,valve_parm[1].hour_parm[i].StartHour3);
		sprintf(objName,"Duration2_1_%d",(i+1));
		cJSON_AddNumberToObject(hur,objName,valve_parm[1].hour_parm[i].Duration1);
		sprintf(objName,"Duration2_2_%d",(i+1));
		cJSON_AddNumberToObject(hur,objName,valve_parm[1].hour_parm[i].Duration2);
		sprintf(objName,"Duration2_3_%d",(i+1));
		cJSON_AddNumberToObject(hur,objName,valve_parm[1].hour_parm[i].Duration3);
		//hours for valve 3
		sprintf(objName,"StartHour3_1_%d",(i+1));
		cJSON_AddNumberToObject(hur,objName,valve_parm[2].hour_parm[i].StartHour1);
		sprintf(objName,"StartHour3_2_%d",(i+1));
		cJSON_AddNumberToObject(hur,objName,valve_parm[2].hour_parm[i].StartHour2);
		sprintf(objName,"StartHour3_3_%d",(i+1));
		cJSON_AddNumberToObject(hur,objName,valve_parm[2].hour_parm[i].StartHour3);
		sprintf(objName,"Duration3_1_%d",(i+1));
		cJSON_AddNumberToObject(hur,objName,valve_parm[2].hour_parm[i].Duration1);
		sprintf(objName,"Duration3_2_%d",(i+1));
		cJSON_AddNumberToObject(hur,objName,valve_parm[2].hour_parm[i].Duration2);
		sprintf(objName,"Duration3_3_%d",(i+1));
		cJSON_AddNumberToObject(hur,objName,valve_parm[2].hour_parm[i].Duration3);
		//hours for valve 4
		sprintf(objName,"StartHour4_1_%d",(i+1));
		cJSON_AddNumberToObject(hur,objName,valve_parm[4].hour_parm[i].StartHour1);
		sprintf(objName,"StartHour4_2_%d",(i+1));
		cJSON_AddNumberToObject(hur,objName,valve_parm[4].hour_parm[i].StartHour2);
		sprintf(objName,"StartHour4_3_%d",(i+1));
		cJSON_AddNumberToObject(hur,objName,valve_parm[4].hour_parm[i].StartHour3);
		sprintf(objName,"Duration4_1_%d",(i+1));
		cJSON_AddNumberToObject(hur,objName,valve_parm[4].hour_parm[i].Duration1);
		sprintf(objName,"Duration4_2_%d",(i+1));
		cJSON_AddNumberToObject(hur,objName,valve_parm[4].hour_parm[i].Duration2);
		sprintf(objName,"Duration4_3_%d",(i+1));
		cJSON_AddNumberToObject(hur,objName,valve_parm[4].hour_parm[i].Duration3);
	}


	const char *sys_info = cJSON_Print(root);

	httpd_resp_set_type(req, "application/json");
	httpd_resp_sendstr(req, sys_info);
	free((void *)sys_info);
	cJSON_Delete(root);

    return ESP_OK;
}







/* This handler allows the custom error handling functionality to be
 * tested from client side. For that, when a PUT request 0 is sent to
 * URI /ctrl, the /hello and /echo URIs are unregistered and following
 * custom error handler http_404_error_handler() is registered.
 * Afterwards, when /hello or /echo is requested, this custom error
 * handler is invoked which, after sending an error message to client,
 * either closes the underlying socket (when requested URI is /echo)
 * or keeps it open (when requested URI is /hello). This allows the
 * client to infer if the custom error handler is functioning as expected
 * by observing the socket state.
 */
esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    if (strcmp("/hello", req->uri) == 0) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/hello URI is not available");
        /* Return ESP_OK to keep underlying socket open */
        return ESP_OK;
    } else if (strcmp("/echo", req->uri) == 0) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/echo URI is not available");
        /* Return ESP_FAIL to close underlying socket */
        return ESP_FAIL;
    }
    /* For any other URI send 404 and close socket */
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
    return ESP_FAIL;
}




static esp_err_t index_get_handler(httpd_req_t *req)
{
    char*  buf;
    size_t buf_len;
    /* Get handle to embedded file upload script */
       extern const unsigned char index_start[] asm("_binary_index_html_start");
       extern const unsigned char index_end[]   asm("_binary_index_html_end");
       const size_t index_size = (index_end - index_start);

       /* Add file upload form and script which on execution sends a POST request to /upload */
       httpd_resp_send(req, (const char *)index_start, index_size);
       ESP_LOGI(TAG, "index_get_handler");
//httpd_resp_send_chunk
    /* Send a simple response */
   //const char resp[] = "URI GET Response";
   //httpd_resp_send(req, resp, strlen(resp));
   return ESP_OK;

}

static void http_cleanup(esp_http_client_handle_t client)
{
    esp_http_client_close(client);
    esp_http_client_cleanup(client);
}


int download_os_update_list(char * os_path)
{

	esp_err_t err;

	esp_http_client_config_t config = {
		.url = "http://www.arbicom.co.il/FTP/ESP_os_update_list.json",
	//	.cert_pem = (char *)server_cert_pem_start,
	};
	esp_http_client_handle_t client = esp_http_client_init(&config);
	if (client == NULL) {
		ESP_LOGE(TAG, "download_os_update_list Failed to initialise HTTP connection");
		//task_fatal_error();
		return -1;
	}
	esp_http_client_set_header(client, "Content-Type", "application/json");
	err = esp_http_client_open(client, 0);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
		esp_http_client_cleanup(client);
		//task_fatal_error();
		return -2;
	}

	int ret = esp_http_client_fetch_headers(client);
	ESP_LOGE(TAG, "fetch_headers %d",ret);

	int recive_count=0;
	char * buf = (char*) malloc(ret);
	//char * bufx = (char*) malloc(ret);
	while (1) {
		int data_read = esp_http_client_read(client, os_list_data, BUFFSIZE);
		if (data_read < 0) {
			ESP_LOGE(TAG, "Error: SSL data read error");
			http_cleanup(client);
			break;
		} else if (data_read > 0){
			memcpy(buf+recive_count,os_list_data,data_read);
			recive_count=+data_read;
			printf("read data %s \n" ,os_list_data);
			//http_cleanup(client);
			//return 0;

		}else if (data_read == 0) {
            ESP_LOGI(TAG, "Connection closed");
            http_cleanup(client);
            break;
        }


	}

	printf("cJSON_Parse \n");

	//sprintf(bufx,"%.*s", recive_count, (char*)buf);
	//printf("%s \n",  (char*)buf);
	cJSON *root = cJSON_Parse(buf);
	if (root != NULL)
	{
		printf("root != NULL \n");

		cJSON* r = cJSON_GetObjectItem(root, "device_type7");
		 if (r != NULL)
		 {
			 printf("r != NULL \n");



				 sprintf(os_path,"%s",cJSON_GetObjectItem(r,"os_uri")->valuestring);
//				 //version":1,"sub_version":6,"type":6
				 int version = cJSON_GetObjectItem(r,"version")->valueint;
				 printf("read  version %d \n",version);
				 int sub_version = cJSON_GetObjectItem(r,"sub_version")->valueint;
				 int type = cJSON_GetObjectItem(r,"type")->valueint;

				 if (version >= DEVICE_VERSION && sub_version > SUB_VERSION && type==6)
				 {
					 ESP_LOGI(TAG, "============  done there is  new version ===============");
					 cJSON_Delete(root);
					 free(buf);
					// free(bufx);
					 return 0;
				 }



		 }
		 cJSON_Delete(root);
	}

	free(buf);
	//free(bufx);

	ESP_LOGI(TAG, "============  done there is no new version ===============");
	return -1;
}


int download_os_file(char * os_path)
{

	esp_err_t err;

	esp_http_client_config_t config= {
		.url = os_path//"http://www.arbicom.co.il/FTP/ESP_os_update_list.json",
	//	.cert_pem = (char *)server_cert_pem_start,
	};



	esp_http_client_handle_t client = esp_http_client_init(&config);
	if (client == NULL) {
		ESP_LOGE(TAG, "download_os_file Failed to initialise HTTP connection");
		//task_fatal_error();
		return -1;
	}

	err = esp_http_client_open(client, 0);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "download_os_file Failed to open HTTP connection: %s", esp_err_to_name(err));
		esp_http_client_cleanup(client);
		//task_fatal_error();
		return -2;
	}

	int ret = esp_http_client_fetch_headers(client);
	ESP_LOGE(TAG, "fetch_headers %d",ret);

	if (ret > 0 )
	{
		//first stage of OTA
		starting_ota();
	}

	int recive_count=0;
	//char * buf = (char*) malloc(ret);
	while (1) {
		int data_read = esp_http_client_read(client, (char *)os_data, BUFFSIZE);
		if (data_read < 0) {
			ESP_LOGE(TAG, "Error: SSL data read error");
			http_cleanup(client);
			break;
		} else if (data_read > 0){
			ota_task(os_data ,data_read);
			recive_count=+data_read;
			printf("read data %d from %d \n" ,recive_count,ret);
			//http_cleanup(client);
			//return 0;

		}else if (data_read == 0) {
            ESP_LOGI(TAG, "Connection closed");
            http_cleanup(client);
            break;
        }


	}

	OTA_End();

//	free(buf);

	ESP_LOGI(TAG, "============  done  ===============");
	return 0;
}

static esp_err_t ESP_Reboot_handler(httpd_req_t *req)
{

	//reboot esp
	esp_restart();
//httpd_resp_send_chunk

   return ESP_OK;

}



static esp_err_t os_update_handler(httpd_req_t *req)
{
    char os_path[500];
    size_t buf_len;
    /* Get handle to embedded file upload script */
    	//download JSON file with OS update data
    int ret = download_os_update_list(os_path);
    if (ret != 0 )
    {

    	return -1;
    }
 	const char* resp_str = (const char*) os_path;
	httpd_resp_send(req, resp_str, strlen(resp_str));
	ESP_LOGI(TAG, "os_update_handler");

	download_os_file(os_path);

    	//if there is update download bin file and update OS




       /* Add file upload form and script which on execution sends a POST request to /upload */

//httpd_resp_send_chunk

   return ESP_OK;

}





static esp_err_t wifi_setting_handler(httpd_req_t *req)
{

	char*  buf;
	size_t buf_len;

	/* Get header value string length and allocate memory for length + 1,
	 * extra byte for null termination */
	buf_len = httpd_req_get_hdr_value_len(req, "ssid") + 1;
	if (buf_len > 1) {

		 buf = malloc(buf_len);
		/* Copy null terminated value string into buffer */
		if (httpd_req_get_hdr_value_str(req, "ssid", buf, buf_len) == ESP_OK) {
			ESP_LOGI(TAG, "Found header => ssid: %s", buf);
			memcpy( wifi_station_parm.ssid,buf, buf_len);

		}
		free(buf);
	}
	buf_len = httpd_req_get_hdr_value_len(req, "password") + 1;
	if (buf_len > 1) {
		 buf = malloc(buf_len);
		/* Copy null terminated value string into buffer */
		if (httpd_req_get_hdr_value_str(req, "password", buf, buf_len) == ESP_OK) {
			ESP_LOGI(TAG, "Found header => password: %s", buf);
			memcpy( wifi_station_parm.password,buf, buf_len);
		}
		free(buf);
	}

    	//download JSON file with OS update data
	//save wifi in memory
	Save_WIFI_parmToMemory();
	//reconnect to A
	wifi_config_sta();

 	const char* resp_str = (const char*) req->user_ctx;
	httpd_resp_send(req, resp_str, strlen(resp_str));
	//ESP_LOGI(TAG, "wifi_sta_start_handler");



   return ESP_OK;

}




static esp_err_t wifi_sta_start_handler(httpd_req_t *req)
{

    size_t buf_len;
    /* Get handle to embedded file upload script */
    	//download JSON file with OS update data

 	const char* resp_str = (const char*) req->user_ctx;
	httpd_resp_send(req, resp_str, strlen(resp_str));
	ESP_LOGI(TAG, "wifi_sta_start_handler");

	//restart wifi sta
	wifi_config_sta();

//httpd_resp_send_chunk

   return ESP_OK;

}


static const httpd_uri_t index_uri = {
    .uri       = "/index.html",
    .method    = HTTP_GET,
    .handler   = index_get_handler,
    .user_ctx  = NULL
};

static const httpd_uri_t post_os_update = {
    .uri       = "/os_update",
    .method    = HTTP_POST,
    .handler   = os_update_handler,
    .user_ctx  = "update now"
};

static const httpd_uri_t postESP_Reboot = {
    .uri       = "/ESP_Reboot",
    .method    = HTTP_POST,
    .handler   = ESP_Reboot_handler,
    .user_ctx  = "update now"
};




static const httpd_uri_t post_wifi_sta_start = {
    .uri       = "/wifi_sta_start",
    .method    = HTTP_POST,
    .handler   = wifi_sta_start_handler,
    .user_ctx  = "post_wifi_sta_start"
};




static const httpd_uri_t wifi_setting = {
    .uri       = "/wifi_setting",
    .method    = HTTP_POST,
    .handler   = wifi_setting_handler,
    .user_ctx  = "wifi_setting"
};



static const httpd_uri_t echo = {
    .uri       = "/ajax",
    .method    = HTTP_POST,
    .handler   = echo_post_handler,
    .user_ctx  = "Hello"
};


static const httpd_uri_t ajaxfirst = {
    .uri       = "/param_ajax",
    .method    = HTTP_POST,
    .handler   = param_ajax_handler,
    .user_ctx  = "ajaxfirst Hello"
};




void start_webserver(void)
{

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        //httpd_register_uri_handler(server, &hello);
        httpd_register_uri_handler(server, &echo);
        httpd_register_uri_handler(server, &index_uri);
        httpd_register_uri_handler(server, &ajaxfirst);
        httpd_register_uri_handler(server, &post_os_update);
        httpd_register_uri_handler(server, &post_wifi_sta_start);
        httpd_register_uri_handler(server, &postESP_Reboot);
        //httpd_register_uri_handler(server, &up_down_but);
        httpd_register_uri_handler(server, &wifi_setting);

        return;
    }

    ESP_LOGI(TAG, "start_webserver Error starting server!");

}

void stop_webserver()
{
    // Stop the httpd server
    httpd_stop(server);
}

void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
   // httpd_handle_t* server = (httpd_handle_t*) arg;
	 ESP_LOGI(TAG, " =========== Stopping webserver");
    if (server) {
        ESP_LOGI(TAG, "Stopping webserver");
        stop_webserver();
        server = NULL;
    }
}

void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    //httpd_handle_t* server = (httpd_handle_t*) arg;
	ESP_LOGI(TAG, "========= Starting webserver");
    start_webserver();

}


void webserver_init(void)
{


    //ESP_ERROR_CHECK(nvs_flash_init());
    tcpip_adapter_init();
   ESP_ERROR_CHECK(esp_event_loop_create_default());



    /* Register event handlers to stop the server when Wi-Fi or Ethernet is disconnected,
     * and re-start it upon connection.
     */

    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));


    /* Start the server for the first time */
    //server = start_webserver();
}
