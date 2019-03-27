#include <ESP8266WiFi.h>
#include "WebSocketClient.h"
#include <math.h>
#include "ArduinoJson.h"
#include "CarMotorCtrl.h"

#define MAX_WIFI_INIT_RETRY 50
#define WIFI_RETRY_DELAY 500

const char* ssid = "A3";
const char* password = "12345678";
char path [] = "/";
char host [] = "192.168.43.91";
int port = 8095;
bool connected = false;
bool sent = true;

WebSocketClient webSocketClient;
WiFiClient client;
pos currentPos;
MotorControl motor;
motorSettings_t settings;

int init_wifi() {
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



void setup(){
  Serial.begin(115200);
  if (init_wifi() == WL_CONNECTED) {
      Serial.println(WiFi.localIP());
  }
  socketConnect();
  StaticJsonDocument<50> doc;
  doc["MessageType"] = "I";
  doc["MessageId"] = 0;
  doc["RecipientId"] = 0;
  String output;
  serializeJson(doc,output);
  writeToMaster(output);
  motor.init();
}

void socketConnect (){
  while (! client.connect(host, port)) {
      Serial.println("Not Connected");
    }
    webSocketClient.path = path;
    webSocketClient.host = host;
    while ( ! webSocketClient.handshake(client)) {
      Serial.println("HandShake Failed");
    }

}

void loop() {


  String data;

  if (client.connected()) {

    webSocketClient.getData(data);
    if (data.length() > 0) {
      Serial.println(data);
      handleData(data);
    }

    }
  else {
    Serial.println("Client disconnected.");
    while (!client.connected()){
      socketConnect();
    }
  }
  motor.update();
  if (motor.ready && !(sent)){
  StaticJsonDocument<50> doc;
   doc["MessageType"] = "R";
   doc["MessageId"] = 0;
   doc["RecipientId"] = 0;
   doc["Data"]["X"] = currentPos.X;
   doc["Data"]["Y"] = currentPos.Y;
   doc["Data"]["Angle"] = currentPos.Angle;
   String output;
   serializeJson(doc,output);
   writeToMaster(output);
   sent = true;
  }
}

void handleData (String data){
  StaticJsonDocument<500> jsonBuffer;
  DeserializationError error = deserializeJson(jsonBuffer, data);
  if (error) {
      return;
    }
  if (jsonBuffer["MessageType"]=="I"){
    connected = true;
    jsonBuffer["MessageType"] = "R";
    jsonBuffer["Data"]["X"] = currentPos.X;
    jsonBuffer["Data"]["Y"] = currentPos.Y;
    jsonBuffer["Data"]["Angle"] = currentPos.Angle;
    String output;
    serializeJson(jsonBuffer, output);
    writeToMaster(output);
    Serial.println(output);

  }
    else if (jsonBuffer["MessageType"] == "M"){

      String state = jsonBuffer["Data"]["State"].as<String>();

      if(state=="STOP"){
          settings.state = STOP;
      }

      else if (state=="FORWARD" ){
          settings.state = FORWARD;
          settings.target = jsonBuffer["Data"]["Distance"].as<float>();
          motor.setMotor(settings);
          sent = false;
          calcXY(settings.target);
      }

      else if (state=="REVERSE"){
        settings.state = REVERSE;
      settings.target = jsonBuffer["Data"]["Distance"].as<float>();
      // motor.setMotor(settings);
          calcXY(settings.target);
      }

      else if (state=="LEFT"){
          settings.state = LEFT;
          settings.target = jsonBuffer["Data"]["Angle"].as<float>();
          calcAngle(settings.target);
      }

      else if (state=="RIGHT"){
          settings.state = RIGHT;
          settings.target = jsonBuffer["Data"]["Angle"].as<float>();
          calcAngle(-1 * settings.target);
      }

      else {
          return;
      }
      jsonBuffer["MessageType"] = "R";
      jsonBuffer["Data"]["X"] = currentPos.X;
      jsonBuffer["Data"]["Y"] = currentPos.Y;
      jsonBuffer["Data"]["Angle"] = currentPos.Angle;
      String output;
      serializeJson(jsonBuffer, output);
//      writeToMaster(output);
      Serial.println(output);
  }

  else if (jsonBuffer["MessageType"] == "R"){
      jsonBuffer["Data"]["X"] = currentPos.X;
      jsonBuffer["Data"]["Y"] = currentPos.Y;
      jsonBuffer["Data"]["Angle"] = currentPos.Angle;
      String output;
      serializeJson(jsonBuffer, output);
      writeToMaster(output);
    }

  else if (jsonBuffer["MessageType"] == "T"){
      jsonBuffer["Data"]["Temperature"] = 10.0;
      jsonBuffer["Data"]["Humidity"] = 80.0;
      String output;
      serializeJson(jsonBuffer, output);
      writeToMaster(output);
  }

  else {
    writeToMaster("Invaild Message");
  }

}


void calcXY(float distance){
  if(0<=currentPos.Angle || currentPos.Angle < 45 || 135<= currentPos.Angle
      || currentPos.Angle < 180 || 180<= currentPos.Angle
      || currentPos.Angle < 225 || 315<= currentPos.Angle
      || currentPos.Angle <360){
     currentPos.X += distance*cos( currentPos.Angle*PI/180 );
     currentPos.Y += distance*sin( currentPos.Angle*PI/180  );
     Serial.println(cos(currentPos.Angle));
     Serial.println(sin(currentPos.Angle));

  }
  else if (45<= currentPos.Angle || currentPos.Angle < 135
      || 225<= currentPos.Angle || currentPos.Angle < 315){
    currentPos.Y += distance*cos( currentPos.Angle *PI/180  );
     currentPos.X += distance*sin( currentPos.Angle *PI/180  );
     Serial.println(cos(currentPos.Angle));
     Serial.println(sin(currentPos.Angle));
  }
}

void calcAngle(float angle){
  if (angle > 360) angle -= 360;
  if (angle < 0)   angle += 360;

  currentPos.Angle = angle;
}

void writeToMaster(String msg){
  if (client.connected()) {
     webSocketClient.sendData(msg);
  }
  else {
    socketConnect();
  }
}
