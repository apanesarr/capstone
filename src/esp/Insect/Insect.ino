#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "ArduinoJson.h"
#include "CarMotorCtrl.h"

#define HTTP_REST_PORT 80
#define WIFI_RETRY_DELAY 500
#define MAX_WIFI_INIT_RETRY 50

const char* ssid = "A3";
const char* password = "12345678";


ESP8266WebServer http_rest_server(HTTP_REST_PORT);

//MotorControl motor;
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
void calcXY(float distance){
  if(0<=currentPos.Angle || currentPos.Angle < 45 || 135<= currentPos.Angle || currentPos.Angle < 180 || 180<= currentPos.Angle || currentPos.Angle < 225 || 315<= currentPos.Angle || currentPos.Angle <360){
     currentPos.X += distance*cos(currentPos.Angle);
     currentPos.Y += distance*sin(currentPos.Angle);
     Serial.println(cos(currentPos.Angle));
     Serial.println(sin(currentPos.Angle));
         
  }
  else if (45<= currentPos.Angle < 135 || 225<= currentPos.Angle < 315){
    currentPos.Y += distance*cos(currentPos.Angle);
     currentPos.X += distance*sin(currentPos.Angle);
     Serial.println(cos(currentPos.Angle));
     Serial.println(sin(currentPos.Angle));
  }
}

void calcAngle(float angle){
  if (currentPos.Angle - angle< 0 ){
    currentPos.Angle = (currentPos.Angle - angle) + 360;
  }
  else if(currentPos.Angle - angle > 360){
    currentPos.Angle = (currentPos.Angle - angle) - 360; 
  }
  else{
    currentPos.Angle = (currentPos.Angle - angle);
  }
}

void setup() {
  Serial.begin(115200);
  if (init_wifi() == WL_CONNECTED) {
    Serial.print("Connetted to ");
    Serial.print(ssid);
    Serial.print("--- IP: ");
    Serial.println(WiFi.localIP());
  }
  config_rest_server_routing();
  http_rest_server.begin(); 
//  motor.init();
  currentPos.X = 0.0;
  currentPos.Y = 0.0;
  currentPos.Angle = 0.0;
}

void loop() {
  http_rest_server.handleClient();
}

void config_rest_server_routing() {
    http_rest_server.on("/", HTTP_PUT, handleJSON);
    http_rest_server.on("/", HTTP_GET, handleJSON);
}

void handleJSON(){
  StaticJsonDocument<500> jsonBuffer;
  String post_body = http_rest_server.arg("plain");
  DeserializationError error = deserializeJson(jsonBuffer, post_body);
   if (error) {
       http_rest_server.send(500, "text/plain", "Deserialization Failed");
    return;
  }
  if (jsonBuffer["MessageType"] == "I"){
    String output;
    serializeJson(jsonBuffer, output);
    http_rest_server.send(200,"application/json", output);
  }
  else if (jsonBuffer["MessageType"] == "M"){
    motorSettings_t settings;
//    const char* state = jsonBuffer["Data"]["State"];
    String state = jsonBuffer["Data"]["State"].as<String>();
    Serial.println("Got here");
    Serial.println(state);
//    Serial.println(state1=="FORWARD");
//    Serial.println(strcmp(state,"FORWARD"));
    if(state=="STOP"){
      settings.state = STOP;
    }
    else if (state=="FORWARD" ){
      Serial.println("IT WORKS");
      settings.state = FORWARD;
      settings.target = jsonBuffer["Data"]["Distance"].as<float>();
      Serial.println(settings.target);
//      motor.setMotor(settings);
//      while(!motor.update());
      calcXY(settings.target);
      
    }
    else if (state=="REVERSE"){
      settings.state = REVERSE;
      settings.target = jsonBuffer["Data"]["Distance"].as<float>();
//      motor.setMotor(settings);
//      while(!motor.update());
      calcXY(settings.target);
    }
    else if (state=="LEFT"){
      settings.state = LEFT;
      settings.target = jsonBuffer["Data"]["Angle"].as<float>();
//      motor.setMotor(settings);
//      while(!motor.update());
      calcAngle(-1*settings.target);
    }
    else if (state=="RIGHT"){
      settings.state = RIGHT;
      settings.target = jsonBuffer["Data"]["Angle"].as<float>();
//      motor.setMotor(settings);
//      while(!motor.update());
      Serial.println(settings.target);
      calcAngle(settings.target);
    }
    else{
      http_rest_server.send(500, "text/plain", "Invalid Motor State");
      return;
    } 
    jsonBuffer["Data"]["X"] = currentPos.X;
    jsonBuffer["Data"]["Y"] = currentPos.Y; 
    jsonBuffer["Data"]["Angle"] = currentPos.Angle;
    String output;
    serializeJson(jsonBuffer, output);
    http_rest_server.send(200,"application/json", output);
  }
  else if (jsonBuffer["MessageType"] == "R"){
    jsonBuffer["Data"]["X"] = currentPos.X;
    jsonBuffer["Data"]["Y"] = currentPos.Y;
    jsonBuffer["Data"]["Angle"] = currentPos.Angle;
    String output;
    serializeJson(jsonBuffer, output);
    http_rest_server.send(200,"application/json", output);
  }
  else if (jsonBuffer["MessageType"] == "T"){
    jsonBuffer["Data"]["Temperature"] = 10.0;
    jsonBuffer["Data"]["Humidity"] = 80.0;
    String output;
    serializeJson(jsonBuffer, output);
    http_rest_server.send(200,"application/json", output);
  }
  else{
    http_rest_server.send(500, "text/plain", "Non Match case");
    }
  

}
