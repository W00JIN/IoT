/****************************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/

/**
 * @file websocket-api.c
 */

#include <stdio.h>
#include <string.h>
#include <shell/tash.h>

#include <artik_module.h>
#include <artik_websocket.h>

#include <apps/netutils/cJSON.h>

#include "command.h"
#ifdef CONFIG_EXAMPLES_ARTIK_WEBSOCKET
#include "wifi-auto.h"
#endif


static artik_websocket_handle g_handle;
static artik_websocket_module *g_websocket;
static artik_websocket_config g_config;

#include "wifi.h"
#include <tinyara/gpio.h>
#include <apps/netutils/mqtt_api.h>
#include <apps/netutils/dhcpc.h>

#include <tinyara/analog/adc.h>
#include <tinyara/analog/ioctl.h>

#include <apps/shell/tash.h>

// for NTP
#include <apps/netutils/ntpclient.h>

#define DEFAULT_CLIENT_ID "123456789"
#define SERVER_ADDR "api.artik.cloud"
//#define SERVER_ADDR "52.86.204.150"
#define SERVER_PORT 8883
//#define SERVER_PORT 1883 // non-secure mode, Not supported in ARTIK Cloud
#define RED "RED"
#define BLUE "BLUE"
#define GREEN "GREEN"
#define RED_LED 45 // on-board LED
#define GREEN_LED 60
#define BLUE_LED 49 // on-board LED
#define RED_ON_BOARD_LED 45
#define NET_DEVNAME "wl1"

char device_id[] = 		"6aea6602d9ed45c4bd123b3a7a9bb725";
char device_token[] = 	"cf9c9f2ccfee457289c0a4186747f465";

char *strTopicMsg;
char *strTopicAct;
int open_count = 0;
int close_count = 0;
int is_opened = 0;
int is_in=0;
static const char mqtt_ca_cert_str[] = \
		"-----BEGIN CERTIFICATE-----\r\n"
				"MIIGrTCCBZWgAwIBAgIQASAP9e8Tbenonqd/EQFJaDANBgkqhkiG9w0BAQsFADBN\r\n"
				"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMScwJQYDVQQDEx5E\r\n"
				"aWdpQ2VydCBTSEEyIFNlY3VyZSBTZXJ2ZXIgQ0EwHhcNMTgwMzA4MDAwMDAwWhcN\r\n"
				"MjAwNDA1MTIwMDAwWjBzMQswCQYDVQQGEwJVUzETMBEGA1UECBMKQ2FsaWZvcm5p\r\n"
				"YTERMA8GA1UEBxMIU2FuIEpvc2UxJDAiBgNVBAoTG1NhbXN1bmcgU2VtaWNvbmR1\r\n"
				"Y3RvciwgSW5jLjEWMBQGA1UEAwwNKi5hcnRpay5jbG91ZDCCASIwDQYJKoZIhvcN\r\n"
				"AQEBBQADggEPADCCAQoCggEBANghNaTXWDfYV/JWgBnX4hmhcClPSO0onx5B2url\r\n"
				"YzpvTc3MBaQ+08YBpAKvTqZvPqrJUIM45Q91M301I5e2kz0DMq2zQZOGB0B83V/O\r\n"
				"O4vwETq4PCjAPhMinF4dN6HeJCuqo1CLh8evhfkFiJvpEfQWTxdjzPJ0Zdj/2U8E\r\n"
				"8Ht7zV5pWiDtuejtIDHB5H6fCx4xeQy/E+5l4V6R3BnRKpZsJtlhTh0RFqWhw5DJ\r\n"
				"/WWpGP//1VTZSHyW9SABsPd+jP1YgDraRD4b4lZBU6c8nC5qT3dhdiYoG6xUgTb3\r\n"
				"kfgUhhlOFpe3sBtR32OS8RuFrFeQDGaa3r6pfSy06Kph/eECAwEAAaOCA2EwggNd\r\n"
				"MB8GA1UdIwQYMBaAFA+AYRyCMWHVLyjnjUY4tCzhxtniMB0GA1UdDgQWBBSNBf6r\r\n"
				"7S/j0oV3A0XmEflXErutQDAlBgNVHREEHjAcgg0qLmFydGlrLmNsb3VkggthcnRp\r\n"
				"ay5jbG91ZDAOBgNVHQ8BAf8EBAMCBaAwHQYDVR0lBBYwFAYIKwYBBQUHAwEGCCsG\r\n"
				"AQUFBwMCMGsGA1UdHwRkMGIwL6AtoCuGKWh0dHA6Ly9jcmwzLmRpZ2ljZXJ0LmNv\r\n"
				"bS9zc2NhLXNoYTItZzYuY3JsMC+gLaArhilodHRwOi8vY3JsNC5kaWdpY2VydC5j\r\n"
				"b20vc3NjYS1zaGEyLWc2LmNybDBMBgNVHSAERTBDMDcGCWCGSAGG/WwBATAqMCgG\r\n"
				"CCsGAQUFBwIBFhxodHRwczovL3d3dy5kaWdpY2VydC5jb20vQ1BTMAgGBmeBDAEC\r\n"
				"AjB8BggrBgEFBQcBAQRwMG4wJAYIKwYBBQUHMAGGGGh0dHA6Ly9vY3NwLmRpZ2lj\r\n"
				"ZXJ0LmNvbTBGBggrBgEFBQcwAoY6aHR0cDovL2NhY2VydHMuZGlnaWNlcnQuY29t\r\n"
				"L0RpZ2lDZXJ0U0hBMlNlY3VyZVNlcnZlckNBLmNydDAJBgNVHRMEAjAAMIIBfwYK\r\n"
				"KwYBBAHWeQIEAgSCAW8EggFrAWkAdgCkuQmQtBhYFIe7E6LMZ3AKPDWYBPkb37jj\r\n"
				"d80OyA3cEAAAAWIHFb1dAAAEAwBHMEUCIQCQ0UjVVJSQDRB3oxzI5aD1Hs5GhbXj\r\n"
				"I6Cqt3/tkXT1WQIgNVWRgbJ72Ik9gp5QoNxhCZ+h//or0uL7PHnv3cP5L9UAdgBv\r\n"
				"U3asMfAxGdiZAKRRFf93FRwR2QLBACkGjbIImjfZEwAAAWIHFb73AAAEAwBHMEUC\r\n"
				"IQDxCxJCsZjuqbQvuwipgdUf1l6qXdiekM5zn33i1+KYxgIgKDMJEuKHzhkweT2S\r\n"
				"Y4dWBuzSdOAzZfoDrIGdsFvkxi0AdwC72d+8H4pxtZOUI5eqkntHOFeVCqtS6BqQ\r\n"
				"lmQ2jh7RhQAAAWIHFb1YAAAEAwBIMEYCIQCNDYdxWmqUGGwNzXlJ1/NXxzwqPYIB\r\n"
				"eSJDuR1xfWtSsQIhAJsygf2rqPS+O7qQAzggCQ2V/3JDRUhuxNDPqwooo47uMA0G\r\n"
				"CSqGSIb3DQEBCwUAA4IBAQBvRGWibvHFrRUWsArJ9lmS5MMZFbXXQPXbflgv3nSG\r\n"
				"ShmhBC3o+k97J0Wgp/wH7uDf01RrRMAVNm458g1Mr4AMAXq3zzxNNTwjGYw/USuG\r\n"
				"UprrKqc9onugtAUX8DGvlZr8SWO3FhPlyamWQ69jutx/X4nfHyZr41bX9WQ/ay0F\r\n"
				"GQJ1tRTrX1eUPO+ucXeG8vTbt09bRNnoY+i97dzrwHakXySfHohNsIbwmrsS4SQv\r\n"
				"7eG9g5+5vsc2B9ugGcELIYKrzDWNPshir37KSpcwLUCmDJkTQp8+KhJUKgbTALTa\r\n"
				"nxuDyNwZIwW66vv1t0Zi4vKU8hfUsAN2N3wcsb6pY/RA\r\n"
				"-----END CERTIFICATE-----\r\n"
				"-----BEGIN CERTIFICATE-----\r\n"
				"MIIElDCCA3ygAwIBAgIQAf2j627KdciIQ4tyS8+8kTANBgkqhkiG9w0BAQsFADBh\r\n"
				"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\r\n"
				"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\r\n"
				"QTAeFw0xMzAzMDgxMjAwMDBaFw0yMzAzMDgxMjAwMDBaME0xCzAJBgNVBAYTAlVT\r\n"
				"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxJzAlBgNVBAMTHkRpZ2lDZXJ0IFNIQTIg\r\n"
				"U2VjdXJlIFNlcnZlciBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\r\n"
				"ANyuWJBNwcQwFZA1W248ghX1LFy949v/cUP6ZCWA1O4Yok3wZtAKc24RmDYXZK83\r\n"
				"nf36QYSvx6+M/hpzTc8zl5CilodTgyu5pnVILR1WN3vaMTIa16yrBvSqXUu3R0bd\r\n"
				"KpPDkC55gIDvEwRqFDu1m5K+wgdlTvza/P96rtxcflUxDOg5B6TXvi/TC2rSsd9f\r\n"
				"/ld0Uzs1gN2ujkSYs58O09rg1/RrKatEp0tYhG2SS4HD2nOLEpdIkARFdRrdNzGX\r\n"
				"kujNVA075ME/OV4uuPNcfhCOhkEAjUVmR7ChZc6gqikJTvOX6+guqw9ypzAO+sf0\r\n"
				"/RR3w6RbKFfCs/mC/bdFWJsCAwEAAaOCAVowggFWMBIGA1UdEwEB/wQIMAYBAf8C\r\n"
				"AQAwDgYDVR0PAQH/BAQDAgGGMDQGCCsGAQUFBwEBBCgwJjAkBggrBgEFBQcwAYYY\r\n"
				"aHR0cDovL29jc3AuZGlnaWNlcnQuY29tMHsGA1UdHwR0MHIwN6A1oDOGMWh0dHA6\r\n"
				"Ly9jcmwzLmRpZ2ljZXJ0LmNvbS9EaWdpQ2VydEdsb2JhbFJvb3RDQS5jcmwwN6A1\r\n"
				"oDOGMWh0dHA6Ly9jcmw0LmRpZ2ljZXJ0LmNvbS9EaWdpQ2VydEdsb2JhbFJvb3RD\r\n"
				"QS5jcmwwPQYDVR0gBDYwNDAyBgRVHSAAMCowKAYIKwYBBQUHAgEWHGh0dHBzOi8v\r\n"
				"d3d3LmRpZ2ljZXJ0LmNvbS9DUFMwHQYDVR0OBBYEFA+AYRyCMWHVLyjnjUY4tCzh\r\n"
				"xtniMB8GA1UdIwQYMBaAFAPeUDVW0Uy7ZvCj4hsbw5eyPdFVMA0GCSqGSIb3DQEB\r\n"
				"CwUAA4IBAQAjPt9L0jFCpbZ+QlwaRMxp0Wi0XUvgBCFsS+JtzLHgl4+mUwnNqipl\r\n"
				"5TlPHoOlblyYoiQm5vuh7ZPHLgLGTUq/sELfeNqzqPlt/yGFUzZgTHbO7Djc1lGA\r\n"
				"8MXW5dRNJ2Srm8c+cftIl7gzbckTB+6WohsYFfZcTEDts8Ls/3HB40f/1LkAtDdC\r\n"
				"2iDJ6m6K7hQGrn2iWZiIqBtvLfTyyRRfJs8sjX7tN8Cp1Tm5gr8ZDOo0rwAhaPit\r\n"
				"c+LJMto4JQtV05od8GiG7S5BNO98pVAdvzr508EIDObtHopYJeS4d60tbvVS3bR0\r\n"
				"j6tJLp07kzQoH3jOlOrHvdPJbRzeXDLz\r\n"
				"-----END CERTIFICATE-----\r\n";

// mqtt client handle
mqtt_client_t* pClientHandle = NULL;

// mqtt client parameters
mqtt_client_config_t clientConfig;

//typedef struct _mqtt_tls_param_t {
//	const unsigned char *ca_cert;	/* CA certificate, common between client and MQTT Broker */
//	const unsigned char *cert;	/* Client certificate */
//	const unsigned char *key;	/* Client private key */
//	int ca_cert_len;			/* the length of CA certificate  */
//	int cert_len;				/* the length of Client certificate */
//	int key_len;				/* the length of key */
//} mqtt_tls_param_t;

mqtt_tls_param_t clientTls;

//int blinkerValue = 0;
int currentLED = 0;

struct ntpc_server_conn_s g_server_conn[2];

static void websocket_rx_callback(void *user_data, void *result)
{
	if (result) {
		fprintf(stderr, "RX: %s\n", (char *)result);
		//add
		if(websocket_event_handler( (char *)result)<0){
			;
		}else{
			free(result);
			//close
			//iot_websocket_disconnect();
		}
	}
}

static void websocket_connection_callback(void *user_data, void *result)
{
	artik_websocket_connection_state state = (artik_websocket_connection_state)result;

	if (state == ARTIK_WEBSOCKET_CLOSED)
		fprintf(stderr, "Websocket connection has been closed\n");
	else if (state == ARTIK_WEBSOCKET_CONNECTED)
		fprintf(stderr, "Websocket is connected\n");
}

int iot_websocket_connect()
{
	artik_error ret = S_OK;

	g_websocket = (artik_websocket_module *)artik_request_api_module("websocket");
	if (!g_websocket) {
		fprintf(stderr, "Websocket module is not available\n");
		return -1;
	}

	memset(&g_config, 0, sizeof(artik_websocket_config));
	g_config.uri = "ws://ec2-3-17-51-29.us-east-2.compute.amazonaws.com/";

	ret = g_websocket->websocket_request(&g_handle, &g_config);
	if (ret != S_OK) {
		fprintf(stderr, "Failed to request websocket (ret=%d)\n", ret);
		return -1;
	}

	ret = g_websocket->websocket_open_stream(g_handle);
	if (ret != S_OK) {
		fprintf(stderr, "Failed to open websocket (ret=%d)\n", ret);
		return -1;
	}

	ret = g_websocket->websocket_set_receive_callback(g_handle, websocket_rx_callback, NULL);
	if (ret != S_OK) {
		fprintf(stderr, "Failed to register rx callback (ret=%d)\n", ret);
		return -1;
	}

	ret = g_websocket->websocket_set_connection_callback(g_handle, websocket_connection_callback, NULL);
	if (ret != S_OK) {
		fprintf(stderr, "Failed to register connection callback (ret=%d)\n", ret);
		return -1;
	}

	return 0;
}

int iot_websocket_disconnect()
{
	if (!g_websocket || !g_handle) {
		fprintf(stderr, "Websocket is not connected\n");
		return -1;
	}

	g_websocket->websocket_close_stream(g_handle);

	artik_release_api_module(g_websocket);
	g_websocket = NULL;
	g_handle = NULL;

	return 0;
}

int iot_websocket_send(){
	if (!g_websocket || !g_handle) {
		fprintf(stderr, "Websocket is not connected\n");
		return -1;
	}

	g_websocket->websocket_write_stream(g_handle, "ws://ec2-3-17-51-29.us-east-2.compute.amazonaws.com/");

	return 0;
}


const unsigned char *get_ca_cert(void) {
	return (const unsigned char*)mqtt_ca_cert_str;
}

// mqtt client on connect callback
void onConnect(void* client, int result) {
    printf("mqtt client connected to the server\n");
}

// mqtt client on disconnect callback
void onDisconnect(void* client, int result) {
    printf("mqtt client disconnected from the server\n");
}

// mqtt client on publish callback
void onPublish(void* client, int result) {
   printf("mqtt client Published message\n");
}

// Write the value of given gpio port.
void gpio_write(int port, int value) {
    char str[4];
    static char devpath[16];
    snprintf(devpath, 16, "/dev/gpio%d", port);
    int fd = open(devpath, O_RDWR);

    ioctl(fd, GPIOIOC_SET_DIRECTION, GPIO_DIRECTION_OUT);
    write(fd, str, snprintf(str, 4, "%d", value != 0) + 1);

    close(fd);
}


// mqtt client on message callback
// header: <apps/netutils/cJSON.h>
void onMessage(void *client, mqtt_msg_t *msg) {
	int i;
	cJSON *jsonMsg = NULL;
	char *strActName = NULL;
	char *payload = strdup(msg->payload);

	printf("Received message\n");
    printf("Topic: %s\n", msg->topic);
    printf("Message: %s\n", payload);

    jsonMsg = cJSON_Parse((const char*)payload);
    cJSON *data = cJSON_GetObjectItem(jsonMsg, "actions");

    if (data == NULL) {
    	printf("data is null\n");
    	return;
    }

     cJSON *action = cJSON_GetArrayItem(data, 0);
     cJSON *actName = cJSON_GetObjectItem(action, "name");
     cJSON *actParameter = cJSON_GetObjectItem(action, "parameters");

     strActName = cJSON_Print(actName);

     cJSON_Delete(jsonMsg);
     free(strActName);
     free(payload);

     //ENTRANCE
     if (strncmp(strActName, "\"lcd_on\"", 8) == 0) {
        printf("[lcd on]\n");
        if(is_opened ==0){
        	if(iot_websocket_connect()<0) printf("Error connecting \n");
        	is_in=1;
        }
     }else if (strncmp(strActName, "\"lcd_off\"", 9) == 0) {
         printf("[lcd off]\n");
        	 sh1106_clear();
        	 is_in=0;
      }
     else {
        printf("Unrecognized action.\n");
     }

}

// Utility function to configure mqtt client
void initializeConfigUtil(void) {
	uint8_t macId[IFHWADDRLEN];
	    int result = netlib_getmacaddr("wl1", macId);
	    if (result < 0) {
	        printf("Get MAC Address failed. Assigning \
	                Client ID as 123456789");
	        clientConfig.client_id =
	                DEFAULT_CLIENT_ID; // MAC id Artik 053
	    } else {
	    printf("MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
	            ((uint8_t *) macId)[0],
	            ((uint8_t *) macId)[1], ((uint8_t *) macId)[2],
	            ((uint8_t *) macId)[3], ((uint8_t *) macId)[4],
	            ((uint8_t *) macId)[5]);
	    char buf[12];
	    sprintf(buf, "%02x%02x%02x%02x%02x%02x",
	            ((uint8_t *) macId)[0],
	            ((uint8_t *) macId)[1], ((uint8_t *) macId)[2],
	            ((uint8_t *) macId)[3], ((uint8_t *) macId)[4],
	            ((uint8_t *) macId)[5]);
	    clientConfig.client_id = buf; // MAC id Artik 053
	    printf("Registering mqtt client with id = %s\n", buf);
	    }

	    clientConfig.user_name = device_id;
	    clientConfig.password = device_token;
	    clientConfig.debug = true;
	    clientConfig.on_connect = (void*) onConnect;
	    clientConfig.on_disconnect = (void*) onDisconnect;
	    clientConfig.on_message = (void*) onMessage;
	    clientConfig.on_publish = (void*) onPublish;

	    clientConfig.protocol_version = MQTT_PROTOCOL_VERSION_311;
	    clientConfig.clean_session = true;

	    clientTls.ca_cert = get_ca_cert();
	    clientTls.ca_cert_len = sizeof(mqtt_ca_cert_str);
	    clientTls.cert = NULL;
	    clientTls.cert_len = 0;
	    clientTls.key = NULL;
	    clientTls.key_len = 0;

	    clientConfig.tls = &clientTls;
}

static void ntp_link_error(void)
{
	printf("ntp_link_error() callback is called.\n");
}




int main(int argc, FAR char *argv[]){

	StartWifiConnection();
	   bool wifiConnected = false;
	   gpio_write(RED_ON_BOARD_LED, 1); // Turn on on board Red LED to indicate no WiFi connection is established

	   strTopicMsg = (char*) malloc(sizeof(char) * 256);
	   strTopicAct = (char*) malloc(sizeof(char) * 256);
	   sprintf(strTopicMsg, "/v1.1/messages/%s", device_id);
	   sprintf(strTopicAct, "/v1.1/actions/%s", device_id);

	   memset(&clientConfig, 0, sizeof(clientConfig));
	   memset(&clientTls, 0, sizeof(clientTls));

	// for NTP Client
	   memset(&g_server_conn, 0, sizeof(g_server_conn));
	   g_server_conn[0].hostname = "0.asia.pool.ntp.org";
	   g_server_conn[0].port = 123;
	   g_server_conn[1].hostname = "1.asia.pool.ntp.org";
	   g_server_conn[1].port = 123;

	#ifdef CONFIG_CTRL_IFACE_FIFO
	   int ret;

	   while (!wifiConnected) {
	      ret = mkfifo(CONFIG_WPA_CTRL_FIFO_DEV_REQ,
	      CONFIG_WPA_CTRL_FIFO_MK_MODE);
	      if (ret != 0 && ret != -EEXIST) {
	         printf("mkfifo error for %s: %s", CONFIG_WPA_CTRL_FIFO_DEV_REQ,
	               strerror(errno));
	      }
	      ret = mkfifo(CONFIG_WPA_CTRL_FIFO_DEV_CFM,
	      CONFIG_WPA_CTRL_FIFO_MK_MODE);
	      if (ret != 0 && ret != -EEXIST) {
	         printf("mkfifo error for %s: %s", CONFIG_WPA_CTRL_FIFO_DEV_CFM,
	               strerror(errno));
	      }

	      ret = mkfifo(CONFIG_WPA_MONITOR_FIFO_DEV, CONFIG_WPA_CTRL_FIFO_MK_MODE);
	      if (ret != 0 && ret != -EEXIST) {
	         printf("mkfifo error for %s: %s", CONFIG_WPA_MONITOR_FIFO_DEV,
	               strerror(errno));
	      }
	#endif

	      if (start_wifi_interface() == SLSI_STATUS_ERROR) {
	         printf("Connect Wi-Fi failed. Try Again.\n");
	      } else {
	         wifiConnected = true;
	         gpio_write(RED_ON_BOARD_LED, 0); // Turn off Red LED to indicate WiFi connection is established
	      }
	   }

	   printf("Connect to Wi-Fi success\n");

	   bool mqttConnected = false;
	   bool ipObtained = false;
	   printf("Get IP address\n");

	   struct dhcpc_state state;
	   void *dhcp_handle;

	   while (!ipObtained) {
	      dhcp_handle = dhcpc_open(NET_DEVNAME);
	      ret = dhcpc_request(dhcp_handle, &state);
	      dhcpc_close(dhcp_handle);

	      if (ret != OK) {
	         printf("Failed to get IP address\n");
	         printf("Try again\n");
	         sleep(1);
	      } else {
	         ipObtained = true;
	      }
	   }
	   netlib_set_ipv4addr(NET_DEVNAME, &state.ipaddr);
	   netlib_set_ipv4netmask(NET_DEVNAME, &state.netmask);
	   netlib_set_dripv4addr(NET_DEVNAME, &state.default_router);

	   printf("IP address  %s\n", inet_ntoa(state.ipaddr));

	   up_mdelay(2000);

	   int ret_ntp = ntpc_start(g_server_conn, 2, 1000, ntp_link_error);
	   printf("ret: %d\n", ret_ntp);

	// Connect to the WiFi network for Internet connectivity
	   printf("mqtt client tutorial\n");

	// Initialize mqtt client
	   initializeConfigUtil();


	   bool mqttSubscribe = false;

	// Subscribe to topic of interest

	    // Read analog signal using ADC
	    int fd;
	    struct adc_msg_s sample;
	    size_t readsize;
	    ssize_t nbytes;
	    int publishedVal = -1;


		if(sh1106_init(0, 51, 52)) {// port, A0, reset
			 printf("display init failed\n");
			return -1;
		}


	    fd = open("/dev/adc0", O_RDONLY);
	    if (fd < 0) {
	    	 printf("%s: open failed: %d\n", __func__, errno);
	        return 0;
	    }

	    while(1) {
	    	ret = ioctl(fd, ANIOC_TRIGGER, 0);
	    	if (ret < 0) {
	    	    printf("%s: ioctl failed: %d\n", __func__, errno);
	    	    close(fd);
	    	    return 0;
	    	}

	    	readsize = sizeof(struct adc_msg_s);

	    	// adc_msg_s structure returns the data structure containing analog readings from ADC pins
	    	nbytes = read(fd, &sample, readsize);
	    	if (nbytes < 0) {
	    	    if (errno != EINTR) {
	    	        printf("%s: read failed: %d\n", __func__, errno);
	    	        close(fd);
	    	        return 0;
	    	    }
	    	} else if (nbytes == 0) {
	    	    printf("%s: No data read, Ignoring\n");
	    	} else { // nbytes > 0
	    	    int nsamples = nbytes / sizeof(struct adc_msg_s);
	    	    if (nsamples * sizeof(struct adc_msg_s) != nbytes) {
	    	        printf("%s: read size=%ld is not a multiple of sample size=%d, Ignoring\n", __func__, (long)nbytes, sizeof(struct adc_msg_s));
	    	    } else {
	    	        // Filter the data obtained from only ADC pin connected to the sensor A0 - 0, A1 - 1, A2 - 2, A3 - 3
	    	        if ((sample.am_channel == 0) && ((sample.am_data > (publishedVal + 100)) || (sample.am_data < (publishedVal - 100)))) // A0 ADC
	    	        {
	    	        	gpio_write(49,1);
	    	            char buf[40];
	    	            sprintf(buf, "{\"distance\" : %d}", sample.am_data);

	    	            if(sample.am_data >1300){
	    	            	open_count++;
	    	            	if(open_count>5 && is_opened == 0){
	    	            		if(iot_websocket_connect()<0) printf("Error connecting \n");
	    	            		else printf("closet opened\n");
	    	            		is_opened = 1;
	    	            		open_count= 0;
	    	            		close_count= 0;
	    	            	}

	    	            }else if(sample.am_data<1000){
	    	            	close_count++;
	    	            	if(close_count>25 && is_opened == 1){
								 sh1106_clear();
								printf("closet closed\n");
								is_opened = 0;
								open_count= 0;
								close_count= 0;
	    	            	}
	    	            }

	    	        }
	    	    }
	    	}

	    }

	    //close(fd);

	return 0;
}


