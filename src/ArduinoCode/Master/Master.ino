#include "CommsToolbox.h"
#include "RF24.h"
#include "RF24Network.h"
#include "CarMotorCtrl.h"
#include "ArduinoJson.h"

#define nodeID 0

RF24 radio1(7, 8);
RF24Network network(radio1);

void setup() {
  Serial.begin(115200);
  
  radio1.begin();
  network.begin(90,nodeID);

  Serial.println("Starting Master");
  
  while(Serial.available() > 0) Serial.readStringUntil('\n');
}

void loop() {
  network.update();
  
  // if (Serial.available() > 0) {
  //   String input = Serial.readStringUntil('\n');

  //   Serial.println("Input:");
  //   Serial.println(input);

  //   SendMessage msg = handlejson(input);
  //   if ( msg.msgType != FAILURE ){
  //     payloadMsg payload;
  //     payload.msg_id = msg.msgID;
  //     strncpy(payload.data, msg.value, sizeof(payload.data));
  //     if ( safeSend(msg.msgType, msg.recID, &payload, 10) ){
  //       sendErr(msg,"Message Sent Sucessfully",0);
  //     }
  //     else{
  //       sendErr(msg, "Message Failed to send",3);
  //     }
  //   }
  // }
  if(network.available()){
    RF24NetworkHeader header;
    payloadMsg payload;
    network.read(header,&payload, sizeof(payloadMsg));
    Serial.println(header.type);
    // switch (header.type){
    //   case INIT:
    //     printinit(&header);
    //     break;
    //   case TEMP_TYPE:
    //     TempData temp;
    //     memcpy(&temp,&payload.data,sizeof(payload.data));
    //     printTemp(&temp,&payload,&header);
    //     break;
    //   case MOTOR_TYPE:
    //     motorSettings_t settings;
    //     memcpy(&settings,&payload.data,sizeof(payload.data));
    //     String MotorState;
    //     switch (settings.state) {
    //       case FORWARD:
    //         MotorState = "FORWARD";
    //         break;
    //       case REVERSE:
    //         MotorState = "REVERSE";
    //         break;
    //       case LEFT:
    //         MotorState = "LEFT";
    //         break;
    //       case RIGHT:
    //         MotorState = "RIGHT";
    //         break;
    //       case STOP:
    //         MotorState = "STOP";
    //         break;
    //       default:
    //         SendMessage msg;
    //         msg.msgID = payload.msg_id;
    //         strncpy(msg.value,payload.data, sizeof(payload.data));
    //         sendErr(msg,"Invalid Incoming Motor Type",5);
    //         return;
    //     }
    //     printMotor(&settings, &payload, &header);
    //     break;
    //   case MOTOR_READY:
    //     motorSettings_t setting;
    //     memcpy(&setting,&payload.data,sizeof(payload.data));
    //     printMotor(&setting, &payload, &header);
    //     break;
    //   default:
    //     SendMessage msg;
    //     msg.msgID = payload.msg_id;
    //     strncpy(msg.value,payload.data, sizeof(payload.data));
    //     sendErr(msg, "Invalid Incoming Message Type", 4);
    //     return;
    // }
    
  }
}

bool safeSend(char type,unsigned int to,payloadMsg *payload, int tryAgain ){
  int i = 0;
  while (!sendToInsect(type,to,payload) && i < tryAgain){
     i++;
  }
  if (i < tryAgain){
    return true;
  }
  return false;
}

bool sendToInsect (char type, unsigned int to, payloadMsg *payload) {
  RF24NetworkHeader header(to,type);
  return network.write(header, payload, sizeof(*payload));
}


SendMessage handlejson(String json){
  StaticJsonDocument<200> doc;
  StaticJsonDocument<200> data;
  Serial.print("Json");
  Serial.print(json); 
  SendMessage msg;
  DeserializationError error = deserializeJson(doc, json);
   if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  JsonVariant variant = doc.as<JsonVariant>();

  msg.msgID = variant["MessageId"].as<unsigned int>();
  msg.recID = variant["RecipientId"].as<unsigned int>();
  msg.msgType = variant["MessageType"].as<String>()[0];
  
  data = variant["Data"].as<JsonObject>();
  
  if ( msg.msgType == MOTOR_TYPE){
    const char* state = data["State"];
    motorSettings_t settings;
    if(strcmp(state,"STOP")){
      settings.state = STOP;
    }
    else if (strcmp(state,"FORWARD") ){
      settings.state = FORWARD;
      settings.targetAngle = data["Distance"];
    }
    else if (strcmp(state,"REVERSE")){
      settings.state = REVERSE;
      settings.targetAngle = data["Distance"];
    }
    else if (strcmp(state,"LEFT")){
      settings.state = LEFT;
      settings.targetAngle = data["Angle"];
    }
    else if (strcmp(state,"RIGHT")){
      settings.state = RIGHT;
      settings.targetAngle = data["Angle"];
    }
    else{
      sendErr(msg,"Invalid Motor Type",2);
      msg.msgType = FAILURE;
    }
    memcpy(&msg.value, &settings, sizeof(settings));
  }
  else if (msg.msgType == INIT){
    initData init;
    init.msgID = data["Initialize"];
    memcpy(&msg.value,&init,sizeof(msg.value));
  }
  else if (msg.msgType == MOTOR_READY){
    return msg;
  }
  else{
    sendErr(msg,"Invalid Message type",1);
    msg.msgType = FAILURE;
  }
  return msg;
}

void sendErr (SendMessage msg, String errM, int errType){
  StaticJsonDocument<512> err;

  err["MessageId"] = msg.msgID;
  err["RecipientId"] = msg.recID;
  err["MessageType"] = msg.msgType;
  err["Data"]["ErrorMessage"] = errM;
  err["Data"]["ErrorType"] = errType;
  serializeJson(err, Serial);
  Serial.println("");
}

void printTemp(TempData *temp, payloadMsg *payload,RF24NetworkHeader *header){
  StaticJsonDocument<200> Tempbody;

  Tempbody["MessageId"] = payload->msg_id;
  Tempbody["RecipientId"] = header->from_node;
  Tempbody["MessageType"] = header->type;
  Tempbody["Data"]["Temperature"] = temp->Temp;
  Tempbody["Data"]["Humidity"] = temp->Humidity;
  serializeJson(Tempbody, Serial);
  Serial.println("");
}

void printMotor( motorSettings_t *motor, payloadMsg *payload, RF24NetworkHeader *header){
  StaticJsonDocument<200> Motorbody;

  Motorbody["MessageId"] = payload->msg_id;
  Motorbody["RecipientId"] = header->from_node;
  Motorbody["MessageType"] = header->type;
  Motorbody["Data"]["X"] = 0;
  Motorbody["Data"]["Y"] = 0;
  Motorbody["Data"]["Angle"] = motor->targetAngle;
  serializeJson(Motorbody, Serial);
  Serial.println("");
}

void printinit(RF24NetworkHeader * header) {
  StaticJsonDocument<200> jsonDoc;

  jsonDoc["MessageId"]          = 0;
  jsonDoc["RecipientId"]        = header->from_node;
  jsonDoc["MessageType"]        = header->type;
  jsonDoc["Data"]["Initialize"] = 0;
  serializeJson(jsonDoc, Serial);
  Serial.println("");
}