#include <ESP8266WiFi.h>
#include "Comms.h"
#include <math.h>
#include "ArduinoJson.h"
#include "CarMotorCtrl.h"
#include "parameters.h"

#include "DHTesp.h"

#define DHTPIN 16
#define DHTTYPE 11

#define ROOM_HUMIDITY 45

pos currentPos;
MotorControl motor;
motorSettings_t settings;
Comms comms;
DHTesp dht;

void setup(){
  Serial.begin(115200);
  dht.setup(DHTPIN, DHTesp::DHT11);
  comms.setup();
  motor.init();
  
}

void loop() {


  if (comms.update() && motor.ready){
    processMsg();
    Serial.println("NEW MESSAGE AND MOTOR READY");
  } 
  motor.update();
  
  if (motor.ready &&  (comms.ready) ){
    Serial.println("MOTOR READY AND SEND READY");
    sendReady();
    comms.ready = false;
  }

  if(motor.ready){
    
    // Serial.print("SEND ME MESSAGE PLZ");
    // Serial.println(comms.ready);

  }
  
}

void processMsg (){
  if (comms.message["MessageType"]=="I"){
    Serial.println("recived I");
  }
  else if (comms.message["MessageType"] == "M"){
    Serial.print("Motor Ready ------->>>>");
    Serial.println(motor.ready);
    String state = comms.message["Data"]["State"].as<String>();
    if(state=="STOP"){
      settings.state = STOP;
      motor.setMotor(settings);
    }

    else if (state=="FORWARD" ){
      settings.state = FORWARD;
      // Serial.println( comms.message["Data"]["Distance"] );
      String test = comms.message["Data"]["Distance"];
      settings.target = comms.message["Data"]["Distance"].as<float>();
      Serial.print(test);
      Serial.print("-------->>");
      Serial.println(settings.target);
      motor.setMotor(settings);
      calcXY(settings.target);
      Serial.print("Motor Ready ------->>>>");
      Serial.println(motor.ready);
    }

    else if (state=="REVERSE"){
      settings.state = REVERSE;
      settings.target = comms.message["Data"]["Distance"].as<float>();
      motor.setMotor(settings);
      calcXY((-1) * settings.target);
      Serial.print("Motor Ready ------->>>>");
      Serial.println(motor.ready);

    }

    else if (state=="ROTATE"){
      settings.state = ROTATE;
      String test = comms.message["Data"]["Angle"];
      settings.target = comms.message["Data"]["Angle"].as<float>();
      Serial.print(test);
      Serial.print("-------->>");
      Serial.println(settings.target);
      motor.setMotor(settings);
      Serial.print("Motor Ready ------->>>>");
      Serial.println(motor.ready);
    }

    else {
      return;
    }
    comms.ready = true;
  }
  else if (comms.message["MessageType"] == "R"){
    sendReady();
  }
  else if (comms.message["MessageType"] == "T"){

#ifdef GORDO_1
    comms.message["Data"]["Temperature"] = dht.getTemperature();
    comms.message["Data"]["Humidity"] = dht.getHumidity();
#else
    comms.message["Data"]["Temperature"] = motor.imu.getTemp();
    comms.message["Data"]["Humidity"] = ROOM_HUMIDITY;
#endif

    comms.message["Data"]["X"] = currentPos.X;
    comms.message["Data"]["Y"] = currentPos.Y;
    comms.message["Data"]["Angle"] = motor.yaw;
    comms.transmit();
  }
  else {
    return;
  }
}

void sendReady(){
    Serial.println("Sending Ready!!!!!:))))");
    comms.messageReady["MessageType"] = "R";
    comms.messageReady["RecipientId"] = REC_ID;
    comms.messageReady["Data"]["X"] =  currentPos.X;
    comms.messageReady["Data"]["Y"] = currentPos.Y;
    comms.messageReady["Data"]["Angle"] = motor.yaw;
    comms.transmitReady();
}

void calcXY(float distance){
  if(0<=motor.yaw || motor.yaw < 45 || 135<= motor.yaw
      || motor.yaw < 180 || 180<= motor.yaw
      || motor.yaw < 225 || 315<= motor.yaw
      || motor.yaw <360){
     currentPos.X += distance*cos( motor.yaw*PI/180 );
     currentPos.Y += distance*sin( motor.yaw*PI/180  );
     // Serial.println(cos(motor.yaw));
     // Serial.println(sin(motor.yaw));

  }
  else if (45<= motor.yaw || motor.yaw < 135
      || 225<= motor.yaw || motor.yaw < 315){
    currentPos.Y += distance*cos( motor.yaw *PI/180  );
     currentPos.X += distance*sin( motor.yaw *PI/180  );
     // Serial.println(cos(motor.yaw));
     // Serial.println(sin(motor.yaw));
  }
}
