#include "Arduino.h"
#include "Comms.h"

const char* ssid = "A3";
const char* password = "12345678";
char path [] = "/";
char host [] = "192.168.43.91";
int port = 8095;

WebSocketClient SocketClient;
WiFiClient client;


int Comms::initWifi(){
	int retries = 0;
    Serial.println("Connecting to WiFi AP..........");
    WiFi.mode(WIFI_STA);
    Serial.print(WIFI_STA);
    WiFi.begin(ssid, password);
    while ((WiFi.status() != WL_CONNECTED) && (retries < MAX_WIFI_INIT_RETRY)) {
        retries++;
        delay(WIFI_RETRY_DELAY);
        Serial.print("#");
    }
    return WiFi.status();
}

void Comms::connectSocket(){
	while (! client.connect(host, port)) {
      Serial.println("Not Connected");
      delay(100);
    }
    SocketClient.path = path;
    SocketClient.host = host;
    while ( ! SocketClient.handshake(client)) {
      Serial.println("HandShake Failed");
      delay(100);
    }
}

int Comms::handleData(String data){
	DeserializationError error = deserializeJson(message, data);
  	if (error) {
		return 0;
	}
	return 1;
}

int Comms::isConnected(){
	if (client.connected()) {
		return 1;
  	}
  	else {
  		return 0;
  	}
}

void Comms::setup(){
	if (initWifi() == WL_CONNECTED) {
		Serial.println(WiFi.localIP());
	}
	connectSocket();
	StaticJsonDocument<96> doc;
	doc["MessageType"] = "I";
	doc["RecipientId"] = 0;
	String output;
	serializeJson(doc,output);
	Serial.println(output);
	send(output);
	ready = true;
}

int Comms::send(String data){
	if(isConnected()){
		Serial.print("sending.......");
		Serial.println(data);
		SocketClient.sendData(data);
		return 1;
	}
	else{
		Serial.println("Failed to print");
		return 0;
	}
}

int Comms::update (){
	String data;
	if (isConnected()){
		SocketClient.getData(data);
		if (data.length() > 0) {
      		Serial.print("Recieved........");
      		Serial.println(data);
      		return handleData(data);
    	}
    	else{
    		return 0;
    	}
	}
	else {
		connectSocket();
		return 0;
	}
}

void Comms::transmit(){
	String output;
    serializeJson(message, output);
    send(output);
}


void Comms::transmitReady(){
	String output;
	serializeJson(messageReady, output);
	send(output);
}
