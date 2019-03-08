#include <CommsToolbox.h>
#include "RF24.h"
#include "RF24Network.h"
#include "CarMotorCtrl.h"

#define nodeID 0

RF24 radio1(7, 8);
RF24Network network(radio1);
const char s[2] = ",";

uint16_t msg = 45;

struct sendMsg {
  unsigned int msg_id;
  uint16_t to_id;
  char value [32];
  char type;
};

sendMsg convertToMsg (String str) {
  char arr[38];
  sendMsg msg;
  uint8_t part = 0; 
  str.toCharArray(arr, 40);
  char *token;
  char *split [4];
  token = strtok(arr,s);
  while( token != NULL ) {
    split[part] = token;
    part++;
    token = strtok(NULL, s);
  }
  Serial.println("");
  msg.msg_id = (unsigned int)split[0][0] - 48;
  msg.to_id = (uint16_t)split[1][0] - 48;
  msg.type = split[2][0];
  strncpy (msg.value, split[3], sizeof(msg.value));
  motorSettings_t settings;
  switch(msg.type){
    case MOTOR_TYPE:
     Serial.println(msg.value);
      if(strcmp(msg.value, "FORWARD")){
        settings.state = FORWARD;
        settings.targetAngle = 0;
        Serial.println("GOING FORWARD");
      }

      else if( strcmp(msg.value,"BACKWARD")){
        settings.state = REVERSE;
        settings.targetAngle = 0;
        Serial.println("GOING BACK");
      }
      break;
    default:
      Serial.println("Error");
      break;
  }
  memcpy(&msg.value, &settings, sizeof(settings));
  return msg;
}

void setup() {
  Serial.begin(115200);
  radio1.begin();
  network.begin(100,nodeID);
  Serial.print("Starting Master");
}

void loop() {
  network.update();

  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    sendMsg data = convertToMsg(input);
    RF24NetworkHeader header(data.to_id,data.type);
    payloadMsg payload;
    payload.msg_id = data.msg_id;
    strcpy(payload.data, data.value);
    network.write(header,&payload,sizeof(payload));
  }
  
  while(network.available()){
    RF24NetworkHeader header;
    payloadMsg payload;
    network.read(header,&payload, sizeof(payloadMsg));
    Serial.print(payload.msg_id);
    Serial.print(",");
    Serial.print(payload.data);
  }
}
