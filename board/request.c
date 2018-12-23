#include <stdlib.h>
#include <cJson.h>
#pragma execution_character_set("utf-8")

int websocket_event_handler(const char* msg){

	char *message = msg;
	char *info[3];
	int i;

	info[0] = strtok (message,"/");
	info[1] = strtok (NULL,"/");
	info[2]= strtok (NULL,"/");

	char *weather = info[0];
	char temperature[100];
	char precipitation[100];
	printf("weather : %s, temperature:%s, precipitation:%s\n",weather,temperature,precipitation);

	if(!strcmp(weather,"흐림")){
		weather = "overcast";
		printf("weather : %s\n",weather);
	}else if(!strcmp(weather,"구름많음")){
		weather = "cloudy";
		printf("weather : %s\n",weather);
	}else if(!strcmp(weather,"맑음")){
		weather = "sunny";
		printf("weather : %s\n",weather);
	}else if(!strcmp(weather,"구름조금")){
		weather = "little cloudy";
		printf("weather : %s\n",weather);
	}else if(!strcmp(weather,"흐리고 가끔 비")){
		weather = "overcast and little rain";
		printf("weather : %s\n",weather);
	}else if(!strcmp(weather,"흐리고 한때 비")){
		weather = "overcast and temporary rain";
		printf("weather : %s\n",weather);
	}else if(!strcmp(weather,"흐리고 비")){
		weather = "overcast and rain";
		printf("weather : %s\n",weather);
	}
	sprintf(temperature, "%s%s", info[1], "'c");
	sprintf(precipitation, "%s%s%s", "rain : ",info[2], "%");


	printf("temperature : %s\n", temperature);
	printf("precipitation : %s\n", precipitation);
	/*
	cJSON* json_data = NULL;
	cJSON* morning_data = NULL;
	cJSON* weather_data = NULL;
	cJSON* temperature_data = NULL;
	cJSON* precipitation_data = NULL;
	int i  =0;
	printf("msg : %s\n",msg);
	json_data = cJSON_Parse((const char*) msg);
	printf("json : %s\n",json_data);
	if(json_data == NULL){
		return -1;
	}

	morning_data = cJSON_GetObjectItem(json_data, "morning");
	if(morning_data == NULL){
		return -1;
	}

	weather_data = cJSON_GetObjectItem(morning_data, "weather");
	temperature_data = cJSON_GetObjectItem(morning_data, "temperature");
	precipitation_data = cJSON_GetObjectItem(morning_data, "precipitation");

	printf("weather : %s, temperature:%s, precipitation:%s\n",(char *)weather_data,temperature_data,(char *)precipitation_data);
	if((char*)weather_data == "흐림"){
		weather = "dark";
		printf("weather : %s\n",weather);
	}else if((char*)weather_data =="구름많음"){
		weather = "cloudy";
		printf("weather : %s\n",weather);
	}*/
	sh1106_write_string(4 ,1, "weather");
	sh1106_write_string(4 ,2, weather);
	sh1106_write_string(4 ,3, temperature);

	sh1106_write_string(4 ,4, precipitation);

	return 0;
}
