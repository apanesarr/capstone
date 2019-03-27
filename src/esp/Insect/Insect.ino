#include <ESP8266WiFi.h>
#include <math.h>
#include "ArduinoJson.h"
#include "CarMotorCtrl.h"
#include "Comms.h"
#include "DHTesp.h"

#define DHTPIN 16
#define DHTTYPE 11

pos currentPos;
MotorControl motor;
motorSettings_t settings;
Comms comms;
DHTesp dht;

void setup(){
  Serial.begin(115200);
  dht.setup(DHTPIN, DHTesp::DHT11);
  Serial.println(dht.getHumidity());
  Serial.println(dht.getTemperature()); 
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
    // comms.message["MessageType"] = "R";
    // comms.message["Data"]["X"] = currentPos.X;
    // comms.message["Data"]["Y"] = currentPos.Y;
    // comms.message["Data"]["Angle"] = currentPos.Angle;
    // comms.transmit();
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
      calcXY(settings.target);
      Serial.print("Motor Ready ------->>>>");
      Serial.println(motor.ready);

    }

    else if (state=="LEFT"){
      settings.state = LEFT;
      String test = comms.message["Data"]["Angle"];
      settings.target = comms.message["Data"]["Angle"].as<float>();
      Serial.print(test);
      Serial.print("-------->>");
      Serial.println(settings.target);
      motor.setMotor(settings);
      calcAngle(settings.target);
      Serial.print("Motor Ready ------->>>>");
      Serial.println(motor.ready);
    }

    else if (state=="RIGHT"){
      settings.state = RIGHT;
      settings.target = comms.message["Data"]["Angle"].as<float>();
      String test = comms.message["Data"]["Angle"];
      Serial.print(test);
      Serial.print("-------->>");
      Serial.println(settings.target);
      motor.setMotor(settings);
      calcAngle(-1 * settings.target);
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
    comms.message["Data"]["Temperature"] = dht.getTemperature();
    comms.message["Data"]["Humidity"] = dht.getHumidity();
    comms.message["Data"]["X"] = currentPos.X;
    comms.message["Data"]["Y"] = currentPos.Y;
    comms.message["Data"]["Angle"] = currentPos.Angle;
    comms.transmit();
  }
  else {
    return;
  }
}

void sendReady(){
    Serial.println("Sending Ready!!!!!:))))");
    comms.messageReady["MessageType"] = "R";
    comms.messageReady["RecipientId"] = 0;
    comms.messageReady["Data"]["X"] =  currentPos.X;
    comms.messageReady["Data"]["Y"] = currentPos.Y;
    comms.messageReady["Data"]["Angle"] = currentPos.Angle;
    comms.transmitReady();
}

void calcXY(float distance){
  if(0<=currentPos.Angle || currentPos.Angle < 45 || 135<= currentPos.Angle
      || currentPos.Angle < 180 || 180<= currentPos.Angle
      || currentPos.Angle < 225 || 315<= currentPos.Angle
      || currentPos.Angle <360){
     currentPos.X += distance*cos( currentPos.Angle*PI/180 );
     currentPos.Y += distance*sin( currentPos.Angle*PI/180  );
     // Serial.println(cos(currentPos.Angle));
     // Serial.println(sin(currentPos.Angle));

  }
  else if (45<= currentPos.Angle || currentPos.Angle < 135
      || 225<= currentPos.Angle || currentPos.Angle < 315){
    currentPos.Y += distance*cos( currentPos.Angle *PI/180  );
     currentPos.X += distance*sin( currentPos.Angle *PI/180  );
     // Serial.println(cos(currentPos.Angle));
     // Serial.println(sin(currentPos.Angle));
  }
}

void calcAngle(float angle){
  if (angle > 360) angle -= 360;
  if (angle < 0)   angle += 360;
  currentPos.Angle = angle;
}
