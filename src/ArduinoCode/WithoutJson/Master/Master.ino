#include <CarMotorCtrl.h>
#include <CommsToolbox.h>
#include "RF24Network.h"
#include "RF24.h"
#include "RF24Mesh.h"
#include <SPI.h>

RF24 radio(7,8);
RF24Network network(radio);
RF24Mesh mesh(radio,network);

struct incoming {
  char type;
  String value;
  unsigned int ID;
  unsigned int msg_id;
};


incoming splitString(String s){
  int start = 0;
  int part = 0;
  int ends;
  String cmds[4];
  for (unsigned int i = 0; i < s.length(); i++){
    if (s[i] == ','){
      ends = i;
      cmds[part] = s.substring(start,ends);
      part++;
      start = i;
     }
  }
  cmds[3] = s.substring(start+1, s.length());
  incoming split;
  split.msg_id = cmds[0].toInt();
  split.ID = (cmds[1].substring(1,cmds[1].length())).toInt();
  split.type = (char)cmds[2][1];
  split.value = cmds[3];
  return split;
}

bool checkIfConnected (unsigned int id) {
  for (int i = 0; i < mesh.addrListTop; i++){
    if (mesh.addrList[i].nodeID == id){
      return true;
    }
  }
  return false;
}

void setup() {
  Serial.begin(115200);
//  Serial.begin(115200);
  mesh.setNodeID(0);
  mesh.begin();
  Serial.println("Starting master");
}
 
void loop() {
  mesh.update();
  mesh.DHCP(); 
  String serialdata = "";
  if (Serial.available() > 0){
    serialdata = Serial.readStringUntil('\0');  
    incoming data = splitString(serialdata);
    if ( data.type == TEMP_TYPE ){
//      if (checkIfConnected(data.ID)){
        payloadMsg_T payload;
        payload.msg_id = data.msg_id;
        int retry = 10;
        while( (!writeToInsect(&payload,data.type, data.ID)) && (retry > 0) ){
          Serial.println("Didnt send");
          delay(100);
          retry--;
        }
//      }
//      else {
//        Serial.println("E-2-Not Connected");
//      }
    }
    else if(data.type == SET_GYRO_TYPE){
      if (checkIfConnected(data.ID)){
        int retry = 10;
        payloadMsg_G set_gyro_s;
        while ( ! writeToInsect_G(&set_gyro_s,data.type, data.ID) && (retry > 0)){
          set_gyro_s.msg_id = data.ID;
          set_gyro_s.gyro = (data.value).toFloat();
        }
      }
      else {
        Serial.println("E-2-Not Connected");
      }
    }
    else if (data.type == GET_GYRO_TYPE){
      if (checkIfConnected(data.ID)){
        payloadMsg_G get_gyro_s;
        int retry = 0;
        while(writeToInsect_G(&get_gyro_s,data.type, data.ID) && (retry > 0)){  
          get_gyro_s.msg_id = data.msg_id;
        }
      }
      else {
        Serial.println("E-2-Not Connected");
      }
    }
    else if (data.type == STOP_TYPE){
      payloadMsg_M payload_SS;
      payload_SS.msg_id = data.msg_id;
//      payload_SS.state = STOP;
      writeToInsect_M(&payload_SS,data.type, data.ID);
    }
    else if (data.type == MOTOR_TYPE){
      payloadMsg_M payload_SS;
      payload_SS.msg_id = data.msg_id;
//      payload_SS.state = STOP;
      writeToInsect_M(&payload_SS,data.type, data.ID); 
    }
    else {
      Serial.println("E,1,Not Detected");
    }
 
  while (network.available()){
    RF24NetworkHeader header;
    network.peek(header);   
    if (header.type == INIT){
        payloadMsg_T payload_I;
        network.read(header, &payload_I, sizeof(payload_I));
        Serial.println("INITED Conncection");
        Serial.println("added node to");
      }
      else if(header.type == TEMP_TYPE){
        payloadMsg_T payload_T;
        network.read(header, &payload_T, sizeof(payload_T));
        Serial.print(payload_T.temp);
        Serial.print(",");
        Serial.println(payload_T.msg_id);
      }
      else if(header.type == SET_GYRO_TYPE){
        payloadMsg_G set_gyro;
        network.read(header, &set_gyro, sizeof(set_gyro));
        Serial.print(set_gyro.msg_id);
        Serial.print(",");
        Serial.println(set_gyro.gyro);
      }
      else if(header.type == GET_GYRO_TYPE){
        payloadMsg_G get_gyro;
        network.read(header, &get_gyro, sizeof(get_gyro));
        Serial.print(get_gyro.msg_id);
        Serial.print(",");
        Serial.println(get_gyro.gyro);
      }
      else if (header.type == STOP_TYPE){
        payloadMsg_M payload_S;
        network.read(header, &payload_S, sizeof(payload_S));
        Serial.print(payload_S.msg_id);
        Serial.print(",");
//        Serial.println(payload_S.state);
      }
      else if (header.type == MOTOR_TYPE){
        payloadMsg_M payload_M;
        network.read(header, &payload_M, sizeof(payload_M));
        Serial.print(payload_M.msg_id);
        Serial.print(",");
//        Serial.println(payload_M.state);
      }
      else {
        void * message;
        size_t maxlen = 0;
        network.read(header, message, maxlen);
        Serial.print("E-3-Recieved message of unknown type");
      }
    }  
  }
}

bool writeToInsect (payloadMsg_T *payload, char type, uint16_t id){
  if (!mesh.write(payload, type ,sizeof(payloadMsg_T),id)) {
    if ( ! mesh.checkConnection() ) { 
      mesh.renewAddress(); 
     } 
     else {
      return false;
     }
     return false;
   } 
     else {
      return true;
     }
 }

 bool writeToInsect_G(payloadMsg_G *payload, char type, uint16_t id){
  if (!mesh.write(payload, type ,sizeof(payloadMsg_T),id)) {
    if ( ! mesh.checkConnection() ) { 
      mesh.renewAddress(); 
     } 
     else {
      // resend
      return false;
     }
     return false;
   } 
     else {
      return true;
     }
 }

 bool writeToInsect_M (payloadMsg_M *payload, char type, uint16_t id){
  if (!mesh.write(payload, type ,sizeof(payloadMsg_T),id)) {
    if ( ! mesh.checkConnection() ) { 
      mesh.renewAddress(); 
    } 
    else {
      return false;
    }
    return false;
  } 
  else {
    return true;
  }
 }