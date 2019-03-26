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

WebSocketClient webSocketClient;
WiFiClient client;
pos currentPos;

int init_wifi() {
    int retries = 0;
    Serial.println("Connecting to WiFi AP..........");
    WiFi.mode(WIFI_STA);
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
}

void socketConnect (){
  if (client.connect(host, port)) {
      Serial.println("Connected");
    } 
    else {
      Serial.println("Connection failed.");
      while(1) {
          // Hang on failure
      }
    }
    webSocketClient.path = path;
    webSocketClient.host = host;
    if (webSocketClient.handshake(client)) {
      Serial.println("Handshake successful");
    } 
    else {
      Serial.println("Handshake failed.");
      while(1) {
        // Hang on failure
      }  
    }
}

void sendInit(){
  if (!connected){
        StaticJsonDocument<50> doc;
        doc["MessageType"] = "I";
        doc["MessageId"] = 0;
        doc["RecipientId"] = 0;
        String output;
        serializeJson(doc,output);
        webSocketClient.sendData(output);
        connected = true;
    }
}

void loop() {

  
  String data;

  if (client.connected()) {
    sendInit();
  
    
    webSocketClient.getData(data);
    if (data.length() > 0) {
      handleData(data);
    }

    
    } 
  else {
    Serial.println("Client disconnected.");
    socketConnect();
  }
  // motor.update();
  // if (motor.ready) {

  // }
}

void handleData (String data){
  StaticJsonDocument<500> jsonBuffer;
  DeserializationError error = deserializeJson(jsonBuffer, data);
  if (error) {
      return;
    }
    else if (jsonBuffer["MessageType"] == "M"){
      
      motorSettings_t settings;
      String state = jsonBuffer["Data"]["State"].as<String>();
      
      if(state=="STOP"){
          settings.state = STOP;
      }

      else if (state=="FORWARD" ){
          settings.state = FORWARD;
          settings.target = jsonBuffer["Data"]["Distance"].as<float>();
     
      // motor.setMotor(settings);
          // while(!motor.ready) {motor.update()};
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
          calcAngle(-1*settings.target);
      }

      else if (state=="RIGHT"){
          settings.state = RIGHT;
          settings.target = jsonBuffer["Data"]["Angle"].as<float>();
          calcAngle(settings.target);
      }

      else {
          return;
      }

      jsonBuffer["Data"]["X"] = currentPos.X;
      jsonBuffer["Data"]["Y"] = currentPos.Y; 
      jsonBuffer["Data"]["Angle"] = currentPos.Angle;
      String output;
      serializeJson(jsonBuffer, output);
      writeToMaster(output);
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
  if(0<=currentPos.Angle || currentPos.Angle < 45 || 135<= currentPos.Angle || currentPos.Angle < 180 || 180<= currentPos.Angle || currentPos.Angle < 225 || 315<= currentPos.Angle || currentPos.Angle <360){
     currentPos.X += distance*cos( currentPos.Angle*PI/180 );
     currentPos.Y += distance*sin( currentPos.Angle*PI/180  );
     Serial.println(cos(currentPos.Angle));
     Serial.println(sin(currentPos.Angle));
         
  }
  else if (45<= currentPos.Angle || currentPos.Angle < 135 || 225<= currentPos.Angle || currentPos.Angle < 315){
    currentPos.Y += distance*cos( currentPos.Angle *PI/180  );
     currentPos.X += distance*sin( currentPos.Angle *PI/180  );
     Serial.println(cos(currentPos.Angle));
     Serial.println(sin(currentPos.Angle));
  }
}

void calcAngle(float angle){
  if (currentPos.Angle == 0){
      currentPos.Angle = angle;
    }
    
    else if (currentPos.Angle - angle < 0 ){
      currentPos.Angle = (currentPos.Angle - angle) + 360;
    }
    
    else if(currentPos.Angle - angle > 360){
      currentPos.Angle = (currentPos.Angle - angle) - 360; 
    }
    else{
      currentPos.Angle = (currentPos.Angle - angle);
    }
}

void writeToMaster(String msg){
  if (client.connected()) {
     webSocketClient.sendData(msg);
  }
  else {
    socketConnect();
  }
}
