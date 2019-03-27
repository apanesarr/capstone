
#include <ESP8266WiFi.h>
#include <math.h>
#include "WebSocketClient.h"
#include "ArduinoJson.h"

#define MAX_WIFI_INIT_RETRY 100
#define WIFI_RETRY_DELAY 500


class Comms {
	public:
		void connectSocket();
		int update();
		bool ready;
		StaticJsonDocument<512> message;
		StaticJsonDocument<512> messageReady;
		int initWifi();
		int handleData(String data);
		int isConnected();
		int send (String data);
		void setup();
		void transmit();
		void transmitReady();
};